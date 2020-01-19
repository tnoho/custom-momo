#ifndef CONNECTION_SETTINGS_H_
#define CONNECTION_SETTINGS_H_

#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "api/rtp_parameters.h"

struct ConnectionSettings {
  std::string camera_name = "";
#if USE_ROS
  bool image_compressed = false;
  std::string audio_topic_name = "";
  int audio_topic_rate = 16000;
  int audio_topic_ch = 1;
#endif

  bool no_video = false;
  bool no_audio = false;
  bool force_i420 = false;
  bool use_native = false;
  std::string video_device = "";
  std::string video_codec = "VP8";
  std::string audio_codec = "OPUS";
  int video_bitrate = 0;
  int audio_bitrate = 0;
  std::string resolution = "VGA";
  int framerate = 30;
  bool fixed_resolution = false;
  std::string priority = "BALANCE";
  int port = 8080;
  bool use_sdl = false;
  bool show_me = false;
  int window_width = 640;
  int window_height = 480;
  bool fullscreen = false;
  std::string serial_device = "";
  unsigned int serial_rate = 9600;

  std::string sora_signaling_host = "wss://example.com/signaling";
  std::string sora_channel_id;
  bool sora_auto_connect = false;
  nlohmann::json sora_metadata;
  // upstream or downstream
  std::string sora_role = "upstream";
  bool sora_multistream = false;
  int sora_spotlight = -1;

  std::string test_document_root;

  std::string ayame_signaling_host;
  std::string ayame_room_id;
  std::string ayame_client_id = "";
  std::string ayame_signaling_key = "";

  bool disable_echo_cancellation = false;
  bool disable_auto_gain_control = false;
  bool disable_noise_suppression = false;
  bool disable_highpass_filter = false;
  bool disable_typing_detection = false;
  bool disable_residual_echo_detector = false;

  struct Size {
    int width;
    int height;
  };
  Size getSize() {
    if (resolution == "QVGA") {
      return {320, 240};
    } else if (resolution == "VGA") {
      return {640, 480};
    } else if (resolution == "HD") {
      return {1280, 720};
    } else if (resolution == "FHD") {
      return {1920, 1080};
    } else if (resolution == "4K") {
      return {3840, 2160};
    }

    // 128x96 みたいな感じのフォーマット
    auto pos = resolution.find('x');
    if (pos == std::string::npos) {
      return {16, 16};
    }
    auto width = std::atoi(resolution.substr(0, pos).c_str());
    auto height = std::atoi(resolution.substr(pos + 1).c_str());
    return {std::max(16, width), std::max(16, height)};
  }

  // FRAMERATE が優先のときは RESOLUTION をデグレさせていく
  webrtc::DegradationPreference getPriority() {
    if (priority == "FRAMERATE") {
      return webrtc::DegradationPreference::MAINTAIN_RESOLUTION;
    } else if (priority == "RESOLUTION") {
      return webrtc::DegradationPreference::MAINTAIN_FRAMERATE;
    }
    return webrtc::DegradationPreference::BALANCED;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const ConnectionSettings& cs) {
    os << "no_video: " << (cs.no_video ? "true" : "false") << "\n";
    os << "no_audio: " << (cs.no_audio ? "true" : "false") << "\n";
    os << "video_codec: " << cs.video_codec << "\n";
    os << "audio_codec: " << cs.audio_codec << "\n";
    os << "video_bitrate: " << cs.video_bitrate << "\n";
    os << "audio_bitrate: " << cs.audio_bitrate << "\n";
    os << "resolution: " << cs.resolution << "\n";
    os << "framerate: " << cs.framerate << "\n";
    os << "fixed_resolution: " << (cs.fixed_resolution ? "true" : "false")
       << "\n";
    os << "priority: " << cs.priority << "\n";
    os << "port: " << cs.port << "\n";
    os << "ayame_signaling_host: " << cs.ayame_signaling_host << "\n";
    os << "ayame_room_id: " << cs.ayame_room_id << "\n";
    os << "ayame_client_id: " << cs.ayame_client_id << "\n";
    os << "sora_signaling_host: " << cs.sora_signaling_host << "\n";
    os << "sora_channel_id: " << cs.sora_channel_id << "\n";
    os << "sora_auto_connect: " << (cs.sora_auto_connect ? "true" : "false")
       << "\n";
    os << "sora_metadata: " << cs.sora_metadata << "\n";
    os << "test_document_root: " << cs.test_document_root << "\n";
    return os;
  }
};

#endif
