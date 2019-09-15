#include "robo_data_channel.h"

#include "rtc_base/logging.h"
#include "absl/strings/match.h"


RoboDataChannel::RoboDataChannel(
        RoboDataManager *data_manager,
        rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel) 
  : _data_manager(data_manager),
    _data_channel(data_channel)
{
  _data_channel->RegisterObserver(this);
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
    _data_manager->CameraEvent(data + 1);
  }
  else if (data[0] == 0x02 && data_len == 5 + 1)
  {
    _data_manager->ArmEvent(data + 1);
  }
}