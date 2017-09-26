bool initSD() {

  if (DEBUG) Serial.println("Initialising SD card ...");

  if (!SD.begin(SD_PIN)) {
    sdOk = false;
    if (DEBUG) Serial.println("SD initialising False.");
  } else {
    sdOk = true;
    if (DEBUG) Serial.println("SD initialising OK.");
  }
  return sdOk;
}


bool readConfig() {
//  return false;
  if (sdOk) {
    if (DEBUG) Serial.printf("Read from SD. File name: %s\n", configFileName.c_str());
    /* read from SD */
    File file = SD.open((char*)configFileName.c_str());
    if (file) {
      file.seek(0);
      byte *p = (byte*)&wmConfig;
      file.read(p, sizeof(_config));
      file.close();
      if (wmConfig.staSsid[0] != 0) staConfigure = true;
      return true;
    } else { 
      if (DEBUG) Serial.printf("readConfigFile() - can't read from config file: %s\n", configFileName.c_str()); 
      return false;
    }
  } else {
    /* read from EEPROM */
    if (NOT_READ_EPPROM) return false;
    if (readEeprom()) {
      if (DEBUG) Serial.println("Read from EEPROM true");
      if (wmConfig.staSsid[0] != 0) staConfigure = true;
      return true;
    } else {
      if (DEBUG) Serial.println("Read from EEPROM false");
      return false;
    }
  }
}

void saveConfig() {
  if (sdOk) {
    if (DEBUG) Serial.printf("Save to SD. File name: %s\n", configFileName.c_str());
    File file = SD.open(configFileName.c_str(), FILE_WRITE);
    if (file) {
      file.seek(0);
      byte *p = (byte*)&wmConfig;
      file.write(p, sizeof(_config));
      file.close();
    } else if (DEBUG) Serial.printf("saveConfigFile() - can\'t write to config file: %s\n", configFileName.c_str());
  } else {
    if (DEBUG) Serial.println("Save to EEPROM" );
    writeEeprom();
  }
}


void rmConfigFile() {
  if (!sdOk) return;
  SD.remove((char*)configFileName.c_str());
  if (DEBUG) Serial.printf("Deleted config file: %s\n", configFileName.c_str());
}

void rm(File dir, String tempPath) {
  while(true) {
    File entry =  dir.openNextFile();
    String localPath;

    if (entry) {
      if ( entry.isDirectory() ) {
        if (tempPath == DELIM) localPath = tempPath + entry.name();
        else localPath = tempPath + DELIM + entry.name();
        rm(entry, localPath);


        if( SD.rmdir((char*)localPath.c_str()) ) {
          if (DEBUG) Serial.printf("Deleted dyrectory: %s\n", localPath.c_str());
        } else {
          if (DEBUG) Serial.printf("Unable to delete directory: %s\n", localPath.c_str());
        }
      } else {
        if (tempPath == DELIM) localPath = tempPath + entry.name();
        else localPath = tempPath + DELIM + entry.name();
        if( SD.remove((char*)localPath.c_str()) ) {
          if (DEBUG) Serial.printf("Deleted: %s\n", localPath.c_str());
        } else {
          if (DEBUG) Serial.printf("Failed to delete: %s\n", localPath.c_str());
        }
      }
    } else {
      // break out of recursion
      break;
    }
  }
}

void rmDirR(String dir) {
  File file = SD.open((char*)dir.c_str());
  delay(2000);
  if (file) {
    rm(file, dir);
    file.close();
    if (SD.rmdir((char*)dir.c_str())) if (DEBUG) {
      Serial.printf("Deleted dyrectory: %s\n", dir.c_str());
    } else if (DEBUG) {
      Serial.printf("Unable to delete dyrectory: %s\n", dir.c_str());
    }
  }
}


bool checkDir(String dir) {
  if (!SD.exists((char*)dir.c_str())) {
    if (DEBUG) Serial.printf("Directiry: %s not found\n", dir.c_str());
    return false;
  }
  File Dir = SD.open((char*)dir.c_str());
  if(!Dir) {
    if (DEBUG) Serial.println("SD.open return false");
    return false;
  }
  if (!Dir.isDirectory()) {
    if (DEBUG) Serial.printf("Name: %s non directory", dir.c_str());
    Dir.close();
    return false;
  }
  Dir.close();
  return true;
}

bool mkDir(String dir) {
  if (!sdOk) return false;
  if (!checkDir(dir)) {
    return SD.mkdir((char*)dir.c_str());
  } 
  return true;
}

bool existsFile(String file) {
  if (!sdOk) return false;
  return SD.exists((char*)file.c_str());
}


