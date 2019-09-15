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
  const uint8_t* data = buffer.data.data<uint8_t>();
  size_t data_len = buffer.data.size();

  RTC_LOG(LS_INFO) << __FUNCTION__ << " data[0]:" << data[0] << " data_len:" << data_len;
  if (data[0] == 0x01 && data_len == sizeof(float) * 2 + 1) {
    CameraEvent(data + 1);
  }
  else if (data[0] == 0x02 && data_len == 5 + 1)
  {
    ArmEvent(data + 1);
  }
}

void RoboDataChannel::CameraEvent(const uint8_t* data)
{
  float alpha;
  float gamma;
  memcpy(&alpha, data, sizeof(float));
  memcpy(&gamma, data + sizeof(float), sizeof(float));
  RTC_LOG(LS_INFO) << __FUNCTION__ << " alpha:" << alpha << " gamma:" << gamma;
  unsigned int alpha_width = (alpha * 950.0 / 90.0) + 1450.0;
  unsigned int gamma_width = (gamma * 950.0 / 90.0) + 1450.0;
  if (alpha_width < 500) alpha_width = 500;
  else if (alpha_width > 2400) alpha_width = 2400;
  if (gamma_width < 500) gamma_width = 500;
  else if (gamma_width > 2400) gamma_width = 2400;
  RTC_LOG(LS_INFO) << __FUNCTION__ << " alpha_width:" << alpha_width << " gamma_width:" << gamma_width;
  gpioServo(12, alpha_width);
  gpioServo(13, gamma_width);
}

void RoboDataChannel::ArmEvent(const uint8_t* data)
{
  if(_serial_fd < 0) {
    return;
  }
  unsigned char buf[] = "00000#";
  buf[0] = data[0] + 0x30;
  buf[1] = data[1] + 0x30;
  buf[2] = data[2] + 0x30;
  buf[3] = data[3] + 0x30;
  buf[4] = data[4] + 0x30;
  RTC_LOG(LS_INFO) << __FUNCTION__ << " buf:" << buf;
  write(_serial_fd, buf, 6);
}