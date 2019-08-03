#include "robo_data_channel.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pigpio.h>

#include "rtc_base/logging.h"
#include "absl/strings/match.h"


RoboDataChannel::RoboDataChannel(
        RoboDataManager *data_manager,
        rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) 
  : _data_manager(data_manager),
    _data_channel(data_channel),
    _serial_fd(-1)
{
  _data_channel->RegisterObserver(this);
  _serial_fd = open("/dev/ttyACM0",O_RDWR);
  if (_serial_fd < 0){
    RTC_LOG(LS_ERROR) << __FUNCTION__ << " open serial failed";
  }
  else
  {
    struct termios tio;
    tio.c_cflag = B9600 | CRTSCTS | CREAD | CLOCAL | CS8;
    tio.c_iflag = IGNPAR | ICRNL;
    tio.c_oflag = 0;
    tio.c_lflag = ICANON;

    tcflush(_serial_fd, TCIFLUSH);
    tcsetattr(_serial_fd, TCSANOW, &tio);
  }
  
}

RoboDataChannel::~RoboDataChannel()
{
  if (_serial_fd >= 0)
  {
    close(_serial_fd);
  }
}

void RoboDataChannel::OnStateChange()
{
  webrtc::DataChannelInterface::DataState state = _data_channel->state();
	if (state == webrtc::DataChannelInterface::kClosed) {
    _data_manager->Remove(this);
  }
}

void RoboDataChannel::OnMessage(const webrtc::DataBuffer& buffer)
{
  std::string recv_string(buffer.data.data<char>(), buffer.data.size());
  json recv_message;
  try
  {
      recv_message = json::parse(recv_string);
  }
  catch (json::parse_error &e)
  {
      return;
  }

  try
  {
    std::string event = recv_message["event"];
    if (absl::StartsWith(event, "camera")) {
      CameraEvent(event, recv_message);
    } else if (absl::StartsWith(event, "arm"))
    {
      ArmEvent(event, recv_message);
    }
  }
  catch (json::type_error &e)
  {
    return;
  }

}

bool RoboDataChannel::SendMessage(const std::string message) {
  webrtc::DataBuffer data_buffer(message);
  if (!_data_channel) {
		return false;
	}
	if (_data_channel->state() == webrtc::DataChannelInterface::kOpen) {
		return _data_channel->Send(data_buffer);
	} else {
		return false;
	}
}

void RoboDataChannel::CameraEvent(std::string& event, json& recv_message)
{
  float alpha = recv_message["alpha"];
  float gamma = recv_message["gamma"];
  unsigned int alpha_width = (alpha * 500.0 / 90) + 1500;
  unsigned int gamma_width = (gamma * 500.0 / 90) + 1500;
  if (alpha_width < 1000) alpha_width = 1000;
  else if (alpha_width > 2000) alpha_width = 2000;
  if (gamma_width < 1000) gamma_width = 1000;
  else if (gamma_width > 2000) gamma_width = 2000;
  RTC_LOG(LS_INFO) << __FUNCTION__ << " alpha_width:" << alpha_width << " gamma_width:" << gamma_width;
  gpioServo(12, alpha_width);
  gpioServo(13, gamma_width);
}

void RoboDataChannel::ArmEvent(std::string& event, json& recv_message)
{
  if(_serial_fd < 0) {
    return;
  }
  unsigned char buf[] = "00000#";
  buf[0] = (unsigned char)recv_message["moter1"] + 0x30;
  buf[1] = (unsigned char)recv_message["moter2"] + 0x30;
  buf[2] = (unsigned char)recv_message["moter3"] + 0x30;
  buf[3] = (unsigned char)recv_message["moter4"] + 0x30;
  buf[4] = (unsigned char)recv_message["moter5"] + 0x30;
  RTC_LOG(LS_INFO) << __FUNCTION__ << " buf:" << buf;
  write(_serial_fd, buf, 6);
}