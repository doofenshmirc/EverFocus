#include "CommandStation.h"

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, LCD_COLS, LCD_ROWS);
LcdMenu menu(renderer);
SimpleRotary encoder(ROTARY_A, ROTARY_B, ROTARY_S);
SimpleRotaryAdapter rotaryInput(&menu, &encoder);

CommandStationClass::CommandStationClass() {
}

void CommandStationClass::init() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(CS_NAME);

  //////Ez csak atmeneti a teszteles idejere amig nincs eeprom olvasas/////
  _leftThrottle = addThrottle(19, "M61 019");
  /////////////////////////////////////////////////////////////////////////
}

uint16_t CommandStationClass::getLocoAddress(uint8_t id) {
  LocoClass *loco = _getLocoById(id);
  
  if ( loco ) { return loco->getAddress(); }

  return 0;
}

LocoClass *CommandStationClass::addLoco(uint16_t addr, uint8_t id, const char *name) {
  LocoClass *loco = _getLocoByAddress(addr);

  if (!loco) { 
    loco = new LocoClass(addr, id, name); 
  }

  if (loco) { 
    if ( loco->getSlotId() == 0 && id == 0 ) LocoNetInterface.send(OPC_LOCO_ADR, (addr & 0xFF80) >> 7, addr & 0x7F ); 
    else if ( loco->getSlotId() != id && id != 0 ) loco->setSlotId(id); 
  }

  return loco;
}

  

ThrottleClass *CommandStationClass::addThrottle(uint16_t addr, const char *name) {
  LocoClass *loco = addLoco(addr, 0, name);
  ThrottleClass *throttle = _getThrottleByAddress(addr);

  if (!throttle) { 
    throttle = new ThrottleClass(loco); 
  }
    
  return throttle;
}

const char *CommandStationClass::getLocoName(uint16_t addr) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if (loco) { return loco->getName(); }
  
  return nullptr;

}

void CommandStationClass::setLocoName(uint16_t addr, const char *name) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if (loco) { return loco->setName(name); }
}

uint8_t CommandStationClass::getLocoStatus(uint16_t addr) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if (loco) { return loco->getSlotStatus(); }
  
  return 128;
}

void CommandStationClass::setLocoStatus(uint16_t addr, uint8_t status) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if (loco) { loco->setSlotStatus(status); }
}

uint8_t CommandStationClass::getLocoSpeed(uint16_t addr) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if ( loco ) { return loco->getSpeed(); }

  return 128;
}

void CommandStationClass::setLocoSpeed(uint16_t addr, uint8_t speed, uint8_t src) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if ( loco ) { loco->setSpeed(speed, src); }
}

uint8_t CommandStationClass::getLocoDirection(uint16_t addr) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if ( loco ) { return loco->getDirection(); }

  return Direction::Forward;
}

void CommandStationClass::setLocoDirection(uint16_t addr, uint8_t dir, uint8_t src) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if ( loco ) { loco->setDirection(dir, src); }
}

uint16_t CommandStationClass::getLocoFunctions(uint16_t addr) {
  LocoClass *loco = _getLocoByAddress(addr);
  
  if ( loco ) { return loco->getFunctions(); }

  return 0;
}

void CommandStationClass::setLocoFunctions(uint16_t addr, uint16_t functions, uint8_t src) {
  LocoClass *loco = _getLocoByAddress(addr);

  if ( loco ) { loco->setFunctions(functions, src); }
}

void CommandStationClass::setSensor(uint16_t addr, uint8_t state, uint8_t src) {
  if ( src != SRC_LOCONET ) LocoNetInterface.setSensor(addr, state);
}

void CommandStationClass::setSwitch(uint16_t addr, uint8_t out, uint8_t dir, uint8_t src) {
  if ( src != SRC_DCCEX ) DCCEXInterface.setSwitch(addr, out, dir);      
  if ( src != SRC_LOCONET ) LocoNetInterface.setSwitch(addr, out, dir);
}

void CommandStationClass::setPower(uint8_t power, uint8_t src) {
  if (_power != power) {
    _power = power;
    if ( src != SRC_LOCONET ) LocoNetInterface.setPower(power);
    if ( src != SRC_DCCEX ) DCCEXInterface.setPower(power);
    if ( src != SRC_XPRESSNET ) XpressNetInterface.setPower(power);
  }
}

void CommandStationClass::EmergencyStop(uint8_t src) {
  if ( src != SRC_DCCEX ) DCCEXInterface.EmergencyStop();
  if ( src != SRC_LOCONET ) LocoNetInterface.EmergencyStop();
  if ( src != SRC_XPRESSNET ) XpressNetInterface.EmergencyStop();
}

void CommandStationClass::check() {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if ( s->getSourceStatus() != SRC_NULL ) {
      if ( s->getSourceStatus() != SRC_LOCONET ) {
        if ( s->getSpeedStatus() ) LocoNetInterface.setSpeed(s->getSlotId(), s->getSpeed());
        if ( s->getDirectionStatus() || (s->getFunctionsStatus() & 0x1F)) LocoNetInterface.setDirFunct0to4(s->getSlotId(), s->getDirection(), s->getFunctions());
        if ( s->getFunctionsStatus() & 0x1E0) LocoNetInterface.setFunct5to8(s->getSlotId(), s->getFunctions());
      }
    
      if ( s->getSourceStatus() != SRC_DCCEX ) {
        if ( s->getSpeedStatus() || s->getDirectionStatus() ) DCCEXInterface.setThrottle(s->getAddress(), s->getSpeed(), s->getDirection());
        if ( s->getFunctionsStatus() ) DCCEXInterface.setFunctions(s->getAddress(), s->getFunctions(), s->getFunctionsStatus());
      }

      if ( s->getSourceStatus() != SRC_XPRESSNET ) {
//        if ( s->getSpeedStatus() || s->getDirectionStatus() ) DCCEXInterface.setThrottle(s->getAddress(), s->getSpeed(), s->getDirection());
//        if ( s->getFunctionsStatus() ) DCCEXInterface.setFunctions(s->getAddress(), s->getFunctions(), s->getFunctionsStatus());
//        void setSpeed(uint16_t Adr, uint8_t Steps, uint8_t Speed);
      }
      
      s->setSpeedStatus(SRC_NULL);
      s->setDirectionStatus(SRC_NULL);
      s->setFunctionsStatus(SRC_NULL);
    }
  }
  
  byte i;

  // 0 = not turning, 1 = CW, 2 = CCW
  i = encoder.rotate();
  lcd.setCursor(0,0);

  if ( i == 1 ) {
    lcd.print("CW");
    _leftThrottle->incReverser();
  }

  if ( i == 2 ) {
    lcd.print("CCW");
    _leftThrottle->decReverser();
  }
  
  lcd.setCursor(0,1);
  lcd.print(_leftThrottle->getReverser());
  lcd.print("     ");
}

// private methods
LocoClass *CommandStationClass::_getLocoByAddress(uint16_t addr) {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if (s->getAddress() == addr) {
      return s;
    }
  }
  return nullptr;
}
LocoClass *CommandStationClass::_getLocoById(uint8_t id) {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if (s->getSlotId() == id) {
      return s;
    }
  }
  return nullptr;
}

ThrottleClass *CommandStationClass::_getThrottleByAddress(uint16_t addr) {
  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    if (t->getAddress() == addr) {
      return t;
    }
  }
  return nullptr;
}

CommandStationClass CommandStation = CommandStationClass();