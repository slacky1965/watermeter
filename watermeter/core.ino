String makeMacAddress() {
  
  String mac = "";

  for (int i = 0; i < macAddress.length(); i++) {
    if (macAddress[i] != ':') mac += macAddress[i];
  }
  
  return mac;
}

unsigned long crc_update(unsigned long crc, byte data) {
  byte tbl_idx;
  tbl_idx = crc ^ (data >> (0 * 4));
  crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
  tbl_idx = crc ^ (data >> (1 * 4));
  crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
  return crc;
}


unsigned long crc_byte(byte *b, int len) {
  unsigned long crc = ~0L;
  int i;

  for (i = 0 ; i < len ; i++) {
    crc = crc_update(crc, *b++);
  }
  crc = ~crc;
  return crc;
}

/* Vcc in volt */
String returnVccStr() {
  String v = "";
  String Vcc;
  int volt;
  int voltInt;
  
  if (!EXT_POWER_CONTROL) {
    Vcc = "Vcc: ";
    voltInt = ESP.getVcc();
    volt = (voltInt*117+5000)/100;
  } else {
    Vcc = "Battery: ";
    voltInt = analogRead(BAT_VOLT_PIN);
    volt = 3300/1024*voltInt;
  }
/*  Serial.printf("voltInt: %d\n", voltInt);*/
  
    v += volt;

    Vcc += v.substring(0, 1);
    Vcc += ',';
    Vcc += v.substring(1, 3);
    Vcc += 'V';

    return Vcc;
    
}

/* received signal strength indicator in dBm */
String returnRssiStr() {
  String rssi = "WiFi: ";
  rssi += WiFi.RSSI();
  rssi += " dBm";
  return rssi;
}

/* init PIN */
void initPin() {
  pinMode(HOT_PIN, INPUT);
  digitalWrite(HOT_PIN, HIGH);
  pinMode(COLD_PIN, INPUT);
  digitalWrite(COLD_PIN, HIGH);
  if (EXT_POWER_CONTROL) {
    pinMode(EXT_POWER_PIN, INPUT_PULLDOWN_16);
  }
  
}

void startApMsg() {
  Serial.printf("WiFi network Name: %s, Password: %s\n", wmConfig.apSsid, wmConfig.apPassword);
  Serial.print("Go to: "); Serial.print(WiFi.softAPIP()); Serial.println(" please");
}

/* Init external interrupt           */
void initInterrupt() {
  
  attachInterrupt(digitalPinToInterrupt(HOT_PIN), hotInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(COLD_PIN), coldInterrupt, RISING);
}

/* External interrupt for hot water  */
void hotInterrupt() {
  counterHotWater++;
}

/* External interrupt for cold water */
void coldInterrupt() {
  counterColdWater++;
}

bool checkExtPower() {

  if (!EXT_POWER_CONTROL) return true;

  int val = digitalRead(EXT_POWER_PIN);

  if (val) {
    sleepDelay = 0;
    if (offWiFi) {
      if (DEBUG) Serial.println("External power high.");  
      offWiFi = false;
    }
    return true;
  }
  else {
    delay(1);
    if (!offWiFi) {
      if (sleepDelay > SLEEP_DELAY) {
        if (DEBUG) Serial.println("External power low.");
        offWiFi = true;
        sleepDelay = 0;
        return false;
      } else {
        sleepDelay++;
      }
    }
    return false;
  }
}

void sleepNow() {
  if (DEBUG) Serial.println("Light sleep now ...");
  apModeNow=staModeNow=false;
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); //light sleep mode
  gpio_pin_wakeup_enable(GPIO_ID_PIN(HOT_PIN), GPIO_PIN_INTR_LOLEVEL);  /* Set the interrupt to look for LOW pulses on HOT_PIN  */
  gpio_pin_wakeup_enable(GPIO_ID_PIN(COLD_PIN), GPIO_PIN_INTR_LOLEVEL); /* Set the interrupt to look for LOW pulses on COLD_PIN */
  wifi_fpm_open();
  delay(100);
  wifi_fpm_set_wakeup_cb(wakeupFromMotion); //wakeup callback
  wifi_fpm_do_sleep(0xFFFFFFF); 
  delay(100);
}

void wakeupFromMotion(void) {
  ESP.wdtFeed();
  initInterrupt();
  wifi_fpm_close();
/*  wifi_set_opmode(STATION_MODE);
  wifi_station_connect();*/
  if (DEBUG) Serial.println("Wake up from sleep.");
}

