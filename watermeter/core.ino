String makeMacAddress() {
  
  String mac = "";

  for (int i = 0; i < macAddress.length(); i++) {
    if (macAddress[i] != ':') mac += macAddress[i];
  }
  
  return mac;
}

/* For read and write to EEPROM */
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

/* Vcc or Battery in volt */
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
    volt = 5000/1024*voltInt*1.19;
  }
/*  Serial.printf("voltInt: %d\n", voltInt);*/
  
    v += volt;

    Vcc += v.substring(0, 1);
    Vcc += ',';
    Vcc += v.substring(1, 3);
    Vcc += 'V';

    return Vcc;
    
}

/* Received signal strength indicator in dBm */
String returnRssiStr() {
  String rssi = "WiFi: ";
  rssi += WiFi.RSSI();
  rssi += " dBm";
  return rssi;
}

/* Init PIN */
void initPin() {
  pinMode(HOT_PIN, INPUT_PULLUP);
  pinMode(COLD_PIN, INPUT_PULLUP);
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

  hotInt = coldInt = 0;
}

/* External interrupt for hot water  */
void hotInterrupt() {
  /* First interrupt if hotInt == 0  */
  if (hotInt == 0) {    
    hotInt++;
    hotTimeBounce = millis();
  } else os_timer_disarm(&hotTimer);
  os_timer_arm(&hotTimer, TIME_BOUNCE, true);
}

/* External interrupt for cold water */
void coldInterrupt() {
  /* First interrupt if coldInt == 0 */
  if (coldInt == 0) {
    coldInt++;
    coldTimeBounce = millis();
  } else os_timer_disarm(&coldTimer);
  os_timer_arm(&coldTimer, TIME_BOUNCE, true);
}

void hotTimerCallback(void *pArg) {
  /* If a long low level, then retiming hotTimeBounce */
  if (!digitalRead(HOT_PIN)) {
    hotTimeBounce = millis();
    return;
  }

  if (digitalRead(HOT_PIN) && hotTimeBounce + TIME_BOUNCE > millis()) return;
  
  os_timer_disarm(&hotTimer);

  hotInt = 0;

  counterHotWater++;  
}

void coldTimerCallback(void *pArg) {
  
  if (!digitalRead(COLD_PIN)) {
    coldTimeBounce = millis();
    return;
  }

  if (digitalRead(COLD_PIN) && coldTimeBounce + TIME_BOUNCE > millis()) return;
  
  os_timer_disarm(&coldTimer);

  coldInt = 0;

  counterColdWater++;  
}

bool checkExtPower() {

  if (!EXT_POWER_CONTROL) return true;

  int val = digitalRead(EXT_POWER_PIN);

  if (val) {
    powerLow = false;
    sleepDelay = 0;
    if (sleepNow) {
      if (DEBUG) Serial.println("External power high.");  
      sleepNow = false;
    }
    return true;
  }
  else {
    powerLow = true;
    delay(1);
    if (!sleepNow) {
      if (sleepDelay > SLEEP_DELAY) {
        if (DEBUG) Serial.println("External power low.");
        sleepNow = true;
        sleepDelay = 0;
        return false;
      } else {
        sleepDelay++;
      }
    }
    return false;
  }
}

void sleepOnNow() {
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
  if (DEBUG) Serial.println("Wake up from sleep.");
  sleepNow = false; 
}

