#ifndef LocoNetInterface_h
#define LocoNetInterface_h

#include "LocoNetSerial.h"
#include <LocoNetESP32.h>
#include "CommandStation.h"
#include "config.h"

class LocoNetInterfaceClass : public LocoNetDispatcher {
  public:
    LocoNetInterfaceClass(LocoNetBus *bus);
    void init();
    void processPacket(const LnMsg *packet);
    void setSpeed(uint8_t id, uint8_t speed);
    void setDirFunct0to4(uint8_t id, uint8_t direction, uint16_t functions);
    void setFunct5to8(uint8_t id, uint16_t functions);
    void setSwitch(uint16_t addr, uint8_t out, uint8_t dir);
    void setSensor(uint16_t addr, uint8_t state);
    void setPower(uint8_t power);
    void EmergencyStop();
};

extern LocoNetInterfaceClass LocoNetInterface;

#endif