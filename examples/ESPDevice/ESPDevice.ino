#include <ESP8266WebServer.h>
#include "SoftwareSerial.h"
#include <ESPDevice.h>
#include <PubSubMqttAdapter.h>

ESP8266WebServer server(80);
WiFiClient mqqtWifiClient;
PubSubClient mqttClient(mqqtWifiClient);
ESPDevice device(server, new PubSubMqttAdapter(mqttClient), "DEVICE");

void callback(char *topic, byte *payload, unsigned int length){
  
};

void mqttSubscribe(){
  ;
}

void setup()
{
  device.init(mqttSubscribe);
  mqttClient.setCallback(callback);
  server.begin();
}

void loop()
{
  server.handleClient();
  device.maintenance();
}