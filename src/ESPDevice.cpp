#include "ESPDevice.h"
#include "HtmlGen.h"
#include "Resource.h"

#define NTP_PACKET_SIZE 48
#define DAY_MS 86400000l
#define HOUR_MS 3600000l
#define E_OK "OK"
#define E_WRITE "ERR_WRITE"
#define E_ERASE "ERR_ERASE"
#define E_READ "ERR_READ"
#define E_SPACE "ERR_SPACE"
#define E_SIZE "ERR_SIZE"
#define E_STREAM "ERR_STREAM"
#define E_MD5 "ERR_MD5"
#define E_FLASH_CONDIF "ERR_FLASH_CONFIG"
#define E_MAGIC_BYTE "ERR_MAGIC_BYTE"
#define E_BOOTSTRAP "ERR_BOOTSTRAP"
#define E_UNKNOWN "UNKNOWN"

ESP8266WebServer *_server;
ESPDevice *_device;
int connectedClients = 0;

char *upTime2str(char *buf, unsigned long ms)
{
  int days = ms / DAY_MS;
  unsigned long leftOfDay = ms - (days * DAY_MS);
  int hours = leftOfDay / HOUR_MS;
  unsigned long leftOfHour = leftOfDay - (hours * HOUR_MS);
  int mins = leftOfHour / MIN_MS;

  String d = String(days);
  String h = String(hours);
  String m = String(mins);
  sprintf(buf, "%s day %s hour %s min", d.c_str(), h.c_str(), m.c_str());
  return buf;
}

void makeTabs(HtmlPage *page, int pageIndex)
{
  char msgWelcome[100];
  sprintf(msgWelcome, "Welcome %s", _device->getDeviceName());
  page->getHeader()->setTitle(msgWelcome);

  HtmlTag *htmlTag = new HtmlTag("meta");
  htmlTag->getAttributes()->append("name", "viewport");
  htmlTag->getAttributes()->append("content", "width=device-width");
  page->getHeader()->append(htmlTag);

  HtmlStyleGroup *group = page->getHeader()->getStyle()->createGroup(".tabs td");
  group->append("width", "100px");
  group = page->getHeader()->getStyle()->createGroup(".htable");
  group->append("border", "1px solid #cdd0d4");
  group->append("width", "400px");
  group = page->getHeader()->getStyle()->createGroup(".hheader");
  group->append("background-color", "#cdd0d4");
  group->append("width", "400px");
  group = page->getHeader()->getStyle()->createGroup(".button");
  group->append("width", "100px");
  group->append("height", "30px");

  HtmlTable *tabs = new HtmlTable(4);

  tabs->append();
  tabs->getAttributes()->append("class", "tabs");
  tabs->getStyle()->append("width", "400px");
  tabs->getStyle()->append("background-color", "#DAF7A6");
  tabs->getCellStyle(0, pageIndex)->append("background-color", "#33ff6e");
  tabs->setCell(0, 0, new HtmlLink(PAGE_MAIN, "Overview"));
  tabs->setCell(0, 1, new HtmlLink(PAGE_NETWORK, "Network"));
  tabs->setCell(0, 2, new HtmlLink(PAGE_MQTT, "Mqtt"));
  tabs->setCell(0, 3, new HtmlLink(PAGE_SYSTEM, "System"));
  page->getHeader()->append(tabs);
}

char *makeAPIUrl(char *buf, char *url)
{
  if (WiFi.getMode() == WIFI_STA)
  {
    sprintf(buf, "http://%s%s", WiFi.localIP().toString().c_str(), url);
  }
  else
  {
    sprintf(buf, "http://%s%s", WiFi.softAPIP().toString().c_str(), url);
  }
  return buf;
}

void pageOverview()
{
  char uptime[20];
  HtmlPage *htmlPage = new HtmlPage();
  makeTabs(htmlPage, 0);

  HtmlTable *tableOverview = new HtmlTable(2);
  tableOverview->append(9);
  tableOverview->getStyle()->append("width", "400px");
  tableOverview->getCellAttribute(0, 0)->append("colspan", 2);
  tableOverview->getCellAttribute(0, 0)->append("align", "center");
  tableOverview->getStyle()->append("border", "1px solid #cdd0d4");
  tableOverview->getCellStyle(0, 0)->append("background-color", "#cdd0d4");
  tableOverview->setCell(0, 0, "System");

  char dTime[25];
  tableOverview->setCell(1, 0, "Time:");
  tableOverview->setCell(1, 1, _device->getTime(dTime));

  tableOverview->setCell(2, 0, "Name:");
  tableOverview->setCell(2, 1, _device->getDeviceName());

  tableOverview->setCell(3, 0, "SSID:");
  if (WiFi.getMode() == WIFI_STA)
  {
    tableOverview->setCell(3, 1, WiFi.SSID().c_str());
  }
  else
  {
    tableOverview->setCell(3, 1, _device->getDeviceName());
  }

  tableOverview->setCell(4, 0, "Mode:");
  const char *modes[] = {"NULL", "STA", "AP", "STA+AP"};
  tableOverview->setCell(4, 1, modes[WiFi.getMode()]);
  tableOverview->setCell(5, 0, "Ip:");
  if (WiFi.getMode() == WIFI_STA)
  {
    tableOverview->setCell(5, 1, WiFi.localIP().toString().c_str());
  }
  else
  {
    tableOverview->setCell(5, 1, WiFi.softAPIP().toString().c_str());
  }
  tableOverview->setCell(6, 0, "Mac:");
  tableOverview->setCell(6, 1, WiFi.macAddress().c_str());
  tableOverview->setCell(7, 0, "Uptime:");
  tableOverview->setCell(7, 1, _device->upTime(millis(), uptime));
  tableOverview->setCell(8, 0, "MQTT Broker:");
  if (_device->getMqttClient()->isConnected())
  {
    tableOverview->setCell(8, 1, "Online");
  }
  else
  {
    tableOverview->setCell(8, 1, "Undefined");
  }

  htmlPage->append(tableOverview);
  char html_overiview[1900];
  html_overiview[0] = '\0';
  htmlPage->print(html_overiview);
  _server->send(200, "text/html", html_overiview);
  delete htmlPage;
}


void pageMqttBroker(){
   if (_server->args() > 0)
  {
    if (_server->hasArg(W_MQTT) && (_server->hasArg(W_MQTT_USER)) && (_server->hasArg(W_MQTT_PSW)))
    {
      String mqttHost = _server->arg(W_MQTT);
      String mqttUser= _server->arg(W_MQTT_USER);
      String mqttPassword = _server->arg(W_MQTT_PSW);
      IPAddress address;
      if (address.fromString(mqttHost))
      {

        if (_device->getMqttClient()->tryConnect(_device->getDeviceName(), mqttHost.c_str(), MQTT_PORT, mqttUser.c_str(), mqttPassword.c_str()))
        {
          _device->setMQTTBrokerIp(mqttHost.c_str()); 
          _device->setMQTTUser(mqttUser.c_str());
          _device->setMQTTPassw(mqttPassword.c_str());
          _device->save();
          _device->getMqttClient()->disconnect();
        }
      }
    }

    if (_server->hasArg(W_MQTT_PREFIX))
    {
      String mqttPrefix = _server->arg(W_MQTT_PREFIX);
      _device->setMQTTPrefix(mqttPrefix.c_str());
      _device->save();
    }
  }

  HtmlPage *page = new HtmlPage();
  makeTabs(page, 2);

  HtmlForm *formMQTT = new HtmlForm("form_mqttt", PAGE_MQTT, Get);
  HtmlTable *tableMQTT = new HtmlTable(2);
  tableMQTT->append(5);
  tableMQTT->setCell(0, 1, "MQTT Broker");
  tableMQTT->getCellStyle(0, 1)->append("background-color", "#cdd0d4");
  tableMQTT->getCellAttribute(0, 1)->append("colspan", 2);
  tableMQTT->getCellAttribute(0, 1)->append("align", "center");
 
  tableMQTT->setCell(1, 0, "Host:");
  tableMQTT->getCellAttribute(1, 0)->append("width", "200px");
  tableMQTT->getStyle()->append("border", "1px solid #cdd0d4");
  tableMQTT->getStyle()->append("width", "400px");
  HtmlText *mqttHost = new HtmlText();
 
  mqttHost->setName(W_MQTT);
  mqttHost->setSize(15);
  mqttHost->setValue(_device->getMQTTBrokerIp());
  tableMQTT->setCell(1, 1, mqttHost);

  tableMQTT->setCell(2, 0, "User:");
  tableMQTT->getCellAttribute(2, 0)->append("width", "200px");
  tableMQTT->getStyle()->append("border", "1px solid #cdd0d4");
  tableMQTT->getStyle()->append("width", "400px");
  HtmlText *mqttUser = new HtmlText();
  mqttUser->setName(W_MQTT_USER);
  mqttUser->setSize(15);
  mqttUser->setValue(_device->getMQTTUser());
  tableMQTT->setCell(2, 1, mqttUser);

  tableMQTT->setCell(3, 0, "Password:");
  tableMQTT->getCellAttribute(3, 0)->append("width", "200px");
  tableMQTT->getStyle()->append("border", "1px solid #cdd0d4");
  tableMQTT->getStyle()->append("width", "400px");
  HtmlPassword *mqttPsw = new HtmlPassword();
  mqttPsw->setName(W_MQTT_PSW);
  mqttPsw->setSize(15);
  mqttPsw->setValue("0000");
  tableMQTT->setCell(3, 1, mqttPsw);

  HtmlButton *butSave = new HtmlButton("Save");

  butSave->getAttributes()->append("class", "button");
  tableMQTT->getCellAttribute(4, 0)->append("colspan", 2);
  tableMQTT->getCellAttribute(4, 0)->append("align", "right");
  tableMQTT->setCell(4, 0, butSave);
  formMQTT->append(tableMQTT);
  
  HtmlForm *formLocation = new HtmlForm("form_location", PAGE_MQTT, Get);
  HtmlTable *tableLocation = new HtmlTable(3);
  tableLocation->append(3);
  tableLocation->setCell(0, 1, "MQTT Prefix");
  tableLocation->getCellStyle(0, 1)->append("background-color", "#cdd0d4");
  tableLocation->getCellAttribute(0, 1)->append("colspan", 2);
  tableLocation->getCellAttribute(0, 1)->append("align", "center");
  tableLocation->setCell(1, 0, "Name:");
  tableLocation->getCellAttribute(1, 0)->append("width", "200px");
  tableLocation->getStyle()->append("border", "1px solid #cdd0d4");
  tableLocation->getStyle()->append("width", "400px");
  HtmlText *prefixMqtt = new HtmlText();
  prefixMqtt->setName(W_MQTT_PREFIX);
  prefixMqtt->setSize(15);
  prefixMqtt->setValue(_device->getMQTTPrefix());
  tableLocation->setCell(1, 1, prefixMqtt);
  HtmlButton *butSave2 = new HtmlButton("Save");
  butSave2->getAttributes()->append("class", "button");
  tableLocation->getCellAttribute(2, 0)->append("colspan", 2);
  tableLocation->getCellAttribute(2, 0)->append("align", "right");
  tableLocation->setCell(2, 0, butSave2);
  formLocation->append(tableLocation);
  page->append(formMQTT); 
  page->append(formLocation);
  char html_network[1900];
  html_network[0] = '\0';
  page->print(html_network);
  _server->send(200, "text/html", html_network);
  delete page;
}

void pageNetwork()
{
  if (_server->args() > 0)
  {
    if ((_server->hasArg(W_SSID)) &&
        (_server->hasArg(W_PASS)))
    {
      String ssid = _server->arg(W_SSID);
      String psw = _server->arg(W_PASS);
      _server->send(200, "text/html", "The wifi connection will be changed.");

      delay(100);
      if (_device->tryConnectTo(ssid.c_str(), psw.c_str()))
      {
        _device->setSSID(ssid.c_str());
        _device->setSSIDPass(psw.c_str());
        _device->save();
      }
      return;
    }
  }

  HtmlPage *page = new HtmlPage();
  makeTabs(page, 1);

  HtmlForm *formNetwork = new HtmlForm("form_network", "/network.html", Get);
  HtmlTable *tableWifi = new HtmlTable(2);
  tableWifi->append(4);
  tableWifi->setCell(0, 1, "Connection");
  tableWifi->getCellStyle(0, 1)->append("background-color", "#cdd0d4");
  tableWifi->getCellAttribute(0, 1)->append("colspan", 2);
  tableWifi->getCellAttribute(0, 1)->append("align", "center");
  tableWifi->setCell(1, 0, "Ssid:");
  tableWifi->getCellAttribute(1, 0)->append("width", "100px");
  tableWifi->getStyle()->append("border", "1px solid #cdd0d4");
  tableWifi->getStyle()->append("width", "400px");
  HtmlText *ssid = new HtmlText();
  ssid->setName(W_SSID);
  ssid->setSize(26);
  ssid->setValue(_device->getSSID());
  tableWifi->setCell(1, 1, ssid);
  tableWifi->setCell(2, 0, "Password:");
  HtmlPassword *ssidPass = new HtmlPassword();
  ssidPass->setName(W_PASS);
  ssidPass->setSize(26);
  ssidPass->setValue(_device->getSSIDPass());
  tableWifi->setCell(2, 1, ssidPass);
  tableWifi->getCellAttribute(3, 0)->append("colspan", 2);
  tableWifi->getCellAttribute(3, 0)->append("align", "right");

  HtmlButton *butConnect = new HtmlButton("Connect");
  butConnect->getAttributes()->append("class", "button");

  tableWifi->setCell(3, 0, butConnect);
  formNetwork->append(tableWifi);
  page->append(formNetwork);
  char html_network[1900];
  html_network[0] = '\0';
  page->print(html_network);
  _server->send(200, "text/html", html_network);
  delete page;
}

void pageFirmwareUpgrade()
{
  HTTPUpload &upload = _server->upload();
  if (upload.status == UPLOAD_FILE_START)
  {
    _device->setUpdateLastError(0);
    WiFiUDP::stopAll();
    uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!Update.begin(maxSketchSpace))
    {
      _device->setUpdateLastError(Update.getError());
    }
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
    {
      _device->setUpdateLastError(Update.getError());
    }
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    Update.end(true);
  }
}

void pageFirmwareUpload()
{
  _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
  ESP.restart();
}

void pageSystem()
{
  HtmlPage *htmlPage = new HtmlPage();
  makeTabs(htmlPage, 3);

  HtmlTable *tableInfo = new HtmlTable(2);
  tableInfo->append(11);
  tableInfo->getStyle()->append("width", "400px");
  tableInfo->getCellAttribute(0, 0)->append("colspan", 2);
  tableInfo->getCellAttribute(0, 0)->append("align", "center");
  tableInfo->getStyle()->append("border", "1px solid #cdd0d4");
  tableInfo->getCellStyle(0, 0)->append("background-color", "#cdd0d4");
  tableInfo->setCell(0, 0, "Info");
  tableInfo->setCell(1, 0, "Chip Id:");
  tableInfo->setCell(1, 1, ((int)ESP.getChipId()));
  tableInfo->setCell(2, 0, "Core:");
  tableInfo->setCell(2, 1, ESP.getCoreVersion().c_str());
  tableInfo->setCell(3, 0, "SDK ver:");
  tableInfo->setCell(3, 1, ESP.getSdkVersion());
  tableInfo->setCell(4, 0, "Firmware:");
  char buildTimeFirmware[50];
  tableInfo->setCell(4, 1, _device->getBuildTimeFirmware(buildTimeFirmware));
  tableInfo->setCell(5, 0, "Frequency:");
  tableInfo->setCell(5, 1, ((int)ESP.getCpuFreqMHz()));
  tableInfo->setCell(6, 0, "Flash Size:");
  tableInfo->setCell(6, 1, ((int)ESP.getFlashChipRealSize() / 1024));
  tableInfo->setCell(7, 0, "Sketch Size:");
  tableInfo->setCell(7, 1, ((int)ESP.getFreeSketchSpace()));
  tableInfo->setCell(8, 0, "Heap Size:");
  tableInfo->setCell(8, 1, (int)ESP.getFreeHeap());
  tableInfo->setCell(9, 0, "Update Error:");
  char buf[50];
  _device->getUpdateLastError(buf);

  tableInfo->setCell(9, 1, buf);
  tableInfo->setCell(10, 0, "Reset Reason:"); 
  tableInfo->setCell(10, 1, ESP.getResetReason().c_str()); 
  
  htmlPage->append(tableInfo);

  HtmlForm *uploadForm = new HtmlForm("upload", API_METHOD_FIRMWARE_UPLOAD, Post);
  uploadForm->getAttributes()->append("enctype", "multipart/form-data");
  HtmlUpload *uploadInput = new HtmlUpload();
  uploadInput->getAttributes()->append("name", "update");
  uploadForm->append(uploadInput);
  HtmlButton *uploadButton = new HtmlButton("update");
  uploadButton->getAttributes()->append("name", "update");
  uploadForm->append(uploadButton);

  HtmlTable *tableUpload = new HtmlTable(2);
  tableUpload->append(2);
  tableUpload->getStyle()->append("width", "400px");
  tableUpload->getCellAttribute(0, 0)->append("colspan", 2);
  tableUpload->getCellAttribute(0, 0)->append("align", "center");
  tableUpload->getStyle()->append("border", "1px solid #cdd0d4");
  tableUpload->getCellStyle(0, 0)->append("background-color", "#cdd0d4");
  tableUpload->setCell(0, 0, "Firmware");
  tableUpload->getCellAttribute(1, 0)->append("colspan", 2);
  tableUpload->setCell(1, 0, uploadForm);

  htmlPage->append(tableUpload);

  char html_overiview[1900];
  html_overiview[0] = '\0';
  htmlPage->print(html_overiview);
  _server->send(200, "text/html", html_overiview);
  delete htmlPage;
}

void pageNotFound()
{
  pageOverview();
}

ESPDevice::ESPDevice(ESP8266WebServer &server, IMqttClient *mqttClient, const char *devicename)
{
  _server = &server;
  _device = this;
  conn.ssid[0] = '\0';
  conn.ssidPass[0] = '\0';

  strcpy(conn.ntp, DEFAULT_NTP_HOST);

  sprintf(deviceName, "%s_%d", devicename, ESP.getChipId());

  this->mqttClient = mqttClient;
}

void onStationModeDisconnectedEvent(const WiFiEventStationModeDisconnected& evt) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
  };
}


void onAPModeStationConnected(const WiFiEventSoftAPModeStationConnected& evt) {
  connectedClients++;
}

void onAPModeStationDisconnected(const WiFiEventSoftAPModeStationDisconnected& evt) {
  connectedClients--;
}

void ESPDevice::init(MqttTopicSubscribe subscribe)
{
  this->subscribe = subscribe;
  _server->on(PAGE_MAIN, pageOverview);
  _server->on(PAGE_NETWORK, pageNetwork);
  _server->on(PAGE_MQTT, pageMqttBroker);
  _server->on(PAGE_SYSTEM, pageSystem);
  _server->on(API_METHOD_FIRMWARE_UPLOAD, HTTP_POST, pageFirmwareUpload, pageFirmwareUpgrade);
  _server->onNotFound(pageNotFound);
  
  WiFi.setAutoReconnect(false);
  WiFi.setAutoConnect(false);
  WiFi.persistent(false);
  stationDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationModeDisconnectedEvent);
  stationAPConnectedHandler = WiFi.onSoftAPModeStationConnected(&onAPModeStationConnected);
  stationAPDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(&onAPModeStationDisconnected);

  EEPROM.begin(EEPROM_SIZE);
  if (EEPROM.read(FLAG_EEPROM_HAS_SETTING) == 1)
  {
    EEPROM.get(EEPROM_NETWORK_SECTION, conn);
  }

  this->initMqttTopicCache();
}

time_t ESPDevice::getTime()
{
  return now();
}

char *ESPDevice::getTime(char *buf)
{
  return time2str(buf, now());
}

char *ESPDevice::getUpTime(char *buf)
{
  return upTime2str(buf, millis());
}

void ESPDevice::publishDeviceInfo(){
  char vrs[20];
  this->getBuildTimeFirmware(vrs);
  char data[150];
  memset(data, '\0', 150);
  strcat(data, "{\"device\":\"");
  strcat(data, this->getDeviceName());
  strcat(data, "\", \"ip\":\"");
  if (WiFi.getMode() == WIFI_STA){
    strcat(data, WiFi.localIP().toString().c_str());
  } else {
    strcat(data, WiFi.softAPIP().toString().c_str());
  }
  strcat(data, "\", \"firmware\":\"");
  strcat(data, vrs);
  strcat(data, "\"}");
  this->mqttClient->publish(this->makeMqttTopic(W_MQTT_INFO), data, true);
}

void ESPDevice::maintenance()
{
  if ((tsNeedSave != 0) && (millis() > tsNeedSave))
  {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.put(EEPROM_NETWORK_SECTION, this->conn);
    EEPROM.write(FLAG_EEPROM_HAS_SETTING, 1);
    EEPROM.commit();
    tsNeedSave = 0;
  }

  if ((tsReconnect == 0) || (millis() > tsReconnect))
  {
    if ((connectedClients == 0) && (!WiFi.isConnected()))
    {
      WiFi.hostname(this->getDeviceName());
      if (strlen(this->getSSID()) > 0)
        this->connectTo(this->getSSID(), this->getSSIDPass());

      if (!WiFi.isConnected()) {
        WiFi.mode(WIFI_AP);
        const char *deviceName = this->getDeviceName();
        if (strlen(this->getSSID()) > 0) {
          WiFi.softAP(deviceName, this->getSSIDPass());
        } else {
          WiFi.softAP(deviceName, NULL);
        }

        WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 0, 0));
      }     
    }
    tsReconnect = millis() + TIMEOUT_WIFI_CHECK_STATE;
  }

  if (WiFi.isConnected())
  {
    if ((tsNTPTime == 0) || (millis() > tsNTPTime)){
        updateTimeByNtp(this->getNtpServer());
        tsNTPTime = millis() + TIMEOUT_UPDATE_NTP_TIME;
    }
    
    
    if ((tsMQTTtime == 0) || (millis() > tsMQTTtime))
    {
      if (!this->mqttClient->isConnected())
      {
        mqttClient->setServer(this->getMQTTBrokerIp(), MQTT_PORT);
        if (mqttClient->connect(this->getDeviceName(), this->getMQTTUser(), this->getMQTTPassw()))
        {
          this->publishDeviceInfo();
          this->subscribe();
        }
      }
      tsMQTTtime = millis() + TIMEOUT_MQTT_CONNECT;
    } 
  }
 
  mqttClient->maintenance();
}

void ESPDevice::initMqttTopicCache()
{
  memset(this->mqttTopicCache, '\0', MQTT_TOPIC_CACHE);
  if (strlen(this->getMQTTPrefix()) > 0){
    strcat(this->mqttTopicCache, this->getMQTTPrefix());
    strcat(this->mqttTopicCache, "/");
  }

  char chip_id[20];
  itoa(ESP.getChipId(), chip_id, 10);  
  strcat(this->mqttTopicCache, chip_id);
  pMqttTopicCache = &this->mqttTopicCache[strlen(this->mqttTopicCache)];
}

char *ESPDevice::makeMqttTopic(char *topic)
{
  this->pMqttTopicCache[0] = '\0';
  strcat(this->pMqttTopicCache, topic);
  return this->mqttTopicCache;
}

IMqttClient *ESPDevice::getMqttClient()
{
  return this->mqttClient;
}

uint8 ESPDevice::getUpdateLastError()
{
  return conn.updateLastError;
}

void ESPDevice::getUpdateLastError(char *buf)
{
  switch (conn.updateLastError)
  {
  case UPDATE_ERROR_OK:
    strcpy(buf, E_OK);
    break;
  case UPDATE_ERROR_WRITE:
    strcpy(buf, E_WRITE);
    break;
  case UPDATE_ERROR_ERASE:
    strcpy(buf, E_ERASE);
    break;
  case UPDATE_ERROR_READ:
    strcpy(buf, E_READ);
    break;
  case UPDATE_ERROR_SPACE:
    strcpy(buf, E_SPACE);
    break;
  case UPDATE_ERROR_SIZE:
    strcpy(buf, E_SIZE);
    break;
  case UPDATE_ERROR_STREAM:
    strcpy(buf, E_STREAM);
    break;
  case UPDATE_ERROR_MD5:
    strcpy(buf, E_MD5);
    break;
  case UPDATE_ERROR_FLASH_CONFIG:
    strcpy(buf, E_FLASH_CONDIF);
    break;
  case UPDATE_ERROR_MAGIC_BYTE:
    strcpy(buf, E_MAGIC_BYTE);
    break;
  case UPDATE_ERROR_BOOTSTRAP:
    strcpy(buf, E_BOOTSTRAP);
    break;
  default:
    strcpy(buf, E_UNKNOWN);
  }
}

void ESPDevice::setUpdateLastError(uint8 error)
{
  conn.updateLastError = error;
  tsNeedSave = 1;
  ESPDevice::maintenance();
}

char *ESPDevice::getSSID()
{
  return conn.ssid;
}

char* ESPDevice::getMQTTBrokerIp()
{
  return conn.mqttBrokerIp;
}

char* ESPDevice::getMQTTUser(){
  return conn.mqttUserName;
}

void ESPDevice::setMQTTUser(char* mqttUser){
  strcpy(conn.mqttUserName, mqttUser);
}

void ESPDevice::setMQTTUser(const char* mqttUser){
  strcpy(conn.mqttUserName, mqttUser);
}

char* ESPDevice::getMQTTPassw(){
  return conn.mqttPassword;
}

void ESPDevice::setMQTTPassw(char* mqttPass){
  strcpy(conn.mqttPassword, mqttPass);
}

void ESPDevice::setMQTTPassw(const char* mqttPass){
  strcpy(conn.mqttPassword, mqttPass);
}

void ESPDevice::setMQTTBrokerIp(const char *mqttBrokerIp)
{
  strcpy(conn.mqttBrokerIp, mqttBrokerIp);
}

void ESPDevice::setMQTTBrokerIp(char *mqttBrokerIp)
{
  strcpy(conn.mqttBrokerIp, mqttBrokerIp);
}

void ESPDevice::setSSID(char *ssid)
{
  strcpy(conn.ssid, ssid);
}

void ESPDevice::setSSID(const char *ssid)
{
  strcpy(conn.ssid, ssid);
}

char *ESPDevice::getSSIDPass()
{
  return conn.ssidPass;
}

void ESPDevice::setSSIDPass(char *pass)
{
  strcpy(conn.ssidPass, pass);
}

void ESPDevice::setSSIDPass(const char *pass)
{
  strcpy(conn.ssidPass, pass);
}

char *ESPDevice::getDeviceName()
{
  return deviceName;
}

char *ESPDevice::getBuildTimeFirmware(char *buf)
{
  buf[0] = '\0';
  sprintf(buf, "%s %s", __DATE__, __TIME__);
  return buf;
}

char *ESPDevice::getNtpServer()
{
  return conn.ntp;
}

char *ESPDevice::upTime(unsigned long timestamp, char *uptime)
{
  int days = timestamp / DAY_MS;
  unsigned long leftOfDay = timestamp - (days * DAY_MS);
  int hours = leftOfDay / HOUR_MS;
  unsigned long leftOfHour = leftOfDay - (hours * HOUR_MS);
  int mins = leftOfHour / MIN_MS;

  String d = String(days);
  String h = String(hours);
  String m = String(mins);
  sprintf(uptime, "%s day %s hour %s min", d.c_str(), h.c_str(), m.c_str());
  return uptime;
}

bool ESPDevice::connectTo(const char *ssid, const char *psw)
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, psw);

  unsigned long timeout = millis() + TIMEOUT_WIFI_CONNECT;
  while ((timeout > millis()) && (WiFi.status() != WL_CONNECTED))
  {
    yield();
  };
  return WiFi.isConnected();
}

bool ESPDevice::tryConnectTo(const char *ssid, const char *psw)
{
  bool result = connectTo(ssid, psw);
  if (!result)
    tsReconnect = 0;
  return result;
}

bool ESPDevice::updateTimeByNtp(char *host)
{
  unsigned long time_ms = 0;
  bool result = false;
  WiFiUDP *udp = new WiFiUDP();
  udp->begin(321);

  byte packetBuf[NTP_PACKET_SIZE];
  memset(packetBuf, 0, NTP_PACKET_SIZE);
  packetBuf[0] = 0b11100011;
  packetBuf[1] = 0;
  packetBuf[2] = 6;
  packetBuf[3] = 0xEC;
  packetBuf[12] = 49;
  packetBuf[13] = 0x4E;
  packetBuf[14] = 49;

  packetBuf[15] = 52;

  IPAddress timeServerIP;
  WiFi.hostByName(host, timeServerIP);

  udp->beginPacket(timeServerIP, 123);
  udp->write(packetBuf, NTP_PACKET_SIZE);
  udp->endPacket();
  delay(1000);

  int respond = udp->parsePacket();
  if (respond > 0)
  {
    udp->read(packetBuf, NTP_PACKET_SIZE);
    unsigned long highWord = word(packetBuf[40], packetBuf[41]);
    unsigned long lowWord = word(packetBuf[42], packetBuf[43]);
    time_t secsSince1900 = highWord << 16 | lowWord;
    time_t time_ms = secsSince1900 - 2208988800UL + 3 * 3600L;

    setTime(time_ms);
    result = true;
  }
  delete udp;
  return result;
}

void ESPDevice::save()
{
  tsNeedSave = millis() + (SEC_MS * 2);
}

void ESPDevice::reset()
{
  EEPROM.begin(EEPROM_SIZE);
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

char *ESPDevice::getMQTTPrefix()
{
  return conn.mqttPrefix;
}

void ESPDevice::setMQTTPrefix(char *mqttPrefix)
{
  strcpy(conn.mqttPrefix, mqttPrefix);
  this->initMqttTopicCache();
}

void ESPDevice::setMQTTPrefix(const char *mqttPrefix)
{
  strcpy(conn.mqttPrefix, mqttPrefix);
  this->initMqttTopicCache();
}

void Ip2Str(char *target, IPAddress ip)
{
  for (int8_t i = 0; i < 4; i++)
  {
    itoa(ip[i], target, 10);
    target = target + strlen(target);
    if (i == 3)
      *target = '\0';
    else
      *target++ = '.';
  };
}
