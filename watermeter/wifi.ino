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

  macAddress = WiFi.macAddress();
  String mac2 = makeMacAddress();

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
  
  if (DEBUG) {
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("MacAddress: "); Serial.println(macAddress);
  }

  restartWiFi = false;
  apModeNow = false;
  staModeNow = true;

  return true;
}

