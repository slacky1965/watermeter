bool readEeprom() {

  int i;
  uint32_t datacrc;
  byte eBuf[sizeof(_config)];
  

  EEPROM.begin(sizeof(_config)+sizeof(memcrc));


  for (i = EEPROM_START; i < EEPROM_START+sizeof(_config); i++) {
    eBuf[i] = EEPROM.read(i);
  }

  p_memcrc[0] = EEPROM.read(i++);
  p_memcrc[1] = EEPROM.read(i++);
  p_memcrc[2] = EEPROM.read(i++);
  p_memcrc[3] = EEPROM.read(i++);

  datacrc = crc_byte(eBuf, sizeof(_config));

  if (memcrc == datacrc) {
    memcpy(&wmConfig, eBuf, sizeof(_config));
    return true;
  }

  return false;
}

void writeEeprom() {
  int i;
  byte eBuf[sizeof(_config)];

  EEPROM.begin(sizeof(_config)+sizeof(memcrc));

  memcpy(eBuf, &wmConfig, sizeof(_config));

  for (i = EEPROM_START; i < EEPROM_START+sizeof(_config); i++) {
    EEPROM.write(i, eBuf[i]);
  }
  memcrc = crc_byte(eBuf, sizeof(_config));

  EEPROM.write(i++, p_memcrc[0]);
  EEPROM.write(i++, p_memcrc[1]);
  EEPROM.write(i++, p_memcrc[2]);
  EEPROM.write(i++, p_memcrc[3]);


  EEPROM.commit();
}

void clearEeprom() {
  EEPROM.begin(sizeof(_config)+sizeof(memcrc));

  for (int i = EEPROM_START; i < EEPROM_START+sizeof(_config)+sizeof(memcrc); i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.end();
}

