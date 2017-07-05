void startWiFiAP() {

  Serial.println("Start WiFi AP Mode");
  
  WiFi.disconnect(true);
  delay(1000);
  WiFi.persistent(false);
  WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  delay(500);

  WiFi.mode(WIFI_AP);

  WiFi.softAP(wmConfig.apSsid, wmConfig.apPassword);
  
  restartWiFi = false;
  apModeNow = true;
  staModeNow = false;

  delay(1000);
}


bool startWiFiSTA() {
  
  Serial.println("Start WiFi STA Mode");
  if (DEBUG) {
    Serial.printf("Connecting to: %s \n", wmConfig.staSsid);
  }

  staConfigure = true;

  WiFi.disconnect(true);
  delay(1000);
  WiFi.persistent(false);
  WiFi.setPhyMode(WIFI_PHY_MODE_11G);
  delay(500);

  WiFi.mode(WIFI_STA);

  delay(1000);

  String mac = WiFi.macAddress();
  String mac2 = "";

  for (int i = 0; i < mac.length(); i++) {
    if (mac[i] != ':') mac2 += mac[i];
  }

  mac2.toUpperCase();

/*  String hostName = WEB_WATERMETER_FIRST_NAME;
  hostName += WEB_WATERMETER_LAST_NAME;*/
  String hostName = AP_SSID;
  hostName += "-";
  hostName += mac2;

  WiFi.hostname(hostName);

  WiFi.begin(wmConfig.staSsid, wmConfig.staPassword);

  delay(1000);

  // Wait for connection
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ < 20) {
    if (DEBUG) Serial.print(".");
    delay(500);
  }
  if (DEBUG) Serial.println();
  if(i == 21){
    if (DEBUG) {
      Serial.printf("Could not connect to: %s\n", wmConfig.staSsid);
    }
    //while(1) delay(500);  
    return false;
  }
  
  WiFi.macAddress(MacAddress);
  
  staMacStr = String(MacAddress[WL_MAC_ADDR_LENGTH - 6], HEX) + ":" +
              String(MacAddress[WL_MAC_ADDR_LENGTH - 5], HEX) + ":" +
              String(MacAddress[WL_MAC_ADDR_LENGTH - 4], HEX) + ":" +
              String(MacAddress[WL_MAC_ADDR_LENGTH - 3], HEX) + ":" +
              String(MacAddress[WL_MAC_ADDR_LENGTH - 2], HEX) + ":" +
              String(MacAddress[WL_MAC_ADDR_LENGTH - 1], HEX);
  staMacStr.toUpperCase();
  
  if (DEBUG) {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MacAddress "); Serial.println(staMacStr);
  }

  restartWiFi = false;
  apModeNow = false;
  staModeNow = true;

  return true;
}

