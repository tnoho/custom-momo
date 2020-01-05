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

#ifndef Jetson_H264_ENCODER_H_
#define Jetson_H264_ENCODER_H_

#include <linux/videodev2.h>

#include <chrono>
#include <memory>
#include <queue>

#include "NvJpegDecoder.h"
#include "NvVideoConverter.h"
#include "NvVideoEncoder.h"
#include "api/video_codecs/video_encoder.h"
#include "common_video/h264/h264_bitstream_parser.h"
#include "common_video/include/bitrate_adjuster.h"
#include "modules/video_coding/codecs/h264/include/h264.h"
#include "rtc_base/critical_section.h"

class ProcessThread;

class JetsonH264Encoder : public webrtc::VideoEncoder {
 public:
  explicit JetsonH264Encoder(const cricket::VideoCodec& codec);
  ~JetsonH264Encoder() override;

  int32_t InitEncode(const webrtc::VideoCodec* codec_settings,
                     int32_t number_of_cores,
                     size_t max_payload_size) override;
  int32_t RegisterEncodeCompleteCallback(
      webrtc::EncodedImageCallback* callback) override;
  int32_t Release() override;
  int32_t Encode(
      const webrtc::VideoFrame& frame,
      const std::vector<webrtc::VideoFrameType>* frame_types) override;
  void SetRates(const RateControlParameters& parameters) override;
  webrtc::VideoEncoder::EncoderInfo GetEncoderInfo() const override;

 private:
  struct FrameParams {
    FrameParams(int32_t w,
                int32_t h,
                int64_t rtms,
                int64_t ntpms,
                int64_t ts,
                webrtc::VideoRotation r,
                absl::optional<webrtc::ColorSpace> c)
        : width(w),
          height(h),
          render_time_ms(rtms),
          ntp_time_ms(ntpms),
          timestamp(ts),
          rotation(r),
          color_space(c) {}

    int32_t width;
    int32_t height;
    int64_t render_time_ms;
    int64_t ntp_time_ms;
    int64_t timestamp;
    webrtc::VideoRotation rotation;
    absl::optional<webrtc::ColorSpace> color_space;
  };

  int32_t JetsonConfigure();
  void JetsonRelease();
  void SendEOS(NvV4l2Element* element);
  static bool ConvertFinishedCallbackFunction(struct v4l2_buffer* v4l2_buf,
                                              NvBuffer* buffer,
                                              NvBuffer* shared_buffer,
                                              void* data);
  bool ConvertFinishedCallback(struct v4l2_buffer* v4l2_buf,
                               NvBuffer* buffer,
                               NvBuffer* shared_buffer);
  static bool EncodeOutputCallbackFunction(struct v4l2_buffer* v4l2_buf,
                                           NvBuffer* buffer,
                                           NvBuffer* shared_buffer,
                                           void* data);
  bool EncodeOutputCallback(struct v4l2_buffer* v4l2_buf,
                            NvBuffer* buffer,
                            NvBuffer* shared_buffer);
  static bool EncodeFinishedCallbackFunction(struct v4l2_buffer* v4l2_buf,
                                             NvBuffer* buffer,
                                             NvBuffer* shared_buffer,
                                             void* data);
  bool EncodeFinishedCallback(struct v4l2_buffer* v4l2_buf,
                              NvBuffer* buffer,
                              NvBuffer* shared_buffer);
  void SetFramerate(uint32_t framerate);
  void SetBitrateBps(uint32_t bitrate_bps);
  int32_t SendFrame(unsigned char* buffer, size_t size);

  webrtc::EncodedImageCallback* callback_;
  NvJPEGDecoder* decoder_;
  NvVideoConverter* converter_;
  NvVideoEncoder* encoder_;
  webrtc::BitrateAdjuster bitrate_adjuster_;
  uint32_t framerate_;
  int32_t configured_framerate_;
  uint32_t target_bitrate_bps_;
  uint32_t configured_bitrate_bps_;
  int key_frame_interval_;
  uint32_t decode_pixfmt_;
  uint32_t raw_width_;
  uint32_t raw_height_;
  int32_t width_;
  int32_t height_;
  int32_t configured_width_;
  int32_t configured_height_;
  bool use_mjpeg_;

  webrtc::H264BitstreamParser h264_bitstream_parser_;

  rtc::CriticalSection frame_params_lock_;
  std::queue<std::unique_ptr<FrameParams>> frame_params_;
  std::mutex enc0_buffer_mtx_;
  std::condition_variable enc0_buffer_cond_;
  bool enc0_buffer_ready_ = false;
  std::queue<NvBuffer*>* enc0_buffer_queue_;
  webrtc::EncodedImage encoded_image_;
};

#endif  // Jetson_H264_ENCODER_H_
