#pragma once

#include <XpressNetMaster.h>
#include "CommandStation.h"
#include "config.h"

class XpressNetInterfaceClass : public XpressNetMasterClass {
  public:
    XpressNetInterfaceClass();
//    void reportSwitch(uint16_t addr);
//    void sendSpeed(uint16_t addr, uint8_t speed);
//    void sendDirFunct0to4(uint8_t id, uint8_t direction, uint16_t functions);
//    void sendFunct5to8(uint8_t id, uint16_t functions);
//    void sendSwitch(uint16_t addr, uint8_t out, uint8_t dir);
//    void sendSensor(uint16_t addr, uint8_t state);
    void setPower(uint8_t power);
    void EmergencyStop();
};

extern XpressNetInterfaceClass XpressNetInterface;