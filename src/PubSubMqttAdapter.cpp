#include "PubSubMqttAdapter.h"
#include "Arduino.h"

PubSubMqttAdapter::PubSubMqttAdapter(PubSubClient& client){
    this->client = &client;    
}

void PubSubMqttAdapter::publish(const char* topic, const char* payload, boolean retained){
    this->client->publish(topic, (const uint8_t*) payload, strlen(payload), retained);
}

void PubSubMqttAdapter::setServer(const char* host, int port){
    IPAddress address;
    address.fromString(host);
    this->client->setServer(address, 1883);
}

bool PubSubMqttAdapter::isConnected(){
    return this->client->connected();
}

void PubSubMqttAdapter::disconnect(){
    this->client->disconnect();
}

bool PubSubMqttAdapter::connect(char* deviceName, char* user, char* password){
    this->client->connect(deviceName, user, password);
    return this->client->connected();
}

bool PubSubMqttAdapter::tryConnect(char* deviceName, const char* host, int port, const char* user, const char* password){
    IPAddress address;
    address.fromString(host);
    this->client->setServer(address, port);
    this->client->connect(deviceName, user, password);
    bool connected = this->client->connected();
    this->client->disconnect();
    return connected; 
}

void PubSubMqttAdapter::maintenance(){
   this->client->loop();  
}