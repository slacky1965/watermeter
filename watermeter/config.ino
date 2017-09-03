void clearConfig(_config *config) {
  memset((void*)config, 0, sizeof(_config));
}
  
void initDefConfig(_config *config) {
  String s;
  
  clearConfig(config);
  
  s = WEB_ADMIN_LOGIN + '\0';
  s.toCharArray(config->webAdminLogin, s.length()+1);
  s = WEB_ADMIN_PASSWORD + '\0';
  s.toCharArray(config->webAdminPassword, s.length()+1);
  config->fullSecurity = false;
  config->staSsid[0] = 0;
  config->staPassword[0] = 0;
  config->apMode = true;
  s = AP_SSID + '\0';
  s.toCharArray(config->apSsid, s.length()+1);
  s = AP_PASSWORD + '\0';
  s.toCharArray(config->apPassword, s.length()+1);
  s = MQTT_BROKER + '\0';
  s.toCharArray(config->mqttBroker, s.length()+1);
  s = MQTT_USER + '\0';
  s.toCharArray(config->mqttUser, s.length()+1);
  s = MQTT_PASSWORD + '\0';
  s.toCharArray(config->mqttPassword, s.length()+1);
  s = MQTT_TOPIC + '\0';
  s.toCharArray(config->mqttTopic, s.length()+1);
  s = NTP_SERVER_NAME + '\0';
  s.toCharArray(config->ntpServerName, s.length()+1);
  config->timeZone = TIME_ZONE;
  config->litersPerPulse = LITERS_PER_PULSE;
  config->hotTime = config->coldTime = now();
  config->hotWater = config->coldWater = 0;

  staConfigure = false;
}

/* copy from tmp _config to wmConfig */

void setConfig(_config *config) {
  String s;
  unsigned int i;

  clearConfig(&wmConfig);
  
  s = config->webAdminLogin + '\0';
  s.toCharArray(wmConfig.webAdminLogin, s.length()+1);
  s = config->webAdminPassword + '\0';
  s.toCharArray(wmConfig.webAdminPassword, s.length()+1);
  wmConfig.fullSecurity = config->fullSecurity;
  s = config->staSsid + '\0';
  s.toCharArray(wmConfig.staSsid, s.length()+1);
  s = config->staPassword + '\0';
  s.toCharArray(wmConfig.staPassword, s.length()+1);
  wmConfig.apMode = config->apMode;
  s = config->apSsid + '\0';
  s.toCharArray(wmConfig.apSsid, s.length()+1);
  s = config->apPassword + '\0';
  s.toCharArray(wmConfig.apPassword, s.length()+1);
  s = config->mqttBroker + '\0';
  s.toCharArray(wmConfig.mqttBroker, s.length()+1);
  s = config->mqttUser + '\0';
  s.toCharArray(wmConfig.mqttUser, s.length()+1);
  s = config->mqttPassword + '\0';
  s.toCharArray(wmConfig.mqttPassword, s.length()+1);
  s = config->mqttTopic + '\0';
  s.toCharArray(wmConfig.mqttTopic, s.length()+1);
  s = config->ntpServerName + '\0';
  s.toCharArray(wmConfig.ntpServerName, s.length()+1);
  wmConfig.timeZone = config->timeZone;
  wmConfig.litersPerPulse = config->litersPerPulse;
  wmConfig.hotTime = config->hotTime;
  wmConfig.coldTime = config->coldTime;
  wmConfig.hotWater = config->hotWater;
  wmConfig.coldWater = config->coldWater;
}


