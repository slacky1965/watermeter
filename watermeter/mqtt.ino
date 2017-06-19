void createMqttTopic() {
  
  String mac, s = wmConfig.mqttTopic;

  mac = makeMacAddress();

  s +=  "/" + mac + "/";
  
  mqttTopicHotIn = s + END_TOPIC_HOT_IN;
  mqttTopicColdIn = s + END_TOPIC_COLD_IN;
  mqttTopicHotOut = s + END_TOPIC_HOT_OUT;
  mqttTopicColdOut = s + END_TOPIC_COLD_OUT;
}


void callback(char *topic, byte *payload, unsigned int len) {
  // handle message arrived

  time_t timeFromServer;
  time_t waterFromServer;
  unsigned int pos;

  String s = "";
  String msg = "";
  String snew = "NEW";
  String stopic = topic;
  
  
  
  for (unsigned int i = 0; i < len; i++) {
    msg += (char)payload[i];
  }

  pos = msg.indexOf(' ');
  s = msg.substring(0, pos);
  timeFromServer = strtoul(s.c_str(), 0, 10);
  s = msg.substring(pos);
  waterFromServer = strtoul(s.c_str(), 0, 10);

  if (DEBUG) {
    Serial.print(msg);
    Serial.print(" => ");
    Serial.println(topic);
  }
  
  if (stopic.compareTo(mqttTopicHotIn) == 0) {
    if (msg.endsWith(snew)) {
      if (waterFromServer > wmConfig.hotWater)
      wmConfig.hotWater = waterFromServer;
      wmConfig.hotTime = timeFromServer;
      saveNewConfig = true;
    } else if (waterFromServer > wmConfig.hotWater) {
      wmConfig.hotWater = waterFromServer + wmConfig.litersInPulse;
      wmConfig.hotTime = timeFromServer;
      saveNewConfig = true;
    }
  } else if (stopic.compareTo(mqttTopicColdIn) == 0) {
    if (msg.endsWith(snew)) {
      wmConfig.coldWater = waterFromServer;
      wmConfig.coldTime = timeFromServer;
      saveNewConfig = true;
    } else if (waterFromServer > wmConfig.coldWater) {
      wmConfig.coldWater = waterFromServer + wmConfig.litersInPulse;
      wmConfig.coldTime = timeFromServer;
      saveNewConfig = true;
    }
  }
  
  if (saveNewConfig) {
    saveNewConfig = false;
    saveConfig();
  }
}


void mqttReconnect() {

  if (apModeNow) return;

  createMqttTopic();

  if (mqttFirstStart) {
    Serial.printf("Full name out topic for hot water: %s\n", mqttTopicHotOut.c_str());
    Serial.printf("Full name in topic for hot water: %s\n", mqttTopicHotIn.c_str());
    Serial.printf("Full name out topic for cold water: %s\n", mqttTopicColdOut.c_str());
    Serial.printf("Full name in topic for cold water: %s\n", mqttTopicColdIn.c_str());
    mqttFirstStart = false;
  }
  
  mqttClientId = MODULE_NAME;
  mqttClientId += "-";
  mqttClientId += makeMacAddress();
  
  mqttClient.setServer(wmConfig.mqttBroker, MQTT_PORT);
  
  if (!mqttClient.connected()) {
    if (DEBUG) Serial.printf("Connecting to MQTT server: %s\n", wmConfig.mqttBroker);
    if (mqttClient.connect(mqttClientId.c_str(), wmConfig.mqttUser, wmConfig.mqttPassword)) {
      if (DEBUG) Serial.printf("Connected to MQTT server: %s\n", wmConfig.mqttBroker);
      mqttClient.setCallback(callback);
      mqttClient.subscribe(mqttTopicHotIn.c_str());
      mqttClient.subscribe(mqttTopicColdIn.c_str());
      mqttRestart = false;
    } else {
      mqttRestart = true;
      if (DEBUG) {
        Serial.println("Could not connect to MQTT server");
        //printMqttState();
      }
    }
  }
}

void printMqttState() {

  if (DEBUG) {
    
    switch (mqttClient.state()) {
      case -4:
        Serial.println("MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time");
        break;
      case -3:
        Serial.println("MQTT_CONNECTION_LOST - the network connection was broken");
        break;
      case -2:
        Serial.println("MQTT_CONNECT_FAILED - the network connection failed");
        break;
      case -1:
        Serial.println("MQTT_DISCONNECTED - the client is disconnected cleanly");
        break;
      case 0:
        Serial.println("MQTT_CONNECTED - the cient is connected");
        break;
      case 1:
        Serial.println("MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT");
        break;
      case 2:
        Serial.println("MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier");
        break;
      case 3:
        Serial.println("MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection");
        break;
      case 4:
        Serial.println("MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected");
        break;
      case 5:
        Serial.println("MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect");
        break;
      default: 
      break;
    }
  }
}

