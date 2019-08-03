#ifndef ROBO_DATA_MANAGER_H_
#define ROBO_DATA_MANAGER_H_
#include <vector>

#include "rtc/data_manager.h"
#include "robo_data_channel.h"

class RoboDataChannel;

class RoboDataManager : public RTCDataManager
{
public:
  ~RoboDataManager();

  void OnDataChannel(
          rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel);
  void Remove(RoboDataChannel *data_channel);
private:
  std::vector<RoboDataChannel *> _data_channels; 
};

#endif