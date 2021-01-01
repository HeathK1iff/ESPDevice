#ifndef ESPDEVICE_H
#define ESPDEVICE_H

#include "EEPROM.h"
#include <ESP8266WiFi.h>
#include "TimeLib.h"
#include <IPAddress.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>

#define LENGTH_MQTT_USER 25
#define LENGTH_MQTT_PASS 25
#define LENGTH_NTP_HOST 30
#define LENGTH_IP_MQTT 16
#define LENGTH_SSID 25
#define LENGTH_SSID_PASS 25
#define LENGTH_DEVICE_NAME 25
#define LENGTH_LOCATION_NAME 30
#define EEPROM_SIZE 512

#define MIN_MS 60000l
#define SEC_MS 1000l
#define EEPROM_NETWORK_SECTION 1
#define FLAG_EEPROM_HAS_SETTING 0
#define EEPROM_USER_SECTION 180
#define MQTT_PORT 1883
#define DEFAULT_NTP_HOST "2.pool.ntp.org"
#define TIMEOUT_WIFI_CHECK_STATE SEC_MS * 15
#define TIMEOUT_WIFI_CONNECT SEC_MS * 5
#define TIMEOUT_UPDATE_NTP_TIME HOUR_MS * 12
#define TIMEOUT_MQTT_CONNECT SEC_MS * 5
#define MQTT_DEVICE_NAME_LEGNTH LENGTH_DEVICE_NAME + LENGTH_LOCATION_NAME + 5
#define MQTT_TOPIC_CACHE MQTT_DEVICE_NAME_LEGNTH * 2

class IMqttClient
{
public:
  virtual void setServer(const char *host, int port) = 0;
  virtual bool isConnected() = 0;
  virtual void disconnect() = 0;
  virtual bool connect(char *devicename, char* user, char* password) = 0;
  virtual bool tryConnect(char* deviceName, const char *host, int port, const char* user, const char* password) = 0;
  virtual void maintenance() = 0;
  virtual void publish(const char* topic, const char* payload, boolean retained) = 0;
};

typedef void (*MqttTopicSubscribe)();

class ESPDevice
{
private:
  struct DeviceSettings
  {
    char ssid[LENGTH_SSID];
    char ssidPass[LENGTH_SSID_PASS];
    char ntp[LENGTH_NTP_HOST];
    char mqttPrefix[LENGTH_LOCATION_NAME];
    char mqttBrokerIp[LENGTH_IP_MQTT];
    char mqttUserName[LENGTH_MQTT_USER];
    char mqttPassword[LENGTH_MQTT_PASS];   
    uint8_t updateLastError;
    byte definedUserSettings;
  } conn;

  unsigned long tsNeedSave = 0;
  unsigned long tsReconnect = 0;
  unsigned long tsNTPTime = 0;
  unsigned long tsMQTTtime = 0;
  unsigned long tsUpdateSensor = 0;
  unsigned long tsUpdateTime = 0; 
  char deviceName[LENGTH_DEVICE_NAME];
  MqttTopicSubscribe subscribe;
  char mqttTopicCache[MQTT_TOPIC_CACHE];
  char *pMqttTopicCache;
  WiFiEventHandler stationDisconnectedHandler;
  WiFiEventHandler stationAPConnectedHandler;
  WiFiEventHandler stationAPDisconnectedHandler;
  
  void publishDeviceInfo();
  void initMqttTopicCache();
protected:
  IMqttClient *mqttClient;

public:
  ESPDevice(ESP8266WebServer &server, IMqttClient *mqttClient, const char *devicename);

  void init(MqttTopicSubscribe subscribe);

  //MQTT Settings 
  IMqttClient *getMqttClient();
  char *makeMqttTopic(char *topic);
  
  char *getMQTTBrokerIp();
  void setMQTTBrokerIp(const char *mqttBrokerIp);
  void setMQTTBrokerIp(char *mqttBrokerIp);
  
  char *getMQTTUser();
  void setMQTTUser(char* mqttUser);
  void setMQTTUser(const char* mqttUser);
  
  char *getMQTTPassw();
  void setMQTTPassw(char* mqttPass);
  void setMQTTPassw(const char* mqttPass);
  
  char * getMQTTPrefix();
  void setMQTTPrefix(char *mqttPrefix);
  void setMQTTPrefix(const char *mqttPrefix);

  //Wifi network settings
  char *getSSID();
  void setSSID(char *ssid);
  void setSSID(const char *ssid);
  
  char *getSSIDPass();
  void setSSIDPass(char *pass);
  void setSSIDPass(const char *pass);
  
  bool connectTo(const char *ssid, const char *psw);
  bool tryConnectTo(const char *ssid, const char *psw);

  //Common settings
  char *getDeviceName();
  char *getBuildTimeFirmware(char *buf);
  uint8 getUpdateLastError();
  void getUpdateLastError(char *buf);
  void setUpdateLastError(uint8 error);
  void save();

  //Time settings
  char *getNtpServer();
  time_t getTime();
  char *getTime(char *buf);
  char *getUpTime(char *buf);
  char *upTime(unsigned long timestamp, char *uptime);
  bool updateTimeByNtp(char *host);


  template <typename T>
  const T &save(const T &t)
  {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(EEPROM_USER_SECTION, t);
    EEPROM.commit();
    
    conn.definedUserSettings = 1;
    save();
  }

  template <typename T>
  T &load(T &t)
  {
    if (conn.definedUserSettings == 1){
      EEPROM.begin(EEPROM_SIZE);
      EEPROM.get(EEPROM_USER_SECTION, t);
    }
    return t;
  }

  void reset();
  void maintenance();
};

void Ip2Str(char *target, IPAddress ip);
char *upTime2str(char *buf, unsigned long ms);

#endif
