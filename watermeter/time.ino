/* Get time_t without timezone */
time_t timeTwTZ() {
  time_t t = now();
  return t;
}

/* Get time_t with timezone    */
time_t localTimeT() {
  time_t t = timeTwTZ();
  t += wmConfig.timeZone * SECS_PER_HOUR;
  return t;
}

String localUptime() {
  String s;
  unsigned long milli;
  if (firstNTP) {
    milli = millis();
  } else {
    milli = (timeTwTZ() - timeStart)*1000;
  }

  unsigned long secs = milli / 1000, mins = secs / 60;
  unsigned int hours = mins / 60, days = hours / 24;
  milli -= secs * 1000;
  secs -= mins * 60;
  mins -= hours * 60;
  hours -= days * 24;
  s += days != 0 ?  (String)days : "";
  s += days != 0 ?  "d " : "";
  s += hours != 0 ?  (String)hours : "";
  s += hours != 0 ?  ":" : "";
  s += mins > 9 ?  "" : "0";
  s += mins;
  s += ":";
  s += secs > 9 ?  "" : "0";
  s += secs;

  return s;
}


String localTimeStr() {
  time_t t = localTimeT();
  String curTime = "";
  curTime += strDigits(hour(t)) + ":" + strDigits(minute(t)) + ":" + strDigits(second(t));
  curTime += " " + strDigits(day(t)) + "." + strDigits(month(t)) + "." + strDigits(year(t));
  return curTime;
}

String strDigits(int digits)
{
  // utility for digital clock display: prints preceding colon and leading 0
  String s = "";
  if (digits < 10) s += "0";
  s += digits;
  return s;
}


void startNTP() {
  Udp.begin(localPort);
  if (DEBUG) {
    Serial.println("Starting UDP");
    Serial.print("Local port: ");
    Serial.println(Udp.localPort());
    Serial.println("waiting for sync");
  }
  setSyncProvider(getNtpTime);
  setSyncInterval(SYNC_TIME);

  if (DEBUG) {
    Serial.printf("Local time: %s\n", localTimeStr().c_str());
  }
}



/*-------- NTP code ----------*/


time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  if (DEBUG) Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(wmConfig.ntpServerName, ntpServerIP);
  if (DEBUG) {
    Serial.print(wmConfig.ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
  }
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      if (DEBUG) Serial.println("Receive NTP Response");
      responseNTP = true;
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL;
    }
  }
  if (DEBUG) Serial.println("No NTP Response :-(");
  responseNTP = false;
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
} 

