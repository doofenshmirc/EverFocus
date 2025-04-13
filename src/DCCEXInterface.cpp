#include "DCCEXInterface.h"

DCCEXInterfaceClass::DCCEXInterfaceClass() {
}

void DCCEXInterfaceClass::init(Stream *client, Stream *log) {
  _dccex.setLogStream(log);
  _dccex.setDelegate(&DCCEXInterface);
  _dccex.connect(client);
  _dccex.requestServerVersion();
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
  
void DCCEXInterfaceClass::receivedLocoUpdate(Loco *loco) {
  DIAG("Received Loco update for DCC address: %u\n", loco->getAddress());
}
 
void DCCEXInterfaceClass::receivedLocoBroadcast(int addr, int speed, Direction direction, int functions) {
  DIAG("Receive DCCEX broadcast: address=%u|speed=%u|direction=%u|functionMap=%u\n", addr, speed, (uint8_t)direction, functions);

  LocoClass *loco = CommandStation.addLoco((uint16_t)addr, 0, "NONAME");
  if ( loco->getSlotId() < 128 ) {
    if ( CommandStation.getLocoStatus(addr) != SLOT_STAT_READY ) {
      if ( CommandStation.getLocoSpeed(addr) == (uint8_t)speed && CommandStation.getLocoDirection(addr) == (uint8_t)direction && CommandStation.getLocoFunctions(addr) == (uint16_t)functions ) { 
        CommandStation.setLocoStatus(addr, SLOT_STAT_READY); 
      }
    }
    
    if ( CommandStation.getLocoStatus(addr) == SLOT_STAT_READY ) {
      CommandStation.setLocoSpeed(addr, speed, SRC_DCCEX);
      DIAG("DCCEX set loco speed %u\n", speed); 
      CommandStation.setLocoDirection(addr, direction, SRC_DCCEX);
      CommandStation.setLocoFunctions(addr, functions, SRC_DCCEX);
    }
  }
}

void DCCEXInterfaceClass::receivedSensorState(int addr, bool state) {
  DIAG("Received Sensor state changed: id=%u state=%u\n", addr, state);
  CommandStation.setSensor(addr, state, SRC_DCCEX);
}

void DCCEXInterfaceClass::setThrottle(uint16_t addr, uint8_t speed, uint8_t dir) {
  Loco loco(addr, LocoSourceEntry);

  CommandStation.setLocoStatus(addr, SLOT_STAT_UNSYNCED);
  _dccex.setThrottle(&loco, speed, (Direction)dir );
}

void DCCEXInterfaceClass::setFunctions(uint16_t addr, uint16_t functions, uint16_t funct_status) {
  Loco loco(addr, LocoSourceEntry);

  DIAG("DCCEXInterfaceClass::sendFunctions addr:%u, functions:%u, funct_status:%u\n", addr, functions, funct_status);

  CommandStation.setLocoStatus(addr, SLOT_STAT_UNSYNCED);
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

void DCCEXInterfaceClass::EmergencyStop() { 
  _dccex.emergencyStop(); 
}

void DCCEXInterfaceClass::check() {
  _dccex.check();
}

DCCEXInterfaceClass DCCEXInterface = DCCEXInterfaceClass();