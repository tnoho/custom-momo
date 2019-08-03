#include "robo_data_manager.h"

RoboDataManager::~RoboDataManager() {
  for (RoboDataChannel *data_channel : _data_channels)
  {
    delete data_channel;
  }
}

void RoboDataManager::OnDataChannel(
        rtc::scoped_refptr<webrtc::DataChannelInterface> data_channel)
{
  _data_channels.push_back(new RoboDataChannel(this, data_channel));
}


void RoboDataManager::Remove(RoboDataChannel *data_channel)
{
  _data_channels.erase(
      std::remove(_data_channels.begin(), _data_channels.end(), data_channel),
      _data_channels.end());
  delete data_channel;
}