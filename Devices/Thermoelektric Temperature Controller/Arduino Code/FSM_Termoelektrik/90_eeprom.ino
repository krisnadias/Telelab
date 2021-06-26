// EEPROM =======================================
// Menyimpan data
#include <EEPROM.h>

// simpan/muat setting ke EPROM
#define ROM_MARK 0xEE

boolean loadROM() {
  int addr=0;
  byte mark = EEPROM.read(addr);
  if (mark != ROM_MARK) return false;
  return true;
}

void resetROM() {
  EEPROM.write(0, 0xFF);  
}

void saveROM() {
  int addr=0;
  EEPROM.write(addr++, ROM_MARK);
  //EEPROM.commit();
}

void resetSettings() {
}
