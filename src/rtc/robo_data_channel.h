#ifndef ROBO_DATA_CHANNEL_H_
#define ROBO_DATA_CHANNEL_H_

#include "api/data_channel_interface.h"

#include "robo_data_manager.h"

class RoboDataManager;

class RoboDataChannel : public webrtc::DataChannelObserver
{
public:
  RoboDataChannel(
        RoboDataManager *data_manager,
        rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
  ~RoboDataChannel();

  void OnStateChange() override;
  void OnMessage(const webrtc::DataBuffer& buffer) override;
  void OnBufferedAmountChange(uint64_t previous_amount) override {}

private:
  void CameraEvent(const uint8_t* data);
  void ArmEvent(const uint8_t* data);

  RoboDataManager *_data_manager;
  rtc::scoped_refptr<webrtc::DataChannelInterface> _data_channel;
  int _serial_fd;
};

#endif