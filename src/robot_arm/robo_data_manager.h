#ifndef ROBO_DATA_MANAGER_H_
#define ROBO_DATA_MANAGER_H_
#include <vector>

#include "rtc/data_manager.h"
#include "robo_data_channel.h"

class RoboDataChannel;

class RoboDataManager : public RTCDataManager
{
public:
  RoboDataManager();
  ~RoboDataManager();

  void OnDataChannel(
          rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
  void Remove(RoboDataChannel *data_channel);

  void CameraEvent(const uint8_t* data);
  void ArmEvent(const uint8_t* data);

private:
  std::vector<RoboDataChannel *> _data_channels;
  int _serial_fd;
};

#endif