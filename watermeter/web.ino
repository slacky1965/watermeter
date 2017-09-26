void returnOK() {
  webServer.send(200, "text/plain", "OK");
}

void returnFail(String msg) {
  webServer.send(500, "text/plain", msg + "\r\n");
}

void initWebServer (void) {


  webServerIndex = htmlBegin();
  webServerIndex += headBegin();
  webServerIndex += metaBegin();
  webServerIndex += titleBegEnd();
  webServerIndex += "<style type=\"text/css\">\r\n";
  webServerIndex += "@media (max-width: 1024px) {\r\n";
  webServerIndex += "  div#main {\r\n";
  webServerIndex += "    width: 100%;\r\n";
  webServerIndex += "    height: auto;\r\n";
  webServerIndex += "    }\r\n";
  webServerIndex += "}\r\n";
  webServerIndex += "@media (max-width: 768px) {\r\n";
  webServerIndex += "  div#main {\r\n";
  webServerIndex += "    width: 100%;\r\n";
  webServerIndex += "    height: auto;\r\n";
  webServerIndex += "  }\r\n";
  webServerIndex += "}\r\n";
  webServerIndex += "@media (max-width: 480px) {\r\n";
  webServerIndex += "  div#main {\r\n";
  webServerIndex += "    width: 100%;\r\n";
  webServerIndex += "    height: auto;\r\n";
  webServerIndex += "  }\r\n";
  webServerIndex += "}\r\n";
  webServerIndex += "@media (max-width: 320px) {\r\n";
  webServerIndex += "  div#main {\r\n";
  webServerIndex += "    width: 100%;\r\n";
  webServerIndex += "    height: auto;\r\n";
  webServerIndex += "  }\r\n";
  webServerIndex += "}\r\n";
  webServerIndex += "</style>";

  webServerIndex += headEnd();      
  webServerIndex += bodyBegin();

  webServerIndex += "<form method='POST' action='/update' enctype='multipart/form-data'>\r\n";
  webServerIndex += "<input type='file' name='update'>\r\n";
  webServerIndex += "<input type='submit' value='Update'></form>\r\n";
  webServerIndex += "<p><a href=\"config\">Return to Config</a></p>\r\n";  

  webServerIndex += bodyEnd();
  webServerIndex += htmlEnd();


  webServer.on("/", handleRoot);
  webServer.on("/config", handleConfig);
/*  webServer.on("/time", handleTime);*/
  webServer.onNotFound(handleNotFound);
  
  webServer.on("/upload", HTTP_GET, [](){
    if (wmConfig.fullSecurity || wmConfig.configSecurity) {
      if (!webServer.authenticate(wmConfig.webAdminLogin, wmConfig.webAdminPassword))
        return webServer.requestAuthentication();
    }
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "text/html", webServerIndex);
  });
  
  webServer.on("/update", HTTP_POST, [](){
    webServer.sendHeader("Connection", "close");
    webServer.sendHeader("Access-Control-Allow-Origin", "*");
    webServer.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
    ESP.restart();
  },[](){
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START){
      Serial.setDebugOutput(true);
      WiFiUDP::stopAll();
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){ //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });

  webServer.begin();
}

String htmlBegin() {
  String s =  "<!DOCTYPE html>\r\n";
         s += "<html>\r\n";
  return s;
}

String htmlEnd() {
  String s = "</html>\r\n";
  return s;
}

String headBegin() {
  String s = "<head>\r\n";
  return s;
}

String headEnd() {
  String s = "</head>\r\n";
  return s;
}

String metaBegin() {
  String s = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" charset=utf-8>\r\n";
         s += "<meta charset=\"UTF-8\">\r\n";
  return s;
}

String titleBegEnd(){
  String s = "<title>";
  s += PLATFORM;
  s += ". ";
  s += MODULE_NAME;
  s += "</title>\r\n";
  return s;
}

String bodyBegin() {
  String s = "<body>\r\n";
  return s;
}

String bodyEnd() {
  String s = "</body>\r\n";
  return s;
}

String styleBegEnd() {

  String s = "<style type=\"text/css\">\r\n";
  s += "* {\r\n";
  s += "  margin: 0;\r\n";
  s += "  padding: 0;\r\n";
  s += "  border: 0;\r\n";
  s += "  vertical-align: baseline;\r\n";
  s += "  box-sizing: border-box;\r\n";
  s += "}\r\n";
  s += "div#main {\r\n";
  s += "  font: 14pt/12pt sans-serif;\r\n";
  s += "  text-align: center;\r\n";
  s += "  max-width: 1000px;\r\n";
  s += "  height: auto;\r\n";
  s += "  width: 35em;\r\n";
  s += "  margin: 1em auto 0;\r\n";
  s += "}\r\n";
  s += "div#main p {\r\n";
  s += "  padding: 0.4em;\r\n";
  s += "}\r\n";
  s += "div#main a {\r\n";
  s += "  color: #333;\r\n";
  s += "}\r\n";
  s += "form div {\r\n";
  s += "  display: inline-block;\r\n";
  s += "  padding-bottom: 0.2em;\r\n";
  s += "}\r\n";
  s += "form div label {\r\n";
  s += "  display: inline-block;\r\n";
  s += "  width: 6em;\r\n";
  s += "}\r\n";
  s += "table.block {\r\n";
  s += "  width: 90%;\r\n";
  s += "  margin: 0 auto;\r\n";
  s += "}\r\n";
  s += "table.block th {\r\n";
  s += "  text-align: left;\r\n";
  s += "  color: white;\r\n";
  s += "  border: 1px solid #888;\r\n";
  s += "  border-radius: 0.2em;\r\n";
  s += "}\r\n";
  s += "table.header td {\r\n";
  s += "  padding: 0.5em;\r\n";
  s += "}\r\n";
  s += "div.header-left {\r\n";
  s += "  text-align: right;\r\n";
  s += "  vertical-align: top;\r\n";
  s += "  float: none;\r\n";
  s += "  display: inline-block;\r\n";
  s += "}\r\n";
  s += "div.header-right {\r\n";
  s += "  font-size: 80%;\r\n";
  s += "  text-align: left;\r\n";
  s += "  float: none;\r\n";
  s += "  display: inline-block;\r\n";
  s += "}\r\n";
  s += "p.logo {\r\n";
  s += "  font-size: 300%;\r\n";
  s += "  padding: 0.5em;\r\n";
  s += "}\r\n";
  s += "input {\r\n";
  s += "  border: 1px black solid;\r\n";
  s += "  border-radius: 0.2em;\r\n";
  s += "  padding: 0.3em;\r\n";
  s += "}\r\n";
  s += "div.large-input {\r\n";
  s += "  border: 2px black solid;\r\n";
  s += "  border-radius: 0.5em;\r\n";
  s += "  text-align: right;\r\n";
  s += "  padding: 0.7em;\r\n";
  s += "  margin-top: 0.5em;\r\n";
  s += "  width: 90%;\r\n";
  s += "  font-size: 125%;\r\n";
  s += "  margin: 0 auto;\r\n";
  s += "}\r\n";
  s += "div#cold {\r\n";
  s += "  border-color: #09c;\r\n";
  s += "}\r\n";
  s += "div#hot {\r\n";
  s += "  border-color: #c63;\r\n";
  s += "}\r\n";
  s += "button.set-conf {\r\n";
  s += "  margin-top: 2em;\r\n";
  s += "  padding: 0.5em 3.5em;\r\n";
  s += "  height: 2em;\r\n";
  s += "  border: 1px solid #888;\r\n";
  s += "  border-radius: 0.2em;\r\n";
  s += "  font-size: 100%;\r\n";
  s += "}\r\n";
  s += "button.settings-button {\r\n";
  s += "  margin-top: 2em;\r\n";
  s += "  width: 90%;\r\n";
  s += "  padding: 0.3em;\r\n";
  s += "  height: 3em;\r\n";
  s += "  border: 1px solid #888;\r\n";
  s += "  border-radius: 0.2em;\r\n";
  s += "  font-size: 120%;\r\n";
  s += "}\r\n";
  s += "@media (max-width: 1024px) {\r\n";
  s += "  div#main {\r\n";
  s += "    width: 100%;\r\n";
  s += "    height: auto;\r\n";
  s += "    }\r\n";
  s += "}\r\n";
  s += "@media (max-width: 768px) {\r\n";
  s += "  div#main {\r\n";
  s += "    width: 100%;\r\n";
  s += "    height: auto;\r\n";
  s += "  }\r\n";
  s += "}\r\n";
  s += "@media (max-width: 480px) {\r\n";
  s += "  div#main {\r\n";
  s += "    width: 100%;\r\n";
  s += "    height: auto;\r\n";
  s += "  }\r\n";
  s += "  p.logo {\r\n";
  s += "    font-size: 100%;\r\n";
  s += "    padding: 0.5em;\r\n";
  s += "  }\r\n";
  s += "}\r\n";
  s += "@media (max-width: 320px) {\r\n";
  s += "  div#main {\r\n";
  s += "    width: 100%;\r\n";
  s += "    height: auto;\r\n";
  s += "  }\r\n";
  s += "  p.logo {\r\n";
  s += "    font-size: 100%;\r\n";
  s += "    padding: 0.5em;\r\n";
  s += "  }\r\n";
  s += "}\r\n";
  s += "</style>";

  return s;
}

String htmlLogo() {
  
  String s = "<p></p>\r\n";
  s += "<div style=\"background-color: white; margin: 0.6em;\">\r\n";
  s += "<div class=\"header-left\">\r\n";
  s += "<p class=\"logo\" style=\"padding-top: 0.5em;\">\r\n";
  s += "<strong>\r\n";
  s += "<span style=\"color: #00008B\">";
  s += WEB_WATERMETER_FIRST_NAME;
  s += "</span>\r\n";
  s += "<span style=\"color: #FF0000\">";
  s += WEB_WATERMETER_LAST_NAME;
  s += "</span>\r\n";
  s += "</strong>\r\n";
  s += "</p>\r\n";
  s += "<p>";
  s += PLATFORM;
  s += "</p>\r\n";
  s += "<p style=\"font-size: 80%\">";
  s += "Version ";
  s += MODULE_VERSION;
  s += "</p>\r\n";
  s += "</div>\r\n";
  s += "<div class=\"header-right\">\r\n";
  s += "<p>";
  s += "<p>Free memory: ";
  s += ESP.getFreeHeap();
  s += " bytes</p>\r\n";
  s += "<p>Uptime: ";
  s += localUptime();
  s += "</p>\r\n";
  s += "<p>" + returnVccStr() + ". " + returnRssiStr() + "</p>";
  s += "<p>Local time: ";
  s += localTimeStr();
  s += "</p>\r\n";
  s += "</div>\r\n";
  s += "</div>\r\n";

  return s;
}



void handleRoot() {

  String fract, s;
  
  if (firstStart) {
    handleConfig();
    return;
  }

  if (wmConfig.fullSecurity) {
    if (!webServer.authenticate(wmConfig.webAdminLogin, wmConfig.webAdminPassword))
    return webServer.requestAuthentication();
  }

  s = htmlBegin();
  s += headBegin();
  s += metaBegin();
  s += "<meta http-equiv=\"refresh\" content=\"15\";URL=\"/\">";
  s += titleBegEnd();
  s += styleBegEnd();
  s += headEnd();      
  s += bodyBegin();

  s += "<iframe width=\"0\" height=\"0\" border=\"0\" name=\"dummyframe\" id=\"dummyframe\"></iframe>\r\n";
  s += "<div id=\"main\">\r\n";
  s += htmlLogo();

  s += "<div>\r\n";
/*  s += "<p><div class=\"large-input\" id=\"cold\">456.789</div></p>\r\n";
  s += "<p><div class=\"large-input\" id=\"hot\">123.456</div></p>\r\n";*/
  s += "<p><div class=\"large-input\" id=\"cold\">";
  s += wmConfig.coldWater/1000;
  s += ".";
  fract = wmConfig.coldWater%1000;
  if (fract.length() == 2) fract = "0" + fract;
  else if (fract.length() == 1) fract = "00" + fract;
  s += fract;
  s += "</div></p>\r\n";
  s += "<p><div class=\"large-input\" id=\"hot\">";
  s += wmConfig.hotWater/1000;
  s += ".";
  fract = wmConfig.hotWater%1000;
  if (fract.length() == 2) fract = "0" + fract;
  else if (fract.length() == 1) fract = "00" + fract;
  s += fract;
  s += "</div></p>\r\n";
  s += "<form>\r\n";
  s += "<input type=\"button\" class=\"settings-button\" onclick=\"window.location=\'/config\';\" value=\"Configure Settings\">\r\n";
  s += "</form>\r\n";
  s += "</div>\r\n";
  s += "</div>\r\n";

  
  s += bodyEnd();
  s += htmlEnd();

  webServer.send(200, "text/html", s);

}

void handleTime() {
  String s;
}

void handleConfig() {

  String s;

  if (wmConfig.fullSecurity || wmConfig.configSecurity) {
    if (!webServer.authenticate(wmConfig.webAdminLogin, wmConfig.webAdminPassword))
    return webServer.requestAuthentication();
  }
  
  s = htmlBegin();
  s += headBegin();
  s += metaBegin();
  s += titleBegEnd();
  s += styleBegEnd();
  s += headEnd();      
  s += bodyBegin();

  s += "<iframe width=\"0\" height=\"0\" border=\"0\" name=\"dummyframe\" id=\"dummyframe\"></iframe>\r\n";
  s += "<div id=\"main\">\r\n";
  s += htmlLogo();
  s += "<div>\r\n";
  s += "<form action=\"/settings\" target=\"dummyframe\">\r\n";
  s += "<table class=\"block\">\r\n";
  s += "<tr>\r\n";
  s += "<th style=\"background-color: #999\"><p>Admin auth settings:</p></th>\r\n";
  s += "</tr>\r\n";
  s += "<tr>\r\n";
  s += "<td>\r\n";
  s += "<p>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"login\">Login:</label>\r\n";
  s += "<input type=\"text\" name=\"login\" id=\"login\" value=\"" + (String)wmConfig.webAdminLogin + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"password\">Password:</label>\r\n";
  s += "<input type=\"text\" name=\"password\" id=\"password\" value=\"" + (String)wmConfig.webAdminPassword + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "</p>\r\n";
  s += "<p>\r\n";
  s += "<input type=\"checkbox\" name=\"fullsecurity\" id=\"fullsecurity\" value=\"true\"";
  if (wmConfig.fullSecurity) s += "checked>\r\n";
  else s += ">\r\n";
  s += "<label for=\"fullsecurity\">Full Security</label>\r\n";
  s += "<input type=\"checkbox\" name=\"configsecurity\" id=\"configsecurity\" value=\"true\"";
  if (wmConfig.configSecurity) s += "checked>\r\n";
  else s += ">\r\n";
  s += "<label for=\"configsecurity\">Config Security</label>\r\n";
  s += "</p>\r\n";
  s += "</td>\r\n";
  s += "</tr>\r\n";
  s += "</table>\r\n";
  s += "<table class=\"block\">\r\n";
  s += "<tr>\r\n";
  s += "<th style=\"background-color: #999\"><p>WiFi options:</p></th>\r\n";
  s += "</tr>\r\n";
  s += "<tr>\r\n";
  s += "<td>\r\n";
  s += "<p>\r\n";
  s += "<input type=\"radio\" name=\"wifi-mode\" id=\"wifi-mode-station\" value=\"station\" ";
  if (!wmConfig.apMode) s += "checked>\r\n";
  else s += ">\r\n";
  s += "<label for=\"wifi-mode-station\">Station mode</label>\r\n";
  s += "<input type=\"radio\" name=\"wifi-mode\" id=\"wifi-mode-ap\" value=\"ap\" ";
  if (wmConfig.apMode) s += "checked>\r\n";
  else s += ">\r\n";
  s += "<label for=\"wifi-mode-ap\">AP mode</label>\r\n";
  s += "</p>\r\n";
  s += "<p>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"wifi-ap-name\">AP name:</label>\r\n";
  s += "<input type=\"text\" name=\"wifi-ap-name\" id=\"wifi-ap-name\" value=\"" + (String)wmConfig.apSsid + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"wifi-ap-pass\">AP Pass:</label>\r\n";
  s += "<input type=\"text\" name=\"wifi-ap-pass\" id=\"wifi-ap-pass\" value=\"" + (String)wmConfig.apPassword + "\" maxlength=\"15\" placeholder=\"8 characters minimum\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"wifi-ap-name\">STA name:</label>\r\n";
  s += "<input type=\"text\" name=\"wifi-sta-name\" id=\"wifi-sta-name\" value=\"" + (String)wmConfig.staSsid + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"wifi-ap-pass\">STA Pass:</label>\r\n";
  s += "<input type=\"text\" name=\"wifi-sta-pass\" id=\"wifi-sta-pass\" value=\"" + (String)wmConfig.staPassword + "\" maxlength=\"15\" placeholder=\"8 characters minimum\">\r\n";
  s += "</div>\r\n";
  s += "</p>\r\n";
  s += "</td>\r\n";
  s += "</tr>\r\n";
  s += "</table>\r\n";
  s += "<table class=\"block\">\r\n";
  s += "<tr>\r\n";
  s += "<th style=\"background-color: #999\"><p>MQTT settings:</p></th>\r\n";
  s += "</tr>\r\n";
  s += "<tr>\r\n";
  s += "<td>\r\n";
  s += "<p>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"mqtt-user\">User:</label>\r\n";
  s += "<input type=\"text\" name=\"mqtt-user\" id=\"mqtt-user\" value=\"" + (String)wmConfig.mqttUser + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"mqtt-pass\">Password:</label>\r\n";
  s += "<input type=\"text\" name=\"mqtt-pass\" id=\"mqtt-pass\" value=\"" + (String)wmConfig.mqttPassword + "\" maxlength=\"15\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"mqtt-broker\">Broker:</label>\r\n";
  s += "<input type=\"text\" name=\"mqtt-broker\" id=\"mqtt-broker\" value=\"" + (String)wmConfig.mqttBroker + "\" maxlength=\"31\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"mqtt-topic\">Topic:</label>\r\n";
  s += "<input type=\"text\" name=\"mqtt-topic\" id=\"mqtt-topic\" value=\"" + (String)wmConfig.mqttTopic + "\" maxlength=\"63\">\r\n";
  s += "</div>\r\n";
  s += "</p>\r\n";
  s += "</td>\r\n";
  s += "</tr>\r\n";
  s += "</table>\r\n";
  s += "<table class=\"block\">\r\n";
  s += "<tr>\r\n";
  s += "<th style=\"background-color: #999\"><p>NTP settings:</p></th>\r\n";
  s += "</tr>\r\n";
  s += "<tr>\r\n";
  s += "<td>\r\n";
  s += "<p>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"serv-ntp\">NTP server:</label>\r\n";
  s += "<input type=\"text\" name=\"serv-ntp\" id=\"serv-ntp\" value=\""; 
  s += wmConfig.ntpServerName;
  s += "\" maxlength=\"31\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"gmt-zone\">GMT zone:</label>\r\n";
  s += "<input type=\"number\" name=\"gmt-zone\" id=\"gmt-zone\" value=\"";
  s += wmConfig.timeZone;
  s += "\" maxlength=\"2\">\r\n";
  s += "</div>\r\n";
  s += "</p>\r\n";
  s += "</td>\r\n";
  s += "</tr>\r\n";
  s += "</table>\r\n";
  s += "<table class=\"block\">\r\n";
  s += "<tr>\r\n";
  s += "<th style=\"background-color: #999\"><p>WaterMeter module settings:</p></th>\r\n";
  s += "</tr>\r\n";
  s += "<tr>\r\n";
  s += "<td>\r\n";
  s += "<p>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"hotw\">Hot liters:</label>\r\n";
  s += "<input type=\"number\" name=\"hotw\" id=\"hotw\" value=\"";
  s += wmConfig.hotWater;
  s += "\" max=\"4294967295\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"coldw\">Cold liters:</label>\r\n";
  s += "<input type=\"number\" name=\"coldw\" id=\"coldw\" value=\"";
  s += wmConfig.coldWater;
  s += "\" max=\"4294967295\">\r\n";
  s += "</div>\r\n";
  s += "<div class=\"input-w\">\r\n";
  s += "<label for=\"countw\">Liters in pulse:</label>\r\n";
  s += "<input type=\"number\" name=\"countw\" id=\"countw\" value=\"";
  s += wmConfig.litersPerPulse;
  s += "\" min=\"1\" max=\"100\">\r\n";
  s += "</div>\r\n";
  s += "</p>\r\n";
  s += "<p>\r\n";
  s += "<input type=\"checkbox\" name=\"reboot\" id=\"reboot\" value=\"true\">\r\n";
  s += "<label for=\"reboot\">Reboot</label>\r\n";
  s += "<input type=\"checkbox\" name=\"defconfig\" id=\"defconfig\" value=\"true\">\r\n";
  s += "<label for=\"defconfig\">Default setting (New start)</label>\r\n";
  s += "</p>\r\n";
  s += "<p>\r\n";
  s += "<button class=\"set-conf\" name=\"set-conf\" value=\"set\">\r\n";
  s += "Set config\r\n";
  s += "</button></p>\r\n";
  s += "<p><a href=\"";
  s += httpRoot;
  s += "\">Main</a>\r\n";
/*  s += "<a href=\"time\">Set Time</a>\r\n";*/
  s += "<a href=\"upload\">Update Firmware</a></p>\r\n";
  s += "</p>\r\n";
  s += "</td>\r\n";
  s += "</tr>\r\n";
  s += "</table>\r\n";
  s += "</div>\r\n";
  s += "</form>\r\n";
  s += "</div><br><br>\r\n";


  s += bodyEnd();
  s += htmlEnd();

  webServer.send(200, "text/html", s);
}


void parssingSettings() {
  _config config;
  String s = "";
  bool newSave = false;

  clearConfig(&config);
  
  for (unsigned int i=0; i<webServer.args(); i++) {
    s = webServer.argName(i);
    if (s == "login") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.webAdminLogin, s.length()+1);
    } else if (s == "password") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.webAdminPassword, s.length()+1);
    } else if (s == "fullsecurity") {
      s = webServer.arg(i);
      if (s == "true") config.fullSecurity = true;
    } else if (s == "configsecurity") {
      s = webServer.arg(i);
      if (s == "true") config.configSecurity = true;
    } else if (s == "wifi-mode") {
      s = webServer.arg(i);
      if (s == "station") config.apMode = false;
      if (s ==  "ap") config.apMode = true;
    } else if (s == "wifi-ap-name") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.apSsid, s.length()+1);
    } else if (s == "wifi-ap-pass") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.apPassword, s.length()+1);
    } else if (s == "wifi-sta-name") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.staSsid, s.length()+1);
    } else if (s == "wifi-sta-pass") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.staPassword, s.length()+1);
    } else if (s == "mqtt-user") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.mqttUser, s.length()+1);
    } else if (s == "mqtt-pass") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.mqttPassword, s.length()+1);
    } else if (s == "mqtt-broker") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.mqttBroker, s.length()+1);
    } else if (s == "mqtt-topic") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.mqttTopic, s.length()+1);
    } else if (s == "serv-ntp") {
      s = webServer.arg(i);
      s.trim();
      s += '\0';
      s.toCharArray(config.ntpServerName, s.length()+1);
    } else if (s == "gmt-zone") {
      s = webServer.arg(i);
      config.timeZone = s.toInt();
    } else if (s == "hotw") {
      config.hotTime = localTimeT();
      s = webServer.arg(i);
      config.hotWater = strtoul(s.c_str(), 0, 10);
      config.hotTime = localTimeT();
    } else if (s == "coldw") {
      config.coldTime = localTimeT();
      s = webServer.arg(i);
      config.coldWater = strtoul(s.c_str(), 0, 10);
      config.coldTime = localTimeT();
    } else if (s == "countw") {
      s = webServer.arg(i);
      config.litersPerPulse = s.toInt();
    } else if (s == "reboot") {
      rebootNow = true;
    } else if (s == "defconfig") {
      defaultConfig = true;
    } else if (s == "set-conf") {
      if (webServer.arg(i) == "set") {
        saveNewConfig = true;
      }
    }
  }

  if (defaultConfig) {
    defaultConfig = false;
    if (sdOk) rmDirR(watermeterDirName);
    clearEeprom();
    if (rebootNow) {
      Serial.println("Rebooting ...");
      delay(2000);
      ESP.reset();
    }
    delay(1000);
    
    firstStart = true;
    initDefConfig(&wmConfig);

    
    if (sdOk) {
        mkDir(watermeterDirName);
    }
    
    startWiFiAP();
    delay(1000);
    startApMsg();
    initWebServer();

    return;
  }

  if (saveNewConfig) {

    if (strcmp(wmConfig.webAdminLogin, config.webAdminLogin) != 0 || strcmp(wmConfig.webAdminPassword, config.webAdminPassword) != 0 ||
               wmConfig.fullSecurity != config.fullSecurity || wmConfig.configSecurity != config.configSecurity) newSave = true;

    if (apModeNow) {
      if (!config.apMode || strcmp(wmConfig.apSsid, config.apSsid) != 0 || strcmp(wmConfig.apPassword, config.apPassword) != 0)  {
        restartWiFi = true;
        newSave = true;
      }
    }

    if (staModeNow) {
      if (config.apMode || strcmp(wmConfig.staSsid, config.staSsid) != 0 || strcmp(wmConfig.staPassword, config.staPassword) != 0) {
        restartWiFi = true;
        staConfigure = true;
        newSave = true;
      }
    }

    if (strcmp(wmConfig.ntpServerName, config.ntpServerName) != 0 || wmConfig.timeZone != config.timeZone) {
      responseNTP = false;
      newSave = true;
    }

    if (strcmp(wmConfig.mqttBroker, config.mqttBroker) != 0 || strcmp(wmConfig.mqttUser, config.mqttUser) != 0 ||
        strcmp(wmConfig.mqttPassword, config.mqttPassword) != 0 || strcmp(wmConfig.mqttTopic, config.mqttTopic) != 0) {
      mqttRestart = true;
      newSave = true;
    }
       
    if (wmConfig.hotWater != config.hotWater) {
      subsHotWater = true;
      newSave = true;
    }
    if (wmConfig.coldWater != config.coldWater) {
      subsColdWater = true;
      newSave = true;
    }

    if (newSave) {
      setConfig(&config);
      saveConfig();
      firstStart = false;
    }

    if (restartWiFi) {
      if (wmConfig.apMode || wmConfig.staSsid[0] == '0') {
        staConfigure = false;
        startWiFiAP();
        delay(1000);
        startApMsg();
      } else {
        if (!startWiFiSTA()) {
          Serial.println("No WiFi connect STA mode. Start AP mode");
          delay(500);
          startWiFiAP();
/*          wmConfig.apMode = true;
          saveData();
          ESP.reset();*/
        } else mqttReconnect();

      }
    }

    if (mqttRestart) mqttReconnect();
    
    saveNewConfig = false;
  }
  

  if (rebootNow) {
    Serial.println("Rebooting ...");
    delay(2000);
    ESP.reset();
  }
}

void handleNotFound(){
  
  if (webServer.uri() == "/settings") {
    parssingSettings();
    return;
  } 
  
  String message = "\nURI: ";
  message += webServer.uri();
  message += "\nMethod: ";
  message += (webServer.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += webServer.args();
  message += "\n";
  for (uint8_t i=0; i<webServer.args(); i++){
    message += " NAME:"+webServer.argName(i) + "\n VALUE:" + webServer.arg(i) + "\n";
  }
  webServer.send(404, "text/plain", message);
  if (DEBUG) {
    Serial.print(message);
  }
} 




