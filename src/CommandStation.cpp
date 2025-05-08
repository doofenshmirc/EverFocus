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
std::vector<const char*> buttonList = {"Rec", "Stop", "DRV", "Track", "F1", "Hold", "Position", "Display", "Play", "Home", "CAM", "FocusF", "F3", "FocusN", "A_Pan", "Mode", "Copy", "Clr", "4", "Set", "Not used", "1", "7", "Seq", "Menu", "Enter", "6", "Not used", "Not used", "3", "9", "Esc", "Not used", "Not used", "Not used", "Not used", "Not used", "Not used", "Not used", "Not used", "Call", "0", "5", "Shift", "Not used", "2", "8", "Search", "Pause", "Not used", "Not used", "ZoomIn", "F4", "ZoomOut", "Not used", "Zoom", "Stop", "Not used", "MON", "Iris1", "F2", "Iris2", "Tour", "Select" };
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


/*MENU_SCREEN(locoScreen, locoItems,
  ITEM_INPUT_CHARSET("Loco name", locoItemName, "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", [](char *name) { Serial.printf("New loco name:%s\n", name); } ),
  ITEM_INPUT("Loco addr", locoItemAddr, [](char *addr) { Serial.printf("New loco addr:%s\n", addr); }),
  ITEM_COMMAND("Save", []() { CommandStation.addLoco(atoi(locoItemAddr), locoItemName); menu.setScreen(locoManagerScreen); } ),
  ITEM_COMMAND("..", []() { menu.setScreen(locoManagerScreen); } )
);*/
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

/*[]() {
  // Dynamically add items to the screen at a specific position
  locosScreen->addItemAt(1, ITEM_BASIC("Network Between"));
  menu.refresh();
}),
ITEM_COMMAND("Delete from roster", []() {
  // Dynamically remove an item from the screen at a specific position
  locosScreen->removeItemAt(1);
  menu.refresh();
}),*/

void rangeCallbackA(const Ref<int>& addr) { if (DCCEXInterface.trackAType != 2 && DCCEXInterface.trackAType != 3) DCCEXInterface.trackAAddr = 0; }
void rangeCallbackB(const Ref<int>& addr) { if (DCCEXInterface.trackBType != 2 && DCCEXInterface.trackBType != 3) DCCEXInterface.trackBAddr = 0; }
void rangeCallbackC(const Ref<int>& addr) { if (DCCEXInterface.trackCType != 2 && DCCEXInterface.trackCType != 3) DCCEXInterface.trackCAddr = 0; }
void rangeCallbackD(const Ref<int>& addr) { if (DCCEXInterface.trackDType != 2 && DCCEXInterface.trackDType != 3) DCCEXInterface.trackDAddr = 0; }
MENU_SCREEN(trackManagerMenuScreen, trackManagerMenuItems,
//  ITEM_LIST_REF("Color", colors, [](const Ref<uint8_t> color) { Serial.printf("FASZA color: %d\n", color); }, A),
//  ITEM_LIST("Color", colors, [](const uint8_t color) { Serial.println("FASZA"); }, A),
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
  lcd.setCursor(0,0);
  lcd.print(CS_NAME);
  delay(1000);

  _operationMode = OperationMode::opDrive;
  
  encoder.begin();
 
  renderer.begin();

  //////Ez csak atmeneti a teszteles idejere amig nincs eeprom olvasas/////
  addThrottle(2093, "M41 2093", DriveMode::drvAC, 7);
  addThrottle(19, "M61 019", DriveMode::drvCAB, 63);
  addThrottle(219, "M62 219", DriveMode::drvCAB, 0);
  addThrottle(119, "V200 119", DriveMode::drvCAB, 56);
  /////////////////////////////////////////////////////////////////////////
  
  menu.setScreen(mainMenuScreen);
   
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
    uint8Input3 = (uint8_t)throttle->getButton();
  }
  
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Loco", locoList, [](const Ref<uint8_t> loco) { Serial.printf("FASZA loco: \n"); }, uint8Input1));
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Mode", driveMode, [](const Ref<uint8_t> drive) { Serial.printf("FASZA drive: \n"); }, uint8Input2));
  throttleInputMenuScreen->addItem(ITEM_LIST_REF("Button", buttonList, [](const Ref<uint8_t> loco) { Serial.printf("FASZA button: \n"); }, uint8Input3));
//  throttleInputMenuScreen->addItem(ITEM_COMMAND("Add or update", []() { CommandStation.addThrottle(convertStringToUint16(input1), rtrim(input2, ' ')) ; CommandStation.updateLocoMenu(); menu.setScreen(locoMenuScreen); menu.refresh(); } ));
//  throttleInputMenuScreen->addItem(ITEM_COMMAND("Remove", []() { CommandStation.delThrottle(convertStringToUint16(input1)) ; CommandStation.updateLocoMenu(); menu.setScreen(locoMenuScreen); menu.reset(); } ));
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
 
  if (loco) delete loco; 
}

ThrottleClass *CommandStationClass::addThrottle(uint16_t addr, const char *name, DriveMode driveMode, uint8_t button) {
  SlotClass *slot = addSlot(addr, 0);
  LocoClass *loco = addLoco(addr, name); 
  ThrottleClass *throttle = getThrottleByAddress(addr);
 
  if (!throttle) { 
    throttle = new ThrottleClass(slot, loco, driveMode, button); 
  } else {
    throttle->setLoco(loco);
  }

  if (!_throttles) _throttles = throttle;
    
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
        case ButtonCode::btnMenu:
          _operationMode = OperationMode::opDrive;
          break;

        case ButtonCode::btnEnter:
          menu.process(ENTER);
          break;
  
        case ButtonCode::btnEsc:
          menu.process(BACK);
          break;
      
        case ButtonCode::btnClr:
          menu.process(BACKSPACE);
          break;
  
        case ButtonCode::btnTour:
          menu.process(UP);
          break;
  
        case ButtonCode::btnSet:
          menu.process(DOWN);
          break;
  
        case ButtonCode::btnHome:
          menu.process(RIGHT);
          break;
  
        case ButtonCode::btnStop:
          menu.process(LEFT);
          break;
  
        case ButtonCode::btn0:
          menu.process('0');
          break;
        
        case ButtonCode::btn1:
          menu.process('1');
          break;
        
        case ButtonCode::btn2:
          menu.process('2');
          break;
        
        case ButtonCode::btn3:
          menu.process('3');
          break;
        
        case ButtonCode::btn4:
          menu.process('4');
          break;
        
        case ButtonCode::btn5:
          menu.process('5');
          break;
        
        case ButtonCode::btn6:
          menu.process('6');
          break;
        
        case ButtonCode::btn7:
          menu.process('7');
          break;
        
        case ButtonCode::btn8:
          menu.process('8');
          break;
        
        case ButtonCode::btn9:
          menu.process('9');
          break;

        case ButtonCode::btnShift:
          //Serial.printf("Debug modeA=%d\n", modeA);
          break;
        
        default:
          break;
      }
    }
  } else if (_operationMode == OperationMode::opDrive ) {

    switch ( currentEncoderState.currentClick ) {   //Encoder rotary
      case NewEncoder::DownClick:
        _leftThrottle->decDrive();
        break;
      case NewEncoder::UpClick:
        _leftThrottle->incDrive();
        break;
      case NewEncoder::NoClick:
      break;
    }

    switch ( currentEncoderState.currentButton ) {   //Encoder button
      case NewEncoder::DownButton:
        _leftThrottle->decReverser();
        break;
      case NewEncoder::UpButton:
        _leftThrottle->incReverser();
        break;
      case NewEncoder::NoButton:
      break;
    }
    
    if (lastIndex != index ) {
      switch (index)                                //Keyboard
      {
        case ButtonCode::btnMenu:
          _operationMode = OperationMode::opMenu;
          break;

        case ButtonCode::btnDisplay:
        case ButtonCode::btnSelect:
          _leftThrottle = getThrottleByButton(index);
          break;

        case ButtonCode::btnRec:
        case ButtonCode::btnStop:
          _rightThrottle = getThrottleByButton(index);
          break;
        
        case ButtonCode::btnTrack:
          _leftThrottle->incReverser();
          break;

        case ButtonCode::btnHold:
          _leftThrottle->decReverser();
          break;

        case ButtonCode::btnIris1:
          _leftThrottle->incDrive();
          break;

        case ButtonCode::btnIris2:
          _leftThrottle->decDrive();
          break;

        case ButtonCode::btnFocusF:
          _rightThrottle->incReverser();
          break;

        case ButtonCode::btnFocusN:
          _rightThrottle->decReverser();
          break;

        case ButtonCode::btnZoomIn:
          _rightThrottle->incDrive();
          break;

        case ButtonCode::btnZoomOut:
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
      if ( _leftThrottle->getDriveMode() == DriveMode::drvAC ) {
        lcd.printf("%c     %3d%c", rev, _leftThrottle->getSpeed(), byte(2));
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
      if ( _rightThrottle->getDriveMode() == DriveMode::drvAC ) {
        lcd.printf(" %c     %3d", rev, _rightThrottle->getSpeed());
      }
      if ( _rightThrottle->getDriveMode() == DriveMode::drvCAB ) {
        if ( _rightThrottle->getBrake() == 0 ) {
          lcd.printf(" %cT%-3d %3d", rev, _rightThrottle->getTargetSpeed(), _rightThrottle->getSpeed());
        } else {
          lcd.printf(" %cB%-3d %3d", rev, _rightThrottle->getBrake(), _rightThrottle->getSpeed());
        }
      }
    } else lcd.printf("          ");
  }

  if ( _operationMode == OperationMode::opMenu ) {
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