#include "CommandStation.h"

byte lcdLineChar[8] = {
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001,
  B00001
};

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, LCD_COLS, LCD_ROWS);
LiquidCrystal_I2CAdapter lcdAdapter(&lcd);
CharacterDisplayRenderer renderer(&lcdAdapter, LCD_COLS, LCD_ROWS);
LcdMenu menu(renderer);
NewEncoder encoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, -20, 20, 0, FULL_PULSE);
I2CKeyPad8x8 keyPad(0x20);
uint32_t lastIndex = 64;

extern MenuScreen *settingsScreen;
MENU_SCREEN(mainScreen, mainItems,
  ITEM_BASIC("Start service"),
  ITEM_BASIC("Connect to WiFi"),
  ITEM_SUBMENU("Settings", settingsScreen),
  ITEM_BASIC("Blink SOS"),
  ITEM_BASIC("Blink random"));
MENU_SCREEN(settingsScreen, settingsItems,
    ITEM_BASIC("Backlight"),
    ITEM_BASIC("Contrast"),
    ITEM_BACK());

CommandStationClass::CommandStationClass() {
}

void CommandStationClass::init() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(2, lcdLineChar);
  lcd.setCursor(0,0);
  lcd.print(CS_NAME);
  delay(1000);

  encoder.begin();

  renderer.begin();
  menu.setScreen(mainScreen);

  _operationMode = OperationMode::Drive;

  //////Ez csak atmeneti a teszteles idejere amig nincs eeprom olvasas/////
  _rightThrottle = addThrottle(3, "M41 2093");
  _rightThrottle->setDriveMode(DriveMode::styleAC);
  _leftThrottle = addThrottle(19, "M61 019");
  _leftThrottle->setDriveMode(DriveMode::styleCab);
  /////////////////////////////////////////////////////////////////////////
}

uint16_t CommandStationClass::getLocoAddress(uint8_t id) {
  LocoClass *loco = getLocoById(id);
  
  if ( loco ) { return loco->getAddress(); }

  return 0;
}

LocoClass *CommandStationClass::addLoco(uint16_t addr, uint8_t id, const char *name) {
  LocoClass *loco = getLocoByAddress(addr);

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
  ThrottleClass *throttle = getThrottleByAddress(addr);

  if (!throttle) { 
    throttle = new ThrottleClass(loco); 
  }
    
  return throttle;
}

const char *CommandStationClass::getLocoName(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if (loco) { return loco->getName(); }
  
  return nullptr;

}

void CommandStationClass::setLocoName(uint16_t addr, const char *name) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if (loco) { return loco->setName(name); }
}

uint8_t CommandStationClass::getLocoStatus(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if (loco) { return loco->getSlotStatus(); }
  
  return 128;
}

void CommandStationClass::setLocoStatus(uint16_t addr, uint8_t status) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if (loco) { loco->setSlotStatus(status); }
}

uint8_t CommandStationClass::getLocoSpeed(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if ( loco ) { return loco->getSpeed(); }

  return 128;
}

void CommandStationClass::setLocoSpeed(uint16_t addr, uint8_t speed, uint8_t src) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if ( loco ) { loco->setSpeed(speed, src); }
}

uint8_t CommandStationClass::getLocoDirection(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if ( loco ) { return loco->getDirection(); }

  return Direction::Forward;
}

void CommandStationClass::setLocoDirection(uint16_t addr, uint8_t dir, uint8_t src) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if ( loco ) { loco->setDirection(dir, src); }
}

uint16_t CommandStationClass::getLocoFunctions(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  
  if ( loco ) { return loco->getFunctions(); }

  return 0;
}

void CommandStationClass::setLocoFunctions(uint16_t addr, uint16_t functions, uint8_t src) {
  LocoClass *loco = getLocoByAddress(addr);

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

void CommandStationClass::inputLoop() {
  NewEncoder::EncoderState currentEncoderState;
  encoder.getState(currentEncoderState);
  uint8_t index = keyPad.getKey();
  
  if (_operationMode == OperationMode::Menu ) { 
    switch ( currentEncoderState.currentClick ) {   //Encoder
      case NewEncoder::DownClick:
       // if (operationMode == OperationMode::Drive ) { _leftThrottle->decDrive(); }
        menu.process(DOWN);
        break;
      case NewEncoder::UpClick:
        // if (operationMode == OperationMode::Drive ) { _leftThrottle->incDrive(); }
        menu.process(UP);
        break;
      case NewEncoder::NoClick:
      break;
    }
  
    if (lastIndex != index ) {
      switch (index)                                //Keyboard
      {
        case 24:  //Menu
          _operationMode = OperationMode::Drive;
          break;

        case 25:  //Enter
          menu.process(ENTER);
          break;
  
        case 31:  //ESC
          menu.process(BACK);
          break;
      
        case 17:  //CLR
          menu.process(BACKSPACE);
          break;
  
        case 62:  //UP
          menu.process(UP);
          break;
  
        case 19:  //DOWN
          menu.process(DOWN);
          break;
  
        case 9:  //RIGHT
          menu.process(RIGHT);
          break;
  
        case 1:  //LEFT
          menu.process(LEFT);
          break;
  
        case 41:  //0
          menu.process('0');
          break;
        
        case 21:  //1
          menu.process('1');
          break;
        
        case 45:  //2
          menu.process('2');
          break;
        
        case 29:  //3
          menu.process('3');
          break;
        
        case 18:  //4
          menu.process('4');
          break;
        
        case 42:  //5
          menu.process('5');
          break;
        
        case 26:  //6
          menu.process('6');
          break;
        
        case 22:  //7
          menu.process('7');
          break;
        
        case 46:  //8
          menu.process('8');
          break;
        
        case 30:  //9
          menu.process('9');
          break;
        
        default:
          break;
      }
    }
  } else if (_operationMode == OperationMode::Drive ) {
    switch ( currentEncoderState.currentClick ) {   //Encoder
      case NewEncoder::DownClick:
        _leftThrottle->decDrive();
        break;
      case NewEncoder::UpClick:
        _leftThrottle->incDrive();
        break;
      case NewEncoder::NoClick:
      break;
    }
    
    if (lastIndex != index ) {
      switch (index)                                //Keyboard
      {
        case 24:  //Menu
          _operationMode = OperationMode::Menu;
          break;
 
        case 3:  //Track Auto
          _leftThrottle->incReverser();
          break;

        case 5:  //Hold
          _leftThrottle->decReverser();
          break;

        case 59:  //Iris+
          _leftThrottle->incDrive();
          break;

        case 61:  //Iris-
          _leftThrottle->decDrive();
          break;

        case 11:  //FocusF
          _rightThrottle->incReverser();
          break;

        case 13:  //FocusN
          _rightThrottle->decReverser();
          break;

        case 51:  //ZoomIn
          _rightThrottle->incDrive();
          break;

        case 53:  //ZoomOut
          _rightThrottle->decDrive();
          break;

        default:
          break;
      }
    }
  }

  lastIndex = index;
}

void CommandStationClass::interfacesLoop() {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if ( s->getSourceStatus() != SRC_NULL ) {
      if ( s->getSourceStatus() != SRC_LOCONET && s->getSlotId() > 0 ) {
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
}

void CommandStationClass::throttleLoop() {
  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    t->check();
  }
}

void CommandStationClass::displayLoop() {
  if ( _operationMode == OperationMode::Drive ) {
    char rev;

    menu.hide();

    lcd.setCursor(0,0);
    if (_leftThrottle) lcd.printf("%-9s%c", _leftThrottle->getName(), byte(2)); 
    else lcd.printf("         %c", byte(2));
    if (_rightThrottle) lcd.printf(" %-9s", _rightThrottle->getName()); 
    else lcd.printf("          ");

    lcd.setCursor(0,1);
    if (_leftThrottle) {
      switch ( _leftThrottle->getReverser() ) {
        case -1:
          rev = char(0x7F);
          break;
        case 0:
          rev = char(0xFD);
          break;
        case 1:
          rev = char(0x7E);
          break;
      }
      if ( _leftThrottle->getDriveMode() == DriveMode::styleAC ) {
        lcd.printf("%c     %3d%c", rev, _leftThrottle->getSpeed(), byte(2));
      }
      if ( _leftThrottle->getDriveMode() == DriveMode::styleCab ) {
        if ( _leftThrottle->getBrake() == 0 ) {
          lcd.printf("%cT%-3d %3d%c", rev, _leftThrottle->getTargetSpeed(), _leftThrottle->getSpeed(), byte(2));
        } else {
          lcd.printf("%cB%-3d %3d%c", rev, _leftThrottle->getBrake(), _leftThrottle->getSpeed(), byte(2));
        }
      }
    } else lcd.printf("         %c", byte(2));

    if (_rightThrottle) {
      switch ( _rightThrottle->getReverser() ) {
        case -1:
          rev = char(0x7F);
          break;
        case 0:
          rev = char(0xFD);
          break;
        case 1:
          rev = char(0x7E);
          break;
      }
      if ( _rightThrottle->getDriveMode() == DriveMode::styleAC ) {
        lcd.printf(" %c     %3d", rev, _rightThrottle->getSpeed());
      }
      if ( _rightThrottle->getDriveMode() == DriveMode::styleCab ) {
        if ( _rightThrottle->getBrake() == 0 ) {
          lcd.printf(" %cT%-3d %3d", rev, _rightThrottle->getTargetSpeed(), _rightThrottle->getSpeed());
        } else {
          lcd.printf(" %cB%-3d %3d", rev, _rightThrottle->getBrake(), _rightThrottle->getSpeed());
        }
      }
    } else lcd.printf("          ");
  }

  if ( _operationMode == OperationMode::Menu ) {
    menu.show();
  }
}

void CommandStationClass::check() {
  inputLoop();
  throttleLoop();
  interfacesLoop();
  displayLoop();
}

// private methods
LocoClass *CommandStationClass::getLocoByAddress(uint16_t addr) {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if (s->getAddress() == addr) {
      return s;
    }
  }
  return nullptr;
}
LocoClass *CommandStationClass::getLocoById(uint8_t id) {
  for (LocoClass *s = _locos->getFirst(); s; s = s->getNext()) {
    if (s->getSlotId() == id) {
      return s;
    }
  }
  return nullptr;
}

ThrottleClass *CommandStationClass::getThrottleByAddress(uint16_t addr) {
  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    if (t->getAddress() == addr) {
      return t;
    }
  }
  return nullptr;
}

CommandStationClass CommandStation = CommandStationClass();