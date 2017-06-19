#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>  
#include <SD.h>
#include <EEPROM.h>
#include <PubSubClient.h>                        /* MQTT library from https://github.com/knolleary/pubsubclient */
#include <TimeLib.h>                             /* Time library from https://github.com/PaulStoffregen/Time    */

#define DEBUG true

#define SD_PIN D0                                 /* SD use D0 for Wemos D1 Mini       */
#define HOT_PIN D1                                /* Number of Pin for hot water       */
#define COLD_PIN D2                               /* Number of Pin for cold water      */ 

/* Name and Version */
#define PLATFORM "Wemos D1 mini & Micro SD"
#define MODULE_VERSION "v1.2"
#define MODULE_NAME "WaterMeter " MODULE_VERSION
#define WEB_WATERMETER_FIRST_NAME "Water"
#define WEB_WATERMETER_LAST_NAME "Meter"

/* Directory and name of config files for SD card */
#define DELIM "/"
#define WM_DIR DELIM "wmeter"                    /* Drirectory for config file        */
#define WM_CONFIG_FILE WM_DIR DELIM "wmeter.dat" /* Full path and name of config file */

/* For config (default settings) */
#define WEB_ADMIN_LOGIN "Admin"                  /* Login for web Auth                */
#define WEB_ADMIN_PASSWORD "1111"                /* Password for web Auth             */
#define AP_SSID "WaterMeter_" MODULE_VERSION     /* Name WiFi AP Mode                 */
#define AP_PASSWORD "12345678"                   /* Password WiFi AP Mode             */
#define MQTT_USER "test"                         /* mqtt user                         */
#define MQTT_PASSWORD "1111"                     /* mqtt password                     */
#define MQTT_BROKER "192.168.1.1"                /* URL mqtt broker                   */
#define MQTT_TOPIC "/WaterMeter"                 /* Primary mqtt topic name           */
#define LITERS_IN_PULSE 10                       /* How many liters in one pulse      */
#define TIME_ZONE 3                              /* Default Time Zone                 */

/* for TimeLib (NTP and clock) */
#define SYNC_TIME 21600                          /* Interval sync to NTP Server       */
#define NTP_SERVER_NAME "ntp4.stratum2.ru"       /* URL of NTP server                 */ 
#define NTP_LOCAL_PORT 8888                      /* NTP local port                    */
unsigned int localPort = NTP_LOCAL_PORT;
const int NTP_PACKET_SIZE = 48;                  /* NTP time is in the first 48 bytes of message */
byte packetBuffer[NTP_PACKET_SIZE];              /* buffer to hold incoming & outgoing packets   */
WiFiUDP Udp;


/* For SD */
bool sdOk = false;                              
String watermeterDirName = WM_DIR;
String configFileName = WM_CONFIG_FILE;

/* For EEPROM */
#define EEPROM_START 0
bool setEEPROM = false;
uint32_t memcrc; uint8_t *p_memcrc = (uint8_t*)&memcrc;

static PROGMEM prog_uint32_t crc_table[16] = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

/* For WiFi */
uint8_t MacAddress[WL_MAC_ADDR_LENGTH];
String staMacStr;
WiFiClient wifiClient;

/* For MQTT */
#define MQTT_PORT 1883
#define END_TOPIC_HOT_IN "HotWater" DELIM "In"
#define END_TOPIC_COLD_IN "ColdWater" DELIM "In"
#define END_TOPIC_HOT_OUT "HotWater" DELIM "Out"
#define END_TOPIC_COLD_OUT "ColdWater" DELIM "Out"
PubSubClient mqttClient(wifiClient);
String mqttClientId;                      /* MODULE_NAME-MacAddress                   */
String mqttTopicHotOut, mqttTopicColdOut, mqttTopicHotIn, mqttTopicColdIn;
                                          /* Full name Topic -                                          *
                                           *  mqttTopicHotOut  - "MQTTTOPIC/MacAddress/HotWater/Out"    *
                                           *  mqttTopicColdOut - "MQTTTOPIC/MacAddress/ColdWater/Out"   *
                                           *  mqttTopicHotIn   - "MQTTTOPIC/MacAddress/HotWater/In"     *
                                           *  mqttTopicColdIn  - "MQTTTOPIC/MacAddress/ColdWater/In"    *
                                           *  see mqtt.ino                                              */


/* For Web */
String webServerIndex;
String httpRoot = DELIM;
ESP8266WebServer webServer(80);

/* Flags */
bool rebootNow = false;
bool restartWiFi = false;
bool apModeNow = true;
bool staModeNow = false;
bool staConfigure = false;
bool defaultConfig = false;
bool saveNewConfig = false;
bool firstStart = false;
bool responseNTP = false;
bool mqttRestart = false;
bool mqttFirstStart = true;
bool subsHotWater = false;
bool subsColdWater = false;

/* Counter of water for interrupts */
volatile unsigned long counterHotWater, counterColdWater;

/* struct of config file */
typedef struct config {
  char webAdminLogin[16];      /* Login for web Auth                    */
  char webAdminPassword[16];   /* Password for web Auth                 */
  bool fullSecurity;           /* true - web Auth, false - free         */
  char staSsid[16];            /* STA SSID WiFi                         */
  char staPassword[16];        /* STA Password WiFi                     */
  bool apMode;                 /* true - AP Mode, false - STA Mode      */
  char apSsid[16];             /* WiFi Name in AP mode                  */
  char apPassword[16];         /* WiFi Password in AP mode              */
  char mqttBroker[32];         /* mqtt broker                           */
  char mqttUser[16];           /* mqtt user                             */
  char mqttPassword[16];       /* mqtt password                         */
  char mqttTopic[64];          /* mqtt topic                            */
  char ntpServerName[32];      /* URL NTP server                        */
  int timeZone;                /* Time Zone                             */
  byte litersInPulse;          /* liters in pulse                       */
  time_t hotTime;              /* Last update time of hot water         */
  unsigned long hotWater;      /* Last number of liters hot water       */
  time_t coldTime;             /* Last update time of cold water        */
  unsigned long coldWater;     /* Last number of litres cold water      */
} _config;

_config wmConfig;

time_t mqttReconnectTime = 0;
time_t staReconnectTime = 0;
time_t ntpReconnectTime = 0;

ADC_MODE (ADC_VCC);


void loop () {
  String s;
  webServer.handleClient();

  /* Restart connecting to NTP server one time in 60 sec */
  if (!responseNTP && !apModeNow && ntpReconnectTime+60000 < millis() && WiFi.status() == WL_CONNECTED)  {
    ntpReconnectTime = millis();
    startNTP();
  }

  /* If reconfigured counter of hot water */
  if (subsHotWater) {
    subsHotWater = false;
    s = "";
    s += wmConfig.hotTime;
    s = s + " " + wmConfig.hotWater + " NEW";
      
    if (DEBUG) Serial.printf("%s <== %s\n", mqttTopicHotOut.c_str(), s.c_str());
    
    if (mqttClient.connected())  mqttClient.publish(mqttTopicHotOut.c_str(),s.c_str());
  }

  /* If reconfigured counter of cold water */
  if (subsColdWater) {
    subsColdWater = false;
    s = "";
    s += wmConfig.coldTime;
    s = s + " " + wmConfig.coldWater + " NEW";
    
    if (DEBUG) Serial.printf("%s <== %s\n", mqttTopicColdOut.c_str(), s.c_str());

    if (mqttClient.connected()) mqttClient.publish(mqttTopicColdOut.c_str(),s.c_str());
  }

  /* If counter of hot water was added */
  if (counterHotWater > 0) {
    wmConfig.hotTime = now();
    s = "";
    s += wmConfig.hotTime;
    wmConfig.hotWater += counterHotWater * wmConfig.litersInPulse;
    s = s + " " + wmConfig.hotWater;
    if (DEBUG) {
      Serial.print(mqttTopicHotOut + " <== "); Serial.println(s);
    }
    if (mqttClient.connected()) mqttClient.publish(mqttTopicHotOut.c_str(),s.c_str());
    else saveConfig();
    counterHotWater = 0;
  }

  /* If counter of cold water was added */
  if (counterColdWater > 0) {
    wmConfig.coldTime = now();
    s = "";
    s += wmConfig.coldTime;
    wmConfig.coldWater += counterColdWater * wmConfig.litersInPulse;
    s = s + " " + wmConfig.coldWater;
    if (DEBUG) {
      Serial.print(mqttTopicColdOut + " <== "); Serial.println(s);
    }
    if (mqttClient.connected()) mqttClient.publish(mqttTopicColdOut.c_str(),s.c_str());
    else saveConfig();
    counterColdWater = 0;
  }

  
  if (mqttClient.connected()) mqttClient.loop();
  else mqttRestart = true;

  /* reconnect to mqqt broker one time in 10 sec. */
  if (mqttRestart && mqttReconnectTime+10000 < millis()) {
    if (staModeNow && WiFi.status() == WL_CONNECTED) {
      mqttClient.disconnect();
      mqttReconnect();
      printMqttState();
    }
    mqttReconnectTime = millis();
  }

  /* checking connect to WiFi network one time in 30 sec */
  if (!wmConfig.apMode && staConfigure && staReconnectTime+30000 < millis() && (apModeNow || (staModeNow && WiFi.status() != WL_CONNECTED))) {
    staReconnectTime = millis();
    if (DEBUG) Serial.printf("Check WiFi network: %s\n", wmConfig.staSsid);
    int n = WiFi.scanNetworks(); 
    if (n != 0) {
      for (int i = 0; i < n; i++) {
        if (strcmp(wmConfig.staSsid, WiFi.SSID(i).c_str()) == 0) {
          if (startWiFiSTA()) {
            apModeNow = false;
            staModeNow = true;
            wmConfig.apMode = false;
            responseNTP = false;
          }
          break;
        }
      }
    }
  }

  /* reset soft watchdog */
  ESP.wdtFeed();
}


