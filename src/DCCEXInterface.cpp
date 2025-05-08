#include "DCCEXInterface.h"

DCCEXInterfaceClass::DCCEXInterfaceClass() {
}

void DCCEXInterfaceClass::init(Stream *client, Stream *log) {
  _dccex.setLogStream(log);
  _dccex.setDelegate(&DCCEXInterface);
  _dccex.connect(client);
  _dccex.requestServerVersion();
  _dccex.getLists(true, false, false, false);
  setTracksType();
}

void DCCEXInterfaceClass::receivedMessage(char *message) {
  DIAG("Received message: %s\n", message);
}

void DCCEXInterfaceClass::receivedServerVersion(int major, int minor, int patch) {
  DIAG("DCCEX version: %d.%d.%d\n", major, minor, patch);
}

void DCCEXInterfaceClass::receivedTrackPower(TrackPower state) {
  DIAG("Received Track Power: %u\n", state);
  CommandStation.setPower((uint8_t)state, SRC_DCCEX);
}

void DCCEXInterfaceClass::receivedTrackType(char track, TrackManagerMode mode, int addr) {
  switch (track) {
    case 'A':
      trackAType = mode;
      trackAAddr = addr;
      break;
    case 'B':
      trackBType = mode;
      trackBAddr = addr;
      break;
    case 'C':
      trackCType = mode;
      trackCAddr = addr;
      break;
    case 'D':
      trackDType = mode;
      trackDAddr = addr;
      break;
  }
}
  
void DCCEXInterfaceClass::receivedRosterList() {
  Serial.println("\n\nReceived Roster");
  for (Loco *loco = _dccex.roster->getFirst(); loco; loco = loco->getNext()) {
    int addr = loco->getAddress();
    const char *name = loco->getName();
    
    CommandStation.addLoco(addr, name);

    Serial.print(addr);
    Serial.print(" ~");
    Serial.print(name);
    Serial.println("~");
    for (int i = 0; i < 32; i++) {
      const char *fName = loco->getFunctionName(i);
      if (fName != nullptr) {
        Serial.print("loadFunctionLabels() ");
        Serial.print(fName);
        if (loco->isFunctionMomentary(i)) {
          Serial.print(" - Momentary");
        }
        Serial.println();
      }
    }
  }
  Serial.println("\n");
}

void DCCEXInterfaceClass::receivedLocoUpdate(Loco *loco) {
  DIAG("Received Loco update for DCC address: %u\n", loco->getAddress());
}
 
void DCCEXInterfaceClass::receivedLocoBroadcast(int addr, int speed, Direction direction, int functions) {
  DIAG("Receive DCCEX broadcast: address=%u|speed=%u|direction=%u|functionMap=%u\n", addr, speed, (uint8_t)direction, functions);

  SlotClass *slot = CommandStation.addSlot((uint16_t)addr, 0);
  if ( slot->getId() < MAX_SLOT_NUM ) {
    if ( CommandStation.getSlotStatus(addr) != SLOT_STAT_READY ) {
      if ( CommandStation.getSlotSpeed(addr) == (uint8_t)speed && CommandStation.getSlotDirection(addr) == (uint8_t)direction && CommandStation.getSlotFunctions(addr) == (uint16_t)functions ) { 
        CommandStation.setSlotStatus(addr, SLOT_STAT_READY); 
      }
    }
    
    if ( CommandStation.getSlotStatus(addr) == SLOT_STAT_READY ) {
      DIAG("DCCEX set loco speed %u\n", speed); 
      CommandStation.setSlotSpeed(addr, speed, SRC_DCCEX);
      CommandStation.setSlotDirection(addr, direction, SRC_DCCEX);
      CommandStation.setSlotFunctions(addr, functions, SRC_DCCEX);
    }
  }
}

void DCCEXInterfaceClass::receivedSensorState(int addr, bool state) {
  DIAG("Received Sensor state changed: id=%u state=%u\n", addr, state);
  CommandStation.setSensor(addr, state, SRC_DCCEX);
}

void DCCEXInterfaceClass::setThrottle(uint16_t addr, uint8_t speed, uint8_t dir) {
  Loco loco(addr, LocoSourceEntry);

  CommandStation.setSlotStatus(addr, SLOT_STAT_UNSYNCED);
  _dccex.setThrottle(&loco, speed, (Direction)dir );
}

void DCCEXInterfaceClass::setFunctions(uint16_t addr, uint16_t functions, uint16_t funct_status) {
  Loco loco(addr, LocoSourceEntry);

  DIAG("DCCEXInterfaceClass::sendFunctions addr:%u, functions:%u, funct_status:%u\n", addr, functions, funct_status);

  CommandStation.setSlotStatus(addr, SLOT_STAT_UNSYNCED);
  for (uint8_t i=0; i<9; i++) {
    if ( funct_status & (0x1 << i) ) (functions >> i) & 0x1 ? _dccex.functionOn(&loco, i) : _dccex.functionOff(&loco, i);
  }
}

void DCCEXInterfaceClass::setSwitch(uint16_t addr, uint8_t out, uint8_t dir) {
  dir ? _dccex.throwTurnout(addr): _dccex.closeTurnout(addr);
}

void DCCEXInterfaceClass::setPower(uint8_t power) {
  power ? _dccex.powerOn() : _dccex.powerOff();
}

void DCCEXInterfaceClass::requestTracksType() {
  char* command = new char[13];
  
  sprintf(command, "=");
  _dccex.sendCommand(command);

  delete[] command;
}

void DCCEXInterfaceClass::requestRosterList() {
  _dccex.getLists(true, false, false, false);
}

void DCCEXInterfaceClass::setTracksType() {
  _dccex.setTrackType('A', (TrackManagerMode)trackAType, trackAAddr);
  _dccex.setTrackType('B', (TrackManagerMode)trackBType, trackBAddr);
  _dccex.setTrackType('C', (TrackManagerMode)trackCType, trackCAddr);
  _dccex.setTrackType('D', (TrackManagerMode)trackDType, trackDAddr);
}

void DCCEXInterfaceClass::EmergencyStop() { 
  _dccex.emergencyStop(); 
}

void DCCEXInterfaceClass::check() {
  _dccex.check();
}

DCCEXInterfaceClass DCCEXInterface = DCCEXInterfaceClass();