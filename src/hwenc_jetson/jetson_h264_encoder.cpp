/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#include "jetson_h264_encoder.h"

#include <limits>
#include <string>

#include "common_video/libyuv/include/webrtc_libyuv.h"
#include "nvbuf_utils.h"
#include "rtc/native_buffer.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/time_utils.h"
#include "system_wrappers/include/metrics.h"
#include "third_party/libyuv/include/libyuv/convert.h"
#include "third_party/libyuv/include/libyuv/convert_from.h"
#include "third_party/libyuv/include/libyuv/video_common.h"

#define H264HWENC_HEADER_DEBUG 0
#define INIT_ERROR(cond, desc)                 \
  if (cond) {                                  \
    RTC_LOG(LS_ERROR) << __FUNCTION__ << desc; \
    Release();                                 \
    return WEBRTC_VIDEO_CODEC_ERROR;           \
  }

namespace {
struct nal_entry {
  size_t offset;
  size_t size;
};

const int kLowH264QpThreshold = 34;
const int kHighH264QpThreshold = 40;

}  // namespace

JetsonH264Encoder::JetsonH264Encoder(const cricket::VideoCodec& codec)
    : callback_(nullptr),
      decoder_(nullptr),
      converter_(nullptr),
      encoder_(nullptr),
      bitrate_adjuster_(.5, .95),
      configured_framerate_(30),
      configured_width_(0),
      configured_height_(0),
      use_mjpeg_(false) {}

JetsonH264Encoder::~JetsonH264Encoder() {
  Release();
}

int32_t JetsonH264Encoder::InitEncode(const webrtc::VideoCodec* codec_settings,
                                      int32_t number_of_cores,
                                      size_t max_payload_size) {
  RTC_LOG(LS_INFO) << __FUNCTION__ << " Start";
  RTC_DCHECK(codec_settings);
  RTC_DCHECK_EQ(codec_settings->codecType, webrtc::kVideoCodecH264);

  int32_t release_ret = Release();
  if (release_ret != WEBRTC_VIDEO_CODEC_OK) {
    return release_ret;
  }

  width_ = codec_settings->width;
  height_ = codec_settings->height;
  target_bitrate_bps_ = codec_settings->startBitrate * 1000;
  key_frame_interval_ = codec_settings->H264().keyFrameInterval;
  bitrate_adjuster_.SetTargetBitrateBps(target_bitrate_bps_);
  framerate_ = codec_settings->maxFramerate;

  RTC_LOG(LS_INFO) << "InitEncode " << framerate_ << "fps "
                   << target_bitrate_bps_ << "bit/sec　"
                   << codec_settings->maxBitrate << "kbit/sec　";

  // Initialize encoded image. Default buffer size: size of unencoded data.
  encoded_image_._completeFrame = true;
  encoded_image_._encodedWidth = 0;
  encoded_image_._encodedHeight = 0;
  encoded_image_.set_size(0);
  encoded_image_.timing_.flags =
      webrtc::VideoSendTiming::TimingFrameFlags::kInvalid;
  encoded_image_.content_type_ =
      (codec_settings->mode == webrtc::VideoCodecMode::kScreensharing)
          ? webrtc::VideoContentType::SCREENSHARE
          : webrtc::VideoContentType::UNSPECIFIED;

  decoder_ = NvJPEGDecoder::createJPEGDecoder("jpegdec");
  INIT_ERROR(!decoder_, "Failed to createJPEGDecoder");

  RTC_LOG(LS_INFO) << __FUNCTION__ << " End";
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t JetsonH264Encoder::Release() {
  RTC_LOG(LS_INFO) << __FUNCTION__ << " Start";
  JetsonRelease();
  if (decoder_) {
    delete decoder_;
    decoder_ = nullptr;
  }
  RTC_LOG(LS_INFO) << __FUNCTION__ << " End";
  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t JetsonH264Encoder::JetsonConfigure() {
  int ret = 0;

  if (use_mjpeg_) {
    enc0_buffer_queue_ = new std::queue<NvBuffer*>;

    converter_ = NvVideoConverter::createVideoConverter("conv");
    INIT_ERROR(!decoder_, "Failed to createVideoConverter");

    ret = converter_->setOutputPlaneFormat(
        decode_pixfmt_, raw_width_, raw_height_, V4L2_NV_BUFFER_LAYOUT_PITCH);
    INIT_ERROR(ret < 0, "Failed to converter setOutputPlaneFormat");

    ret = converter_->setCapturePlaneFormat(
        V4L2_PIX_FMT_YUV420M, width_, height_, V4L2_NV_BUFFER_LAYOUT_PITCH);
    INIT_ERROR(ret < 0, "Failed to converter setCapturePlaneFormat");

    ret = converter_->output_plane.setupPlane(V4L2_MEMORY_DMABUF, 1, false,
                                              false);
    INIT_ERROR(ret < 0, "Failed to setupPlane at converter output_plane");

    ret = converter_->capture_plane.setupPlane(V4L2_MEMORY_MMAP, 10, false,
                                               false);
    INIT_ERROR(ret < 0, "Failed to setupPlane at converter capture_plane");

    ret = converter_->output_plane.setStreamStatus(true);
    INIT_ERROR(ret < 0, "Failed to setStreamStatus at converter output_plane");

    ret = converter_->capture_plane.setStreamStatus(true);
    INIT_ERROR(ret < 0, "Failed to setStreamStatus at converter capture_plane");

    converter_->capture_plane.setDQThreadCallback(
        ConvertFinishedCallbackFunction);
  }

  encoder_ = NvVideoEncoder::createVideoEncoder("enc0");
  INIT_ERROR(!encoder_, "Failed to createVideoEncoder");

  ret = encoder_->setCapturePlaneFormat(V4L2_PIX_FMT_H264, width_, height_,
                                        4 * 1024 * 1024);
  INIT_ERROR(ret < 0, "Failed to encoder setCapturePlaneFormat");

  ret = encoder_->setOutputPlaneFormat(V4L2_PIX_FMT_YUV420M, width_, height_);
  INIT_ERROR(ret < 0, "Failed to encoder setOutputPlaneFormat");

  ret = encoder_->setBitrate(bitrate_adjuster_.GetAdjustedBitrateBps());
  INIT_ERROR(ret < 0, "Failed to setBitrate");

  ret = encoder_->setProfile(V4L2_MPEG_VIDEO_H264_PROFILE_HIGH);
  INIT_ERROR(ret < 0, "Failed to setProfile");

  ret = encoder_->setLevel(V4L2_MPEG_VIDEO_H264_LEVEL_5_1);
  INIT_ERROR(ret < 0, "Failed to setLevel");

  ret = encoder_->setRateControlMode(V4L2_MPEG_VIDEO_BITRATE_MODE_CBR);
  INIT_ERROR(ret < 0, "Failed to setRateControlMode");

  ret = encoder_->setIDRInterval(key_frame_interval_);
  INIT_ERROR(ret < 0, "Failed to setIDRInterval");

  ret = encoder_->setIFrameInterval(key_frame_interval_);
  INIT_ERROR(ret < 0, "Failed to setIFrameInterval");

  ret = encoder_->setFrameRate(framerate_, 1);
  INIT_ERROR(ret < 0, "Failed to setFrameRate");

  //V4L2_ENC_HW_PRESET_ULTRAFAST が推奨値だけど MEDIUM もフレームレート出てる気がする
  ret = encoder_->setHWPresetType(V4L2_ENC_HW_PRESET_MEDIUM);
  INIT_ERROR(ret < 0, "Failed to setHWPresetType");

  ret = encoder_->setNumBFrames(0);
  INIT_ERROR(ret < 0, "Failed to setNumBFrames");

  //この設定を入れればフレームレートより画質が優先されるが動くとフレームレートが激しく落ちる
  //ret = encoder_->setConstantQp(30);
  //INIT_ERROR(ret < 0, "Failed to setConstantQp");

  ret = encoder_->setInsertSpsPpsAtIdrEnabled(true);
  INIT_ERROR(ret < 0, "Failed to setInsertSpsPpsAtIdrEnabled");

  ret = encoder_->setInsertVuiEnabled(true);
  INIT_ERROR(ret < 0, "Failed to setInsertSpsPpsAtIdrEnabled");

  if (use_mjpeg_) {
    ret =
        encoder_->output_plane.setupPlane(V4L2_MEMORY_DMABUF, 10, false, false);
    INIT_ERROR(ret < 0, "Failed to setupPlane at encoder output_plane");
  } else {
    ret = encoder_->output_plane.setupPlane(V4L2_MEMORY_MMAP, 10, true, false);
    INIT_ERROR(ret < 0, "Failed to setupPlane at encoder output_plane");
  }

  ret = encoder_->capture_plane.setupPlane(V4L2_MEMORY_MMAP, 10, true, false);
  INIT_ERROR(ret < 0, "Failed to setupPlane at capture_plane");

  ret = encoder_->subscribeEvent(V4L2_EVENT_EOS, 0, 0);
  INIT_ERROR(ret < 0, "Failed to subscribeEvent V4L2_EVENT_EOS");

  ret = encoder_->output_plane.setStreamStatus(true);
  INIT_ERROR(ret < 0, "Failed to setStreamStatus at encoder output_plane");

  ret = encoder_->capture_plane.setStreamStatus(true);
  INIT_ERROR(ret < 0, "Failed to setStreamStatus at encoder capture_plane");

  if (use_mjpeg_) {
    converter_->capture_plane.startDQThread(this);

    for (uint32_t i = 0; i < converter_->capture_plane.getNumBuffers(); i++) {
      struct v4l2_buffer v4l2_buf;
      struct v4l2_plane planes[MAX_PLANES];
      memset(&v4l2_buf, 0, sizeof(v4l2_buf));
      memset(planes, 0, MAX_PLANES * sizeof(struct v4l2_plane));
      v4l2_buf.index = i;
      v4l2_buf.m.planes = planes;
      ret = converter_->capture_plane.qBuffer(v4l2_buf, NULL);
      INIT_ERROR(ret < 0, "Failed to qBuffer at converter capture_plane");
    }

    for (uint32_t i = 0; i < encoder_->output_plane.getNumBuffers(); i++) {
      enc0_buffer_queue_->push(encoder_->output_plane.getNthBuffer(i));
    }
    encoder_->output_plane.setDQThreadCallback(EncodeOutputCallbackFunction);
  }
  encoder_->capture_plane.setDQThreadCallback(EncodeFinishedCallbackFunction);
  if (use_mjpeg_) {
    encoder_->output_plane.startDQThread(this);
  }
  encoder_->capture_plane.startDQThread(this);

  for (uint32_t i = 0; i < encoder_->capture_plane.getNumBuffers(); i++) {
    struct v4l2_buffer v4l2_buf;
    struct v4l2_plane planes[MAX_PLANES];
    memset(&v4l2_buf, 0, sizeof(v4l2_buf));
    memset(planes, 0, MAX_PLANES * sizeof(struct v4l2_plane));
    v4l2_buf.index = i;
    v4l2_buf.m.planes = planes;
    ret = encoder_->capture_plane.qBuffer(v4l2_buf, NULL);
    INIT_ERROR(ret < 0, "Failed to qBuffer at encoder capture_plane");
  }

  configured_framerate_ = framerate_;
  configured_width_ = width_;
  configured_height_ = height_;

  return WEBRTC_VIDEO_CODEC_OK;
}

void JetsonH264Encoder::JetsonRelease() {
  if (!encoder_)
    return;
  if (converter_) {
    SendEOS(converter_);
  } else {
    SendEOS(encoder_);
  }
  encoder_->capture_plane.waitForDQThread(2000);
  encoder_->capture_plane.deinitPlane();
  encoder_->output_plane.deinitPlane();
  if (converter_) {
    delete enc0_buffer_queue_;
  }
  delete encoder_;
  encoder_ = nullptr;
  if (converter_) {
    converter_->capture_plane.waitForDQThread(2000);
    delete converter_;
    converter_ = nullptr;
  }
}

void JetsonH264Encoder::SendEOS(NvV4l2Element* element) {
  if (element->output_plane.getStreamStatus()) {
    struct v4l2_buffer v4l2_buf;
    struct v4l2_plane planes[MAX_PLANES];
    NvBuffer* buffer;

    memset(&v4l2_buf, 0, sizeof(v4l2_buf));
    memset(planes, 0, MAX_PLANES * sizeof(struct v4l2_plane));
    v4l2_buf.m.planes = planes;

    if (element->output_plane.getNumQueuedBuffers() ==
        element->output_plane.getNumBuffers()) {
      if (element->output_plane.dqBuffer(v4l2_buf, &buffer, NULL, 10) < 0) {
        RTC_LOG(LS_ERROR) << "Failed to dqBuffer at encoder output_plane";
      }
    }
    planes[0].bytesused = 0;
    if (element->output_plane.qBuffer(v4l2_buf, NULL) < 0) {
      RTC_LOG(LS_ERROR) << "Failed to qBuffer at encoder output_plane";
    }
  }
}

bool JetsonH264Encoder::ConvertFinishedCallbackFunction(
    struct v4l2_buffer* v4l2_buf,
    NvBuffer* buffer,
    NvBuffer* shared_buffer,
    void* data) {
  return ((JetsonH264Encoder*)data)
      ->ConvertFinishedCallback(v4l2_buf, buffer, shared_buffer);
}

bool JetsonH264Encoder::ConvertFinishedCallback(struct v4l2_buffer* v4l2_buf,
                                                NvBuffer* buffer,
                                                NvBuffer* shared_buffer) {
  NvBuffer* enc0_buffer;
  struct v4l2_buffer enc0_qbuf;
  struct v4l2_plane planes[MAX_PLANES];

  if (!v4l2_buf) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " v4l2_buf is null";
    return false;
  }
  {
    std::unique_lock<std::mutex> lock(enc0_buffer_mtx_);
    while (enc0_buffer_queue_->empty()) {
      enc0_buffer_cond_.wait(lock, [this] { return enc0_buffer_ready_; });
      enc0_buffer_ready_ = false;
    }
    enc0_buffer = enc0_buffer_queue_->front();
    enc0_buffer_queue_->pop();
  }

  memset(&enc0_qbuf, 0, sizeof(enc0_qbuf));
  memset(&planes, 0, sizeof(planes));

  enc0_qbuf.index = enc0_buffer->index;
  enc0_qbuf.m.planes = planes;

  for (int i = 0; i < MAX_PLANES; i++) {
    enc0_qbuf.m.planes[i].bytesused = v4l2_buf->m.planes[i].bytesused;
  }

  enc0_qbuf.flags |= V4L2_BUF_FLAG_TIMESTAMP_COPY;
  enc0_qbuf.timestamp.tv_sec = v4l2_buf->timestamp.tv_sec;
  enc0_qbuf.timestamp.tv_usec = v4l2_buf->timestamp.tv_usec;

  if (encoder_->output_plane.qBuffer(enc0_qbuf, buffer) < 0) {
    RTC_LOG(LS_ERROR) << __FUNCTION__
                      << " Failed to qBuffer at encoder output_plane";
    return false;
  }

  if (v4l2_buf->m.planes[0].bytesused == 0) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " buffer size is zero";
    return false;
  }

  return true;
}

bool JetsonH264Encoder::EncodeOutputCallbackFunction(
    struct v4l2_buffer* v4l2_buf,
    NvBuffer* buffer,
    NvBuffer* shared_buffer,
    void* data) {
  return ((JetsonH264Encoder*)data)
      ->EncodeOutputCallback(v4l2_buf, buffer, shared_buffer);
}

bool JetsonH264Encoder::EncodeOutputCallback(struct v4l2_buffer* v4l2_buf,
                                             NvBuffer* buffer,
                                             NvBuffer* shared_buffer) {
  struct v4l2_buffer conv_qbuf;
  struct v4l2_plane planes[MAX_PLANES];

  if (!v4l2_buf) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " v4l2_buf is null";
    return false;
  }

  memset(&conv_qbuf, 0, sizeof(conv_qbuf));
  memset(&planes, 0, sizeof(planes));

  conv_qbuf.index = shared_buffer->index;
  conv_qbuf.m.planes = planes;

  {
    std::unique_lock<std::mutex> lock(enc0_buffer_mtx_);
    if (converter_->capture_plane.qBuffer(conv_qbuf, nullptr) < 0) {
      RTC_LOG(LS_ERROR) << __FUNCTION__
                        << "Failed to qBuffer at converter capture_plane";
      return false;
    }
    enc0_buffer_queue_->push(buffer);
    enc0_buffer_ready_ = true;
    enc0_buffer_cond_.notify_all();
  }

  if (conv_qbuf.m.planes[0].bytesused == 0) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " buffer size is zero";
    return false;
  }

  return true;
}

bool JetsonH264Encoder::EncodeFinishedCallbackFunction(
    struct v4l2_buffer* v4l2_buf,
    NvBuffer* buffer,
    NvBuffer* shared_buffer,
    void* data) {
  return ((JetsonH264Encoder*)data)
      ->EncodeFinishedCallback(v4l2_buf, buffer, shared_buffer);
}

bool JetsonH264Encoder::EncodeFinishedCallback(struct v4l2_buffer* v4l2_buf,
                                               NvBuffer* buffer,
                                               NvBuffer* shared_buffer) {
  if (!v4l2_buf) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " v4l2_buf is null";
    return false;
  }
  if (buffer->planes[0].bytesused == 0) {
    RTC_LOG(LS_INFO) << __FUNCTION__ << " buffer size is zero";
    return false;
  }

  uint64_t pts = v4l2_buf->timestamp.tv_sec * rtc::kNumMicrosecsPerSec +
                 v4l2_buf->timestamp.tv_usec;
  RTC_LOG(LS_INFO) << __FUNCTION__ << " pts:" << pts
                   << " bytesused:" << buffer->planes[0].bytesused;

  std::unique_ptr<FrameParams> params;
  {
    rtc::CritScope lock(&frame_params_lock_);
    do {
      if (frame_params_.empty()) {
        RTC_LOG(LS_WARNING)
            << __FUNCTION__
            << "Frame parameter is not found. SkipFrame pts:" << pts;
        return true;
      }
      params = std::move(frame_params_.front());
      frame_params_.pop();
    } while (params->timestamp < pts);
    if (params->timestamp != pts) {
      RTC_LOG(LS_WARNING) << __FUNCTION__
                          << "Frame parameter is not found. SkipFrame pts:"
                          << pts;
      return true;
    }
  }

  encoded_image_._encodedWidth = params->width;
  encoded_image_._encodedHeight = params->height;
  encoded_image_.capture_time_ms_ = params->render_time_ms;
  encoded_image_.ntp_time_ms_ = params->ntp_time_ms;
  encoded_image_.SetTimestamp(pts / rtc::kNumMicrosecsPerMillisec);
  encoded_image_.rotation_ = params->rotation;
  encoded_image_.SetColorSpace(params->color_space);

  SendFrame(buffer->planes[0].data, buffer->planes[0].bytesused);

  if (encoder_->capture_plane.qBuffer(*v4l2_buf, NULL) < 0) {
    RTC_LOG(LS_ERROR) << __FUNCTION__ << "Failed to qBuffer at capture_plane";
    return false;
  }

  return true;
}

int32_t JetsonH264Encoder::RegisterEncodeCompleteCallback(
    webrtc::EncodedImageCallback* callback) {
  callback_ = callback;
  return WEBRTC_VIDEO_CODEC_OK;
}

void JetsonH264Encoder::SetRates(const RateControlParameters& parameters) {
  if (encoder_ == nullptr)
    return;
  if (parameters.bitrate.get_sum_bps() <= 0 || parameters.framerate_fps <= 0)
    return;

  RTC_LOG(LS_INFO) << __FUNCTION__ << " framerate:" << parameters.framerate_fps
                   << " bitrate:" << parameters.bitrate.get_sum_bps();
  framerate_ = parameters.framerate_fps;
  target_bitrate_bps_ = parameters.bitrate.get_sum_bps();
  bitrate_adjuster_.SetTargetBitrateBps(target_bitrate_bps_);
  return;
}

void JetsonH264Encoder::SetFramerate(uint32_t framerate) {
  if (width_ <= 1920 && height_ <= 1080 && framerate > 60) {
    framerate = 60;
  } else if (framerate > 30) {
    framerate = 30;
  }
  if (configured_framerate_ == framerate) {
    return;
  }
  RTC_LOG(LS_INFO) << __FUNCTION__ << " " << framerate << "fps";
  if (encoder_->setFrameRate(framerate, 1) < 0) {
    RTC_LOG(LS_ERROR) << "Failed to set bitrate";
    return;
  }
  configured_framerate_ = framerate;
}

void JetsonH264Encoder::SetBitrateBps(uint32_t bitrate_bps) {
  if (bitrate_bps < 300000 || configured_bitrate_bps_ == bitrate_bps) {
    return;
  }
  RTC_LOG(LS_INFO) << __FUNCTION__ << " " << bitrate_bps << "bit/sec";
  if (encoder_->setBitrate(bitrate_bps) < 0) {
    RTC_LOG(LS_ERROR) << "Failed to setBitrate";
    return;
  }
  configured_bitrate_bps_ = bitrate_bps;
}

webrtc::VideoEncoder::EncoderInfo JetsonH264Encoder::GetEncoderInfo() const {
  EncoderInfo info;
  info.supports_native_handle = true;
  info.implementation_name = "Jetson H264";
  info.scaling_settings =
      VideoEncoder::ScalingSettings(kLowH264QpThreshold, kHighH264QpThreshold);
  info.is_hardware_accelerated = true;
  info.has_internal_source = false;
  return info;
}

int32_t JetsonH264Encoder::Encode(
    const webrtc::VideoFrame& input_frame,
    const std::vector<webrtc::VideoFrameType>* frame_types) {
  if (!callback_) {
    RTC_LOG(LS_WARNING)
        << "InitEncode() has been called, but a callback function "
        << "has not been set with RegisterEncodeCompleteCallback()";
    return WEBRTC_VIDEO_CODEC_UNINITIALIZED;
  }

  int fd = 0;
  rtc::scoped_refptr<webrtc::VideoFrameBuffer> frame_buffer =
      input_frame.video_frame_buffer();
  if (frame_buffer->type() == webrtc::VideoFrameBuffer::Type::kNative) {
    use_mjpeg_ = true;
    NativeBuffer* native_buffer =
        dynamic_cast<NativeBuffer*>(frame_buffer.get());
    int ret = decoder_->decodeToFd(fd, (unsigned char*)native_buffer->Data(),
                                   native_buffer->length(), decode_pixfmt_,
                                   raw_width_, raw_height_);
    if (ret < 0) {
      RTC_LOG(LS_ERROR) << "Failed to decodeToFd";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
  } else {
    use_mjpeg_ = false;
  }

  if (frame_buffer->width() != configured_width_ ||
      frame_buffer->height() != configured_height_) {
    RTC_LOG(LS_INFO) << "Encoder reinitialized from " << configured_width_
                     << "x" << configured_height_ << " to "
                     << frame_buffer->width() << "x" << frame_buffer->height()
                     << " framerate:" << framerate_;
    JetsonRelease();
    if (JetsonConfigure() != WEBRTC_VIDEO_CODEC_OK) {
      RTC_LOG(LS_ERROR) << "Failed to JetsonConfigure";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
  }

  bool force_key_frame = false;
  if (frame_types != nullptr) {
    RTC_DCHECK_EQ(frame_types->size(), static_cast<size_t>(1));
    if ((*frame_types)[0] == webrtc::VideoFrameType::kEmptyFrame) {
      return WEBRTC_VIDEO_CODEC_OK;
    }
    if ((*frame_types)[0] == webrtc::VideoFrameType::kVideoFrameKey) {
      if (encoder_->forceIDR() < 0) {
        RTC_LOG(LS_ERROR) << "Failed to forceIDR";
      }
    }
  }

  SetFramerate(framerate_);
  SetBitrateBps(bitrate_adjuster_.GetAdjustedBitrateBps());
  {
    rtc::CritScope lock(&frame_params_lock_);
    frame_params_.push(absl::make_unique<FrameParams>(
        frame_buffer->width(), frame_buffer->height(),
        input_frame.render_time_ms(), input_frame.ntp_time_ms(),
        input_frame.timestamp_us(), input_frame.rotation(),
        input_frame.color_space()));
  }

  struct v4l2_buffer v4l2_buf;
  struct v4l2_plane planes[MAX_PLANES];

  memset(&v4l2_buf, 0, sizeof(v4l2_buf));
  memset(planes, 0, sizeof(planes));
  v4l2_buf.m.planes = planes;

  if (use_mjpeg_) {
    NvBuffer* buffer;
    if (converter_->output_plane.getNumQueuedBuffers() ==
        converter_->output_plane.getNumBuffers()) {
      if (converter_->output_plane.dqBuffer(v4l2_buf, &buffer, NULL, 10) < 0) {
        RTC_LOG(LS_ERROR) << "Failed to dqBuffer at converter output_plane";
        return WEBRTC_VIDEO_CODEC_ERROR;
      }
    }

    v4l2_buf.index = 0;
    planes[0].m.fd = fd;
    planes[0].bytesused = 1234;

    v4l2_buf.flags |= V4L2_BUF_FLAG_TIMESTAMP_COPY;
    v4l2_buf.timestamp.tv_sec =
        input_frame.timestamp_us() / rtc::kNumMicrosecsPerSec;
    v4l2_buf.timestamp.tv_usec =
        input_frame.timestamp_us() % rtc::kNumMicrosecsPerSec;

    if (converter_->output_plane.qBuffer(v4l2_buf, nullptr) < 0) {
      RTC_LOG(LS_ERROR) << "Failed to qBuffer at converter output_plane";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
  } else {
    NvBuffer* buffer;

    RTC_LOG(LS_INFO) << __FUNCTION__ << " output_plane.getNumBuffers: "
                     << encoder_->output_plane.getNumBuffers()
                     << " output_plane.getNumQueuedBuffers: "
                     << encoder_->output_plane.getNumQueuedBuffers();

    if (encoder_->output_plane.getNumQueuedBuffers() ==
        encoder_->output_plane.getNumBuffers()) {
      if (encoder_->output_plane.dqBuffer(v4l2_buf, &buffer, NULL, 10) < 0) {
        RTC_LOG(LS_ERROR) << "Failed to dqBuffer at encoder output_plane";
        return WEBRTC_VIDEO_CODEC_ERROR;
      }
    } else {
      buffer = encoder_->output_plane.getNthBuffer(
          encoder_->output_plane.getNumQueuedBuffers());
      v4l2_buf.index = encoder_->output_plane.getNumQueuedBuffers();
    }

    rtc::scoped_refptr<const webrtc::I420BufferInterface> i420_buffer =
        frame_buffer->ToI420();
    for (uint32_t i = 0; i < buffer->n_planes; i++) {
      const uint8_t* source_data;
      int source_stride;
      if (i == 0) {
        source_data = i420_buffer->DataY();
        source_stride = i420_buffer->StrideY();
      } else if (i == 1) {
        source_data = i420_buffer->DataU();
        source_stride = i420_buffer->StrideU();
      } else if (i == 2) {
        source_data = i420_buffer->DataV();
        source_stride = i420_buffer->StrideV();
      } else {
        break;
      }
      NvBuffer::NvBufferPlane& plane = buffer->planes[i];
      std::streamsize bytes_to_read = plane.fmt.bytesperpixel * plane.fmt.width;
      uint8_t* input_data = plane.data;
      plane.bytesused = 0;
      for (uint32_t j = 0; j < plane.fmt.height; j++) {
        memcpy(input_data, source_data + (source_stride * j), bytes_to_read);
        input_data += plane.fmt.stride;
      }
      plane.bytesused = plane.fmt.stride * plane.fmt.height;
    }

    v4l2_buf.flags |= V4L2_BUF_FLAG_TIMESTAMP_COPY;
    v4l2_buf.timestamp.tv_sec =
        input_frame.timestamp_us() / rtc::kNumMicrosecsPerSec;
    v4l2_buf.timestamp.tv_usec =
        input_frame.timestamp_us() % rtc::kNumMicrosecsPerSec;

    for (int i = 0; i < MAX_PLANES; i++) {
      if (NvBufferMemSyncForDevice(buffer->planes[i].fd, i,
                                   (void**)&buffer->planes[i].data) < 0) {
        RTC_LOG(LS_ERROR) << "Failed to NvBufferMemSyncForDevice";
        return WEBRTC_VIDEO_CODEC_ERROR;
      }
    }

    if (encoder_->output_plane.qBuffer(v4l2_buf, nullptr) < 0) {
      RTC_LOG(LS_ERROR) << "Failed to qBuffer at encoder output_plane";
      return WEBRTC_VIDEO_CODEC_ERROR;
    }
  }

  return WEBRTC_VIDEO_CODEC_OK;
}

int32_t JetsonH264Encoder::SendFrame(unsigned char* buffer, size_t size) {
  encoded_image_.set_buffer(buffer, size);
  encoded_image_.set_size(size);
  encoded_image_._frameType = webrtc::VideoFrameType::kVideoFrameDelta;

  uint8_t zero_count = 0;
  size_t nal_start_idx = 0;
  std::vector<nal_entry> nals;
  for (size_t i = 0; i < size; i++) {
    uint8_t data = buffer[i];
    if ((i != 0) && (i == nal_start_idx)) {
      if ((data & 0x1F) == 0x05) {
        encoded_image_._frameType = webrtc::VideoFrameType::kVideoFrameKey;
      }
    }
    if (data == 0x01 && zero_count == 3) {
      if (nal_start_idx != 0) {
        nals.push_back({nal_start_idx, i - nal_start_idx + 1 - 4});
      }
      nal_start_idx = i + 1;
    }
    if (data == 0x00) {
      zero_count++;
    } else {
      zero_count = 0;
    }
  }
  if (nal_start_idx != 0) {
    nals.push_back({nal_start_idx, size - nal_start_idx});
  }

  webrtc::RTPFragmentationHeader frag_header;
  frag_header.VerifyAndAllocateFragmentationHeader(nals.size());
  for (size_t i = 0; i < nals.size(); i++) {
    frag_header.fragmentationOffset[i] = nals[i].offset;
    frag_header.fragmentationLength[i] = nals[i].size;
  }

  webrtc::CodecSpecificInfo codec_specific;
  codec_specific.codecType = webrtc::kVideoCodecH264;
  codec_specific.codecSpecific.H264.packetization_mode =
      webrtc::H264PacketizationMode::NonInterleaved;

  h264_bitstream_parser_.ParseBitstream(buffer, size);
  h264_bitstream_parser_.GetLastSliceQp(&encoded_image_.qp_);
  RTC_LOG(LS_INFO) << __FUNCTION__ << " last slice qp:" << encoded_image_.qp_;

  webrtc::EncodedImageCallback::Result result =
      callback_->OnEncodedImage(encoded_image_, &codec_specific, &frag_header);
  if (result.error != webrtc::EncodedImageCallback::Result::OK) {
    RTC_LOG(LS_ERROR) << __FUNCTION__
                      << " OnEncodedImage failed error:" << result.error;
    return WEBRTC_VIDEO_CODEC_ERROR;
  }
  bitrate_adjuster_.Update(size);
  return WEBRTC_VIDEO_CODEC_OK;
}
