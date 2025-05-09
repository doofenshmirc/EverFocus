#include "CommandStation.h"

#define VERSION "v1.0.0"

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
NewEncoder encoder(ENCODER_CLK_PIN, ENCODER_DT_PIN, ENCODER_SW1_PIN, ENCODER_SW2_PIN, ENCODER_SW3_PIN, ENCODER_SWD_PIN, -20, 20, 0, FULL_PULSE);
I2CKeyPad8x8 keyPad(0x20);
uint32_t lastIndex = 64;

//###########   MENU  ##############################
extern MenuScreen *locoManagerMenuScreen;
extern MenuScreen *locoMenuScreen;
extern MenuScreen *locoInputMenuScreen;
extern MenuScreen *throttleManagerMenuScreen;
extern MenuScreen *throttleMenuScreen;
extern MenuScreen *throttleInputMenuScreen;
extern MenuScreen *trackManagerMenuScreen;
char* charInput1 = new char[10];
char* charInput2 = new char[10];
uint8_t uint8Input1 = 0;
uint8_t uint8Input2 = 0;
uint8_t uint8Input3 = 0;
std::vector<const char*> trackType = {"MAIN", "PROG", "DC", "DCX", "NONE"};
std::vector<const char*> locoList;
std::vector<const char*> driveMode = {"AC", "DC", "CAB"};
std::vector<const char*> buttonList = {"Display", "Rec", "Select", "Stop", "Mode", "Play", "Zoom", "Pause", "Seq", "Copy", "Search", "Call", "1", "4", "2", "5", "3", "6", "7", "Clr", "8", "0", "9", "Enter" "DRV", "CAM" };
std::map<uint8_t, uint8_t> buttonCode = { {0, 7}, {1, 0}, {2, 63}, {3, 56}, {4, 15}, {5, 8}, {6, 55}, {7, 48}, {8, 23}, {9, 16}, {10, 47}, {11, 40}, {12, 21}, {13, 18}, {14, 45}, {15, 42}, {16, 29}, {17, 26}, {18, 22}, {19, 17}, {20, 46}, {21, 41}, {22, 30}, {23, 25}, {24, 2}, {25, 10} };
std::map<uint8_t, uint8_t> buttonIndex = { {7, 0}, {0, 1}, {63, 2}, {56, 3}, {15, 4}, {8, 5}, {55, 6}, {48, 7}, {23, 8}, {16, 9}, {47, 10}, {40, 11}, {21, 12}, {18, 13}, {45, 14}, {42, 15}, {29, 16}, {26, 17}, {22, 18}, {17, 19}, {46, 20}, {41, 21}, {30, 22}, {25, 23}, {2, 24}, {10, 25} };

MENU_SCREEN(mainMenuScreen, mainMenuItems,
  ITEM_SUBMENU("Locomotive manager", locoManagerMenuScreen),
  ITEM_SUBMENU("Throttle manager", throttleManagerMenuScreen),
  ITEM_SUBMENU("Track manager", trackManagerMenuScreen),
  ITEM_COMMAND("Store config data", []() { /* Fejlesztendo */ } )
);

MENU_SCREEN(locoManagerMenuScreen, locoManagerMenuItems,
  ITEM_COMMAND("Locomotives", []() { CommandStation.updateLocoMenu(); menu.setScreen(locoMenuScreen);} ),
  ITEM_COMMAND("Add locomotive", []() { CommandStation.updateLocoInputMenu(0); menu.setScreen(locoInputMenuScreen); menu.reset(); } ),
  ITEM_COMMAND("Import from DCC-EX", []() { DCCEXInterface.requestRosterList(); } ),
  ITEM_BACK()
);

MENU_SCREEN(throttleManagerMenuScreen, throttleManagerMenuItems,
  ITEM_COMMAND("Throttles", []() { CommandStation.updateThrottleMenu(); menu.setScreen(throttleMenuScreen);} ),
  ITEM_COMMAND("Add throttle", []() { CommandStation.updateThrottleInputMenu(0); menu.setScreen(throttleInputMenuScreen); menu.reset(); } ),
  ITEM_BACK()
);

MENU_SCREEN(locoMenuScreen, locoMenuItems,
  ITEM_BACK()
);

MENU_SCREEN(locoInputMenuScreen, locoInputMenuItems,
  ITEM_BACK()
);

MENU_SCREEN(throttleMenuScreen, throttleMenuItems,
  ITEM_BACK()
);

MENU_SCREEN(throttleInputMenuScreen, throttleInputMenuItems,
  ITEM_BACK()
);

void rangeCallbackA(const Ref<int>& addr) { if (DCCEXInterface.trackAType != 2 && DCCEXInterface.trackAType != 3) DCCEXInterface.trackAAddr = 0; }
void rangeCallbackB(const Ref<int>& addr) { if (DCCEXInterface.trackBType != 2 && DCCEXInterface.trackBType != 3) DCCEXInterface.trackBAddr = 0; }
void rangeCallbackC(const Ref<int>& addr) { if (DCCEXInterface.trackCType != 2 && DCCEXInterface.trackCType != 3) DCCEXInterface.trackCAddr = 0; }
void rangeCallbackD(const Ref<int>& addr) { if (DCCEXInterface.trackDType != 2 && DCCEXInterface.trackDType != 3) DCCEXInterface.trackDAddr = 0; }
MENU_SCREEN(trackManagerMenuScreen, trackManagerMenuItems,
  ITEM_WIDGET(
    "Track(A)",
    [](const Ref<uint8_t> trackType, Ref<int> addr) { /*Serial.printf("Type:%s Addr:%d\n", trackTypes[trackType], static_cast<int>(addr));*/ },
    WIDGET_LIST_REF(trackType, DCCEXInterface.trackAType, "%s", 0, true, [](const Ref<uint8_t>& trackType) { if (DCCEXInterface.trackAType != 2 && DCCEXInterface.trackAType != 3) DCCEXInterface.trackAAddr = 0; } ),
    WIDGET_RANGE_REF(DCCEXInterface.trackAAddr, 1, 0, 9999, " [%d]", 0, true, rangeCallbackA )
  ),
  ITEM_WIDGET(
    "Track(B)",
    [](const Ref<uint8_t> trackType, Ref<int> addr) { /*Serial.printf("Type:%s Addr:%d\n", trackTypes[trackType], static_cast<int>(addr));*/ },
    WIDGET_LIST_REF(trackType, DCCEXInterface.trackBType, "%s", 0, true, [](const Ref<uint8_t>& trackType) { if (DCCEXInterface.trackBType != 2 && DCCEXInterface.trackBType != 3) DCCEXInterface.trackBAddr = 0; } ),
    WIDGET_RANGE_REF(DCCEXInterface.trackBAddr, 1, 0, 9999, " [%d]", 0, true, rangeCallbackB )
  ),
  ITEM_WIDGET(
    "Track(C)",
    [](const Ref<uint8_t> trackType, Ref<int> addr) { /*Serial.printf("Type:%s Addr:%d\n", trackTypes[trackType], static_cast<int>(addr));*/ },
    WIDGET_LIST_REF(trackType, DCCEXInterface.trackCType, "%s", 0, true, [](const Ref<uint8_t>& trackType) { if (DCCEXInterface.trackCType != 2 && DCCEXInterface.trackCType != 3) DCCEXInterface.trackCAddr = 0; } ),
    WIDGET_RANGE_REF(DCCEXInterface.trackCAddr, 1, 0, 9999, " [%d]", 0, true, rangeCallbackC )
  ),
  ITEM_WIDGET(
    "Track(D)",
    [](const Ref<uint8_t> trackType, Ref<int> addr) { /*Serial.printf("Type:%s Addr:%d\n", trackTypes[trackType], static_cast<int>(addr));*/ },
    WIDGET_LIST_REF(trackType, DCCEXInterface.trackDType, "%s", 0, true, [](const Ref<uint8_t>& trackType) { if (DCCEXInterface.trackDType != 2 && DCCEXInterface.trackDType != 3) DCCEXInterface.trackDAddr = 0; } ),
    WIDGET_RANGE_REF(DCCEXInterface.trackDAddr, 1, 0, 9999, " [%d]", 0, true, rangeCallbackD )
  ),
  ITEM_COMMAND("Get from DCC-EX", []() { DCCEXInterface.requestTracksType(); } ),
  ITEM_COMMAND("Set to DCC-EX", []() { DCCEXInterface.setTracksType(); } )
);

CommandStationClass::CommandStationClass() {
}

void CommandStationClass::init() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(2, lcdLineChar);
  lcd.setCursor(0, 0);
  lcd.printf("  %s %s", CS_NAME, VERSION);
  delay(1000);
  
  encoder.begin();
  renderer.begin();
  menu.setScreen(mainMenuScreen);

  //////Ez csak atmeneti a teszteles idejere amig nincs eeprom olvasas/////
  addLoco(2093, "M41 002");
  addLoco(19, "M61 019");
  addLoco(219, "M62 219");
  addLoco(1119, "V200 1119");
  addThrottle(2093, DriveMode::drvAC, 7);
  addThrottle(19, DriveMode::drvDC, 63);
  addThrottle(219, DriveMode::drvDC, 0);
  addThrottle(1119, DriveMode::drvCAB, 56);
  /////////////////////////////////////////////////////////////////////////
}

void CommandStationClass::updateLocoMenu() {
  locoMenuScreen->clear();
  locoMenuScreen->setParent(locoManagerMenuScreen);

  for (LocoClass *l = _locos->getFirst(); l; l = l->getNext()) {
    char* s = new char[21];
    sprintf(s, "%-9s [%04d]", l->getName(), l->getAddress());
    const char *name = s;
    locoMenuScreen->addItem(ITEM_COMMAND(name, []() { CommandStation.updateLocoInputMenu(convertStringToUint16(substr(locoMenuScreen->getItemAt(locoMenuScreen->getCursor())->getText(), 11, 4))); menu.setScreen(locoInputMenuScreen); menu.reset(); } ));
  }

  locoMenuScreen->addItem(ITEM_BACK());
}

void CommandStationClass::updateLocoInputMenu(uint16_t addr) {
  LocoClass* loco = getLocoByAddress(addr);
  locoInputMenuScreen->clear();
  locoInputMenuScreen->setParent(locoMenuScreen);

  if (loco) {
    sprintf(charInput1, "%d", loco->getAddress());
    sprintf(charInput2, "%s", loco->getName());
  } else {
    strcpy(charInput1, "");
    strcpy(charInput2, "");
  }
  locoInputMenuScreen->addItem(ITEM_INPUT("Addr", charInput1, [](char *addr) { } ));
  locoInputMenuScreen->addItem(ITEM_INPUT_CHARSET("Name", charInput2, "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", [](char *name) { } ));
  locoInputMenuScreen->addItem(ITEM_COMMAND("Add or update", []() { CommandStation.addLoco(convertStringToUint16(charInput1), rtrim(charInput2, ' ')) ; CommandStation.updateLocoMenu(); menu.setScreen(locoMenuScreen); menu.refresh(); } ));
  locoInputMenuScreen->addItem(ITEM_COMMAND("Remove", []() { CommandStation.delLoco(convertStringToUint16(charInput1)) ; CommandStation.updateLocoMenu(); menu.setScreen(locoMenuScreen); menu.reset(); } ));
  locoInputMenuScreen->addItem(ITEM_BACK());
}

void CommandStationClass::updateThrottleMenu() {
  throttleMenuScreen->clear();
  throttleMenuScreen->setParent(throttleManagerMenuScreen);

  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    char* s = new char[21];
    sprintf(s, "%-9s [%04d]", t->getName(), t->getAddress());
    const char *name = s;
    throttleMenuScreen->addItem(ITEM_COMMAND(name, []() { CommandStation.updateThrottleInputMenu(convertStringToUint16(substr(throttleMenuScreen->getItemAt(throttleMenuScreen->getCursor())->getText(), 11, 4))); menu.setScreen(throttleInputMenuScreen); menu.reset(); } ));
  }

  throttleMenuScreen->addItem(ITEM_BACK());
}

void CommandStationClass::updateThrottleInputMenu(uint16_t addr) {
  ThrottleClass* throttle = getThrottleByAddress(addr);

  throttleInputMenuScreen->clear();
  throttleInputMenuScreen->setParent(throttleMenuScreen);

  uint8Input1 = 0;
  uint8Input2 = 0;
  uint8Input3 = 0;

  //for (auto ptr : _locosList) free(const_cast<char*>(ptr));
  locoList.clear();
  
  if (getLocoCount() > 0) {
    uint8_t i = 0;

    for (LocoClass *l = _locos->getFirst(); l; l = l->getNext()) {
      locoList.push_back(l->getName());
      if ( addr == l->getAddress() ) uint8Input1 = i;
      i++;
    }
  }    
  
  if (throttle) {
    uint8Input2 = (uint8_t)throttle->getDriveMode();
    uint8Input3 = buttonIndex[throttle->getButton()];
  }
  
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Loco", locoList, [](const Ref<uint8_t> loco) { }, uint8Input1));
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Mode", driveMode, [](const Ref<uint8_t> drive) { }, uint8Input2));
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Button", buttonList, [](const Ref<uint8_t> button) { }, uint8Input3));
  throttleInputMenuScreen->addItem(ITEM_COMMAND("Add or update", []() { CommandStation.addThrottle(CommandStation.getLocoByName(locoList[uint8Input1])->getAddress(), uint8Input2, buttonCode[uint8Input3]) ; CommandStation.updateThrottleMenu(); menu.setScreen(throttleMenuScreen); menu.refresh(); } ));
  throttleInputMenuScreen->addItem(ITEM_COMMAND("Remove", []() { CommandStation.delThrottle(CommandStation.getLocoByName(locoList[uint8Input1])->getAddress()) ; CommandStation.updateThrottleMenu(); menu.setScreen(throttleMenuScreen); menu.reset(); } ));
  throttleInputMenuScreen->addItem(ITEM_BACK());
}

uint16_t CommandStationClass::getSlotAddress(uint8_t id) {
  SlotClass *slot = getSlotById(id);
  
  if ( slot ) { return slot->getAddress(); }

  return 0;
}

SlotClass *CommandStationClass::addSlot(uint16_t addr, uint8_t id) {
  SlotClass *slot = getSlotByAddress(addr);

  if (!slot) { 
    slot = new SlotClass(addr, id); 
  }

  if (slot) { 
    if ( slot->getId() == 0 && id == 0 ) LocoNetInterface.send(OPC_LOCO_ADR, (addr & 0xFF80) >> 7, addr & 0x7F ); 
    else if ( slot->getId() != id && id != 0 ) slot->setId(id); 
  }

  if (!_slots) _slots = slot;

  return slot;
}

LocoClass *CommandStationClass::addLoco(uint16_t addr, const char* name) {
  LocoClass *loco = getLocoByAddress(addr);
 
  if (!loco) { 
    loco = new LocoClass(addr, name); 
  } else { 
    loco->setName(name); 
  }

  if (!_locos) _locos = loco;

  return loco;
}

void CommandStationClass::delLoco(uint16_t addr) {
  LocoClass *loco = getLocoByAddress(addr);
  ThrottleClass *throttle = getThrottleByAddress(addr);
 
  if (loco && !throttle) delete loco; 
}

ThrottleClass *CommandStationClass::addThrottle(uint16_t addr, uint8_t driveMode, uint8_t button) {
  SlotClass *slot = addSlot(addr, 0);
  LocoClass *loco = getLocoByAddress(addr); 
  ThrottleClass *throttle = getThrottleByAddress(addr);

  if (!throttle) { 
    throttle = new ThrottleClass(slot, loco, driveMode, button); 
  } else {
    throttle->setLoco(loco);
    throttle->setButton(button);
  }

  if (!_throttles) _throttles = throttle;
    
  return throttle;
}

void CommandStationClass::delThrottle(uint16_t addr) {
  ThrottleClass *throttle = getThrottleByAddress(addr);
 
  if (throttle) {
    if (_leftThrottle == throttle) _leftThrottle = nullptr;
    if (_rightThrottle == throttle) _rightThrottle = nullptr;
    delete throttle; 
  }
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

uint8_t CommandStationClass::getSlotStatus(uint16_t addr) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if (slot) { return slot->getStatus(); }
  
  return 128;
}

void CommandStationClass::setSlotStatus(uint16_t addr, uint8_t status) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if (slot) { slot->setStatus(status); }
}

uint8_t CommandStationClass::getSlotSpeed(uint16_t addr) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if ( slot ) { return slot->getSpeed(); }

  return 128;
}

void CommandStationClass::setSlotSpeed(uint16_t addr, uint8_t speed, uint8_t src) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if ( slot ) { slot->setSpeed(speed, src); }
}

uint8_t CommandStationClass::getSlotDirection(uint16_t addr) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if ( slot ) { return slot->getDirection(); }

  return Direction::Forward;
}

void CommandStationClass::setSlotDirection(uint16_t addr, uint8_t dir, uint8_t src) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if ( slot ) { slot->setDirection(dir, src); }
}

uint16_t CommandStationClass::getSlotFunctions(uint16_t addr) {
  SlotClass *slot = getSlotByAddress(addr);
  
  if ( slot ) { return slot->getFunctions(); }

  return 0;
}

void CommandStationClass::setSlotFunctions(uint16_t addr, uint16_t functions, uint8_t src) {
  SlotClass *slot = getSlotByAddress(addr);

  if ( slot ) { slot->setFunctions(functions, src); }
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
  
  if (_operationMode == OperationMode::opMenu ) { 
    switch ( currentEncoderState.currentClick ) {   //Encoder rotary
      case NewEncoder::DownClick:
        menu.process(UP);
        break;
      case NewEncoder::UpClick:
        menu.process(DOWN);
        break;
      case NewEncoder::NoClick:
      break;
    }

    switch ( currentEncoderState.currentButton ) {   //Encoder button
      case NewEncoder::DownButton:
        menu.process(UP);
        break;
      case NewEncoder::UpButton:
        menu.process(DOWN);
        break;
      case NewEncoder::NoButton:
      break;
    }

    if (lastIndex != index ) {
      switch (index)                                //Keyboard
      {
        case BTN_MENU:
          _operationMode = OperationMode::opDrive;
          break;

        case BTN_ENTER:
          menu.process(ENTER);
          break;
  
        case BTN_ESC:
          menu.process(BACK);
          break;
      
        case BTN_CLR:
          menu.process(BACKSPACE);
          break;
  
        case BTN_TOUR:
          menu.process(UP);
          break;
  
        case BTN_SET:
          menu.process(DOWN);
          break;
  
        case BTN_HOME:
          menu.process(RIGHT);
          break;
  
        case BTN_STOP2:
          menu.process(LEFT);
          break;
  
        case BTN_0:
          menu.process('0');
          break;
        
        case BTN_1:
          menu.process('1');
          break;
        
        case BTN_2:
          menu.process('2');
          break;
        
        case BTN_3:
          menu.process('3');
          break;
        
        case BTN_4:
          menu.process('4');
          break;
        
        case BTN_5:
          menu.process('5');
          break;
        
        case BTN_6:
          menu.process('6');
          break;
        
        case BTN_7:
          menu.process('7');
          break;
        
        case BTN_8:
          menu.process('8');
          break;
        
        case BTN_9:
          menu.process('9');
          break;

        case BTN_MON:
          EmergencyStop(SRC_THROTTLE);
          break;

        case BTN_SHIFT:
          break;
        
        default:
          break;
      }
    }
  } else if (_operationMode == OperationMode::opDrive ) {
    switch ( currentEncoderState.currentClick ) {   //Encoder rotary
      case NewEncoder::DownClick:
        if (_leftThrottle) _leftThrottle->decDrive();
        break;
      case NewEncoder::UpClick:
        if (_leftThrottle) _leftThrottle->incDrive();
        break;
      case NewEncoder::NoClick:
      break;
    }

    switch ( currentEncoderState.currentButton ) {   //Encoder button
      case NewEncoder::DownButton:
        if (_leftThrottle) _leftThrottle->decReverser();
        break;
      case NewEncoder::UpButton:
        if (_leftThrottle) _leftThrottle->incReverser();
        break;
      case NewEncoder::NoButton:
      break;
    }
    
    if (lastIndex != index ) {
      switch (index)                                //Keyboard
      {
        case BTN_MENU:
          _operationMode = OperationMode::opMenu;
          break;

        case BTN_DISPLAY:
        case BTN_SELECT:
        case BTN_MODE:
        case BTN_ZOOM:
        case BTN_SEQ:
        case BTN_SEARCH:
        case BTN_1:
        case BTN_2:
        case BTN_3:
        case BTN_7:
        case BTN_8:
        case BTN_9:
        case BTN_DRV:
          _leftThrottle = getThrottleByButton(index);
          break;
        case BTN_FOCUSN:
          if (_leftThrottle) _leftThrottle->incReverser();
          break;
        case BTN_HOLD:
          if (_leftThrottle) _leftThrottle->decReverser();
          break;
        case BTN_IRIS1:
          if (_leftThrottle) _leftThrottle->incDrive();
          break;
        case BTN_IRIS2:
          if (_leftThrottle) _leftThrottle->decDrive();
          break;
        case BTN_AUTO:
          if (_leftThrottle) _leftThrottle->chgFunction(0);
          break;
        case BTN_FOCUSF:
          if (_leftThrottle) _leftThrottle->chgFunction(1);
          break;
        case BTN_ZOOMIN:
          if (_leftThrottle) _leftThrottle->chgFunction(2);
          break;
        case BTN_ZOOMOUT:
          if (_leftThrottle) _leftThrottle->chgFunction(3);
          break;

        case BTN_REC:
        case BTN_STOP:
        case BTN_PLAY:
        case BTN_PAUSE:
        case BTN_COPY:
        case BTN_CALL:
        case BTN_4:
        case BTN_5:
        case BTN_6:
        case BTN_CLR:
        case BTN_0:
        case BTN_ENTER:
        case BTN_CAM:
          _rightThrottle = getThrottleByButton(index);
          break;
        case BTN_HOME:
          if (_rightThrottle) _rightThrottle->incReverser();
          break;
        case BTN_STOP2:
          if (_rightThrottle) _rightThrottle->decReverser();
          break;
        case BTN_TOUR:
          if (_rightThrottle) _rightThrottle->incDrive();
          break;
        case BTN_SET:
          if (_rightThrottle) _rightThrottle->decDrive();
          break;
        case BTN_POSITION:
          if (_rightThrottle) _rightThrottle->chgFunction(0);
          break;
        case BTN_F1:
          if (_rightThrottle) _rightThrottle->chgFunction(1);
          break;
        case BTN_F2:
          if (_rightThrottle) _rightThrottle->chgFunction(2);
          break;
        case BTN_F3:
          if (_rightThrottle) _rightThrottle->chgFunction(3);
          break;
        case BTN_F4:
          if (_rightThrottle) _rightThrottle->chgFunction(4);
          break;
        case BTN_SHIFT:
          _operationMode = OperationMode::opFunction;
          break;

        case BTN_MON:
          EmergencyStop(SRC_THROTTLE);
          break;

        default:
          break;
      }
    }
  } else if (_operationMode == OperationMode::opFunction ) {
    if (lastIndex != index ) {
      switch (index) {
        case BTN_SHIFT:
          _operationMode = OperationMode::opDrive;
          break;
        case BTN_DISPLAY:
          if (_leftThrottle) _leftThrottle->chgFunction(0);
          break;
        case BTN_SELECT:
          if (_leftThrottle) _leftThrottle->chgFunction(1);
          break;
        case BTN_MODE:
          if (_leftThrottle) _leftThrottle->chgFunction(2);
          break;
        case BTN_ZOOM:
          if (_leftThrottle) _leftThrottle->chgFunction(3);
          break;
        case BTN_SEQ:
          if (_leftThrottle) _leftThrottle->chgFunction(4);
          break;
        case BTN_SEARCH:
          if (_leftThrottle) _leftThrottle->chgFunction(5);
          break;
        case BTN_AUTO:
          if (_leftThrottle) _leftThrottle->chgFunction(6);
          break;
        case BTN_IRIS1:
          if (_leftThrottle) _leftThrottle->chgFunction(7);
          break;
        case BTN_FOCUSF:
          if (_leftThrottle) _leftThrottle->chgFunction(8);
          break;
        case BTN_ZOOMIN:
          if (_leftThrottle) _leftThrottle->chgFunction(9);
          break;
        case BTN_REC:
          if (_rightThrottle) _rightThrottle->chgFunction(0);
          break;
        case BTN_STOP:
          if (_rightThrottle) _rightThrottle->chgFunction(1);
          break;
        case BTN_PLAY:
          if (_rightThrottle) _rightThrottle->chgFunction(2);
          break;
        case BTN_PAUSE:
          if (_rightThrottle) _rightThrottle->chgFunction(3);
          break;
        case BTN_COPY:
          if (_rightThrottle) _rightThrottle->chgFunction(4);
          break;
        case BTN_CALL:
          if (_rightThrottle) _rightThrottle->chgFunction(5);
          break;
        case BTN_HOLD:
          if (_rightThrottle) _rightThrottle->chgFunction(6);
          break;
        case BTN_IRIS2:
          if (_rightThrottle) _rightThrottle->chgFunction(7);
          break;
        case BTN_FOCUSN:
          if (_rightThrottle) _rightThrottle->chgFunction(8);
          break;
        case BTN_ZOOMOUT:
          if (_rightThrottle) _rightThrottle->chgFunction(9);
          break;
      }
    }
  }

  lastIndex = index;
}

void CommandStationClass::interfacesLoop() {
  for (SlotClass *s = _slots->getFirst(); s; s = s->getNext()) {
    if ( s->getSourceStatus() != SRC_NULL ) {
      if ( s->getSourceStatus() != SRC_LOCONET && s->getId() > 0 ) {
        if ( s->getSpeedStatus() ) LocoNetInterface.setSpeed(s->getId(), s->getSpeed());
        if ( s->getDirectionStatus() || (s->getFunctionsStatus() & 0x1F)) LocoNetInterface.setDirFunct0to4(s->getId(), s->getDirection(), s->getFunctions());
        if ( s->getFunctionsStatus() & 0x1E0) LocoNetInterface.setFunct5to8(s->getId(), s->getFunctions());
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
  if ( _operationMode == OperationMode::opDrive ) {
    menu.hide();

    lcd.setCursor(0,0);
    if (_leftThrottle) lcd.printf("%-9s%c", _leftThrottle->getName(), byte(2)); 
    else lcd.printf("         %c", byte(2));
    if (_rightThrottle) lcd.printf(" %-9s", _rightThrottle->getName()); 
    else lcd.printf("          ");

    lcd.setCursor(0,1);
    if (_leftThrottle) {
      char rev;
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
      if ( _leftThrottle->getDriveMode() == DriveMode::drvDC ) {
        lcd.printf("%cDC   %3d%c", rev, _leftThrottle->getSpeed(), byte(2));
      }
      if ( _leftThrottle->getDriveMode() == DriveMode::drvAC ) {
        lcd.printf("%cAC   %3d%c", rev, _leftThrottle->getSpeed(), byte(2));
      }
      if ( _leftThrottle->getDriveMode() == DriveMode::drvCAB ) {
        if ( _leftThrottle->getBrake() == 0 ) {
          lcd.printf("%cT%-3d %3d%c", rev, _leftThrottle->getTargetSpeed(), _leftThrottle->getSpeed(), byte(2));
        } else {
          lcd.printf("%cB%-3d %3d%c", rev, _leftThrottle->getBrake(), _leftThrottle->getSpeed(), byte(2));
        }
      }
    } else lcd.printf("         %c", byte(2));
    if (_rightThrottle) {
      char rev;
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
      if ( _rightThrottle->getDriveMode() == DriveMode::drvDC ) {
        lcd.printf(" %cDC   %3d", rev, _rightThrottle->getSpeed());
      }
      if ( _rightThrottle->getDriveMode() == DriveMode::drvAC ) {
        lcd.printf(" %cAC   %3d", rev, _rightThrottle->getSpeed());
      }
      if ( _rightThrottle->getDriveMode() == DriveMode::drvCAB ) {
        if ( _rightThrottle->getBrake() == 0 ) {
          lcd.printf(" %cT%-3d %3d", rev, _rightThrottle->getTargetSpeed(), _rightThrottle->getSpeed());
        } else {
          lcd.printf(" %cB%-3d %3d", rev, _rightThrottle->getBrake(), _rightThrottle->getSpeed());
        }
      }
    } else lcd.printf("          ");
  } else if ( _operationMode == OperationMode::opFunction ) {
    menu.hide();

    lcd.setCursor(0,0);
    if (_leftThrottle) lcd.printf("012345678%c", byte(2)); 
    else lcd.printf("         %c", byte(2));
    if (_rightThrottle) lcd.printf(" 012345678"); 
    else lcd.printf("          ");
    lcd.setCursor(0,1);
    if (_leftThrottle) lcd.printf("%s%s%s%s%s%s%s%s%s%c", BOOL_STR(_leftThrottle->getFunction(0)), BOOL_STR(_leftThrottle->getFunction(1)), BOOL_STR(_leftThrottle->getFunction(2)), BOOL_STR(_leftThrottle->getFunction(3)), BOOL_STR(_leftThrottle->getFunction(4)), BOOL_STR(_leftThrottle->getFunction(5)), BOOL_STR(_leftThrottle->getFunction(6)), BOOL_STR(_leftThrottle->getFunction(7)), BOOL_STR(_leftThrottle->getFunction(8)), byte(2));
    else lcd.printf("         %c", byte(2));
    if (_rightThrottle) lcd.printf(" %s%s%s%s%s%s%s%s%s", BOOL_STR(_rightThrottle->getFunction(0)), BOOL_STR(_rightThrottle->getFunction(1)), BOOL_STR(_rightThrottle->getFunction(2)), BOOL_STR(_rightThrottle->getFunction(3)), BOOL_STR(_rightThrottle->getFunction(4)), BOOL_STR(_rightThrottle->getFunction(5)), BOOL_STR(_rightThrottle->getFunction(6)), BOOL_STR(_rightThrottle->getFunction(7)), BOOL_STR(_rightThrottle->getFunction(8)) );
    else lcd.printf("          ");
  } else if ( _operationMode == OperationMode::opMenu ) {
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
SlotClass *CommandStationClass::getSlotByAddress(uint16_t addr) {
  for (SlotClass *s = _slots->getFirst(); s; s = s->getNext()) {
    if (s->getAddress() == addr) {
      return s;
    }
  }
  return nullptr;
}

SlotClass *CommandStationClass::getSlotById(uint8_t id) {
  for (SlotClass *s = _slots->getFirst(); s; s = s->getNext()) {
    if (s->getId() == id) {
      return s;
    }
  }
  return nullptr;
}

LocoClass *CommandStationClass::getLocoByAddress(uint16_t addr) {
  for (LocoClass *l = _locos->getFirst(); l; l = l->getNext()) {
    if (l->getAddress() == addr) {
      return l;
    }
  }
  return nullptr;
}

LocoClass *CommandStationClass::getLocoByName(const char* name) {
  for (LocoClass *l = _locos->getFirst(); l; l = l->getNext()) {
    if (strcmp(l->getName(), name) == 0) {
      return l;
    }
  }
  return nullptr;
}

uint8_t CommandStationClass::getLocoCount() {
  uint8_t i = 0;

  for (LocoClass *l = _locos->getFirst(); l; l = l->getNext()) i++;
  return i;
}


ThrottleClass *CommandStationClass::getThrottleByAddress(uint16_t addr) {
  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    if (t->getAddress() == addr) {
      return t;
    }
  }
  return nullptr;
}

ThrottleClass *CommandStationClass::getThrottleByButton(uint8_t button) {
  for (ThrottleClass *t = _throttles->getFirst(); t; t = t->getNext()) {
    if (t->getButton() == button) {
      return t;
    }
  }
  return nullptr;
}


CommandStationClass CommandStation = CommandStationClass();