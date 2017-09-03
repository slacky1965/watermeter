void setup() {

  Serial.begin(115200);

  Serial.printf("\n\nStart %s (%s)\n\n", MODULE_NAME, PLATFORM);

  counterHotWater = 0;
  counterColdWater = 0;
  
  initPin();                    /* core.ino   */

  initInterrupt();              /* core.ino   */

  initSD();                     /* sd.ino     */

  initDefConfig(&wmConfig);         /* config.ino */

  if (!readConfig()) {            /* sd.ino     */
    firstStart = true;
    if (sdOk) {
      mkDir(watermeterDirName); /* sd.ino     */
    }
  }

  if (!checkExtPower()) {
    while (sleepDelay <= SLEEP_DELAY) {
      checkExtPower();
    }
    checkExtPower();
  }


  if (firstStart || wmConfig.apMode || wmConfig.staSsid[0] == '0') {
    if (!offWiFi) {
      startWiFiAP();              /* wifi.ino   */
      delay(1000);
      startApMsg();               /* core.ino   */
    }
  } else {
    if (!offWiFi) {
      if (!startWiFiSTA()) {      /* wifi.ino   */
        Serial.println("No WiFi connect STA mode. Start AP mode");
        delay(500);
        startWiFiAP();            /* wifi.ino   */
/*      wmConfig.apMode = true;
        saveData();
        ESP.reset();*/
      }
    }
  }

  mqttFirstStart = true;

  if (staModeNow && WiFi.status() == WL_CONNECTED) { 
    startNTP();               /* time.ino     */
    mqttReconnect();          /* mqtt.ino     */
  }

  initWebServer();            /* web.ino      */


}




