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
  String Vcc = "Vcc: ";
  
  int voltInt = ESP.getVcc();
  unsigned long volt = (voltInt*117+5000)/100;

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
}

void startApMsg() {
  Serial.printf("WiFi network Name: %s, Password: %s\n", wmConfig.apSsid, wmConfig.apPassword);
  Serial.print("and Go to: "); Serial.print(WiFi.softAPIP()); Serial.println(" please");
}

/* Init external interrupt           */
void initInterrupt() {
  attachInterrupt(digitalPinToInterrupt(HOT_PIN), hotInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(COLD_PIN), coldInterrupt, FALLING);
}

/* External interrupt for hot water  */
void hotInterrupt() {
  counterHotWater++;
}

/* External interrupt for cold water */
void coldInterrupt() {
  counterColdWater++;
}

