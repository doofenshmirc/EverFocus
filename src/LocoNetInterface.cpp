#include "LocoNetInterface.h"

LocoNetBus bus;
LocoNetESP32 locoNetPhy(&bus, LOCONET_RX, LOCONET_TX, 0);

LocoNetInterfaceClass::LocoNetInterfaceClass(LocoNetBus *bus) : LocoNetDispatcher(bus) {
}
  
void LocoNetInterfaceClass::init() {
  locoNetPhy.begin();

  onPacket(CALLBACK_FOR_ALL_OPCODES, [](const lnMsg *rxPacket) {
    LocoNetInterface.processPacket(rxPacket);
  });
}

void LocoNetInterfaceClass::setSpeed(uint8_t id, uint8_t speed) {
  send(OPC_LOCO_SPD, id, speed);   
}

void LocoNetInterfaceClass::setDirFunct0to4(uint8_t id, uint8_t direction, uint16_t functions) {
  uint8_t dirFunct0to4 = (direction << 5) | ((functions & 0x1) << 4) | ((functions & 0x1E) >> 1);
  DIAG("LocoNetInterfaceClass::sendDirFunct0to4 id:%u, dir:%u, func:%u\n", id, direction, functions);
  send(OPC_LOCO_DIRF, id, dirFunct0to4);
}

void LocoNetInterfaceClass::setFunct5to8(uint8_t id, uint16_t functions) {
  uint8_t funct5to8 = ((functions & 0x1E0) >> 5);
  send(OPC_LOCO_SND, id, funct5to8);
}

void LocoNetInterfaceClass::setSwitch(uint16_t addr, uint8_t out, uint8_t dir) {
  requestSwitch(&bus, addr, out, dir);
}

void LocoNetInterfaceClass::setSensor(uint16_t addr, uint8_t state) {
  reportSensor(&bus, addr, state);
}
  
void LocoNetInterfaceClass::setPower(uint8_t power) {
  reportPower(&bus, power);
}

void LocoNetInterfaceClass::EmergencyStop(uint8_t src) {
  send(OPC_IDLE, 0, 0);
}

void LocoNetInterfaceClass::processPacket(const LnMsg *packet) {
  uint16_t addr;
  SlotClass *slot;
  char tmp[100];
  
  formatMsg(*packet, tmp, sizeof(tmp));
  DIAG("Receive loconet packet: %s\n", tmp);

  switch(packet->sd.command) {
    case OPC_GPON:
      CommandStation.setPower(PowerOn, SRC_LOCONET);
      break;

    case OPC_GPOFF:
      CommandStation.setPower(PowerOff, SRC_LOCONET);
      break;

    case OPC_IDLE:
      CommandStation.EmergencyStop(SRC_LOCONET);
      break;

    case OPC_WR_SL_DATA:
    case OPC_SL_RD_DATA:
      addr = ((uint16_t)packet->sd.adr2 << 7) + packet->sd.adr;
      slot = CommandStation.addSlot(addr, packet->sd.slot);
      DIAG("LocoNetInterfaceClass::processMessage OPC_SL_RD_DATA addr:%u, id:%u\n", addr, slot->getId());

      if ( slot->getId() > 0 && slot->getId() < MAX_SLOT_NUM ) {
        CommandStation.setSlotSpeed(addr, packet->sd.spd, SRC_LOCONET);
        CommandStation.setSlotDirection(addr, (Direction)((packet->sd.dirf & 0x20) >> 5), SRC_LOCONET);
        CommandStation.setSlotFunctions(addr, (CommandStation.getSlotFunctions(addr) & 0xFE00) | (((uint16_t)packet->sd.snd & 0x0F) << 5 ) | (((uint16_t)packet->sd.dirf & 0xF) << 1) | (((uint16_t)packet->sd.dirf & 0x10) >> 4), SRC_LOCONET);
        
      }
      break;

    case OPC_LOCO_SPD:
      addr = CommandStation.getSlotAddress(packet->ld.slot);
      DIAG("LocoNetInterfaceClass::processMessage OPC_LOCO_SPD addr:%u, speed:%u\n", addr, packet->ld.data);

      if ( addr > 0 ) {  
        CommandStation.setSlotSpeed(addr, packet->ld.data, SRC_LOCONET);
      } else send(OPC_RQ_SL_DATA, packet->ld.slot, 0);
      break;

    case OPC_LOCO_DIRF:
      addr = CommandStation.getSlotAddress(packet->ld.slot);
      DIAG("LocoNetInterfaceClass::processMessage OPC_LOCO_DIRF addr:%u, dirf:%u\n", addr, packet->ld.data);

      if ( addr > 0 ) { 
        CommandStation.setSlotDirection(addr, ((packet->ld.data & 0x20) >> 5), SRC_LOCONET);
        CommandStation.setSlotFunctions(addr, (CommandStation.getSlotFunctions(addr) & 0xFFE0) | (((uint16_t)packet->ld.data & 0xF) << 1) | (((uint16_t)packet->ld.data & 0x10) >> 4), SRC_LOCONET);
      } else send(OPC_RQ_SL_DATA, packet->ld.slot, 0);
      break;

    case OPC_LOCO_SND:
      addr = CommandStation.getSlotAddress(packet->ld.slot);
      DIAG("LocoNetInterfaceClass::processMessage OPC_LOCO_SND addr:%u, snd:%u\n", addr, packet->ld.data);

      if ( addr > 0 ) {
        if ( CommandStation.setSlotFunctions(addr, (CommandStation.getSlotFunctions(addr) &0xFE1F ) | ((uint16_t)packet->ld.data << 5), SRC_LOCONET), SRC_LOCONET);
      } else send(OPC_RQ_SL_DATA, packet->ld.slot, 0);
      break;

    case OPC_SLOT_STAT1:
      addr = CommandStation.getSlotAddress(packet->ld.slot);
      DIAG("LocoNetInterfaceClass::processMessage OPC_SLOT_STAT1 addr:%u, snd:%u\n", addr, packet->ld.data);

      if ( addr > 0 ) {
        CommandStation.setSlotStatus(addr, packet->ld.data);
      } else send(OPC_RQ_SL_DATA, packet->ld.slot, 0);
      break;
    
    case OPC_SW_REQ:
      addr = (packet->srq.sw1 | ((packet->srq.sw2 & 0x0F) << 7)) + 1;
      DIAG("LocoNetInterfaceClass::processMessage OPC_SW_REQ addr:%u, out:%u, dir:%u\n", addr, packet->srq.sw2 & OPC_SW_REQ_OUT, packet->srq.sw2 & OPC_SW_REQ_DIR);

      CommandStation.setSwitch(addr, packet->srq.sw2 & OPC_SW_REQ_OUT, packet->srq.sw2 & OPC_SW_REQ_DIR, SRC_LOCONET);
      break;  

    default:
        DIAG("Ignore LocoNet message\n");      
        break;
  }
}

LocoNetInterfaceClass LocoNetInterface(&bus);