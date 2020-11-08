#ifndef PUB_SUB_MQQT_ADAPTER_H
#define PUB_SUB_MQQT_ADAPTER_H

#include <PubSubClient.h>
#include "ESPDevice.h"


class PubSubMqttAdapter: public IMqttClient {
  private:
     PubSubClient* client;
  public:
     PubSubMqttAdapter(PubSubClient& client);
     void setServer(const char* host, int port);
     bool isConnected();
     void disconnect();
     bool connect(char* devicename, char* user, char* password);
     bool tryConnect(char* deviceName, const char* host, int port, const char* user, const char* password);
     void maintenance();
     void publish(const char* topic, const char* payload, boolean retained);
};


#endif