#pragma once

#include "DCCEXInterface.h"
#include "LocoNetInterface.h"
#include "XpressNetInterface.h"
#include "Throttle.h"
#include "Loco.h"
#include <ItemBack.h>
#include <ItemSubMenu.h>
#include <LcdMenu.h>
#include <MenuScreen.h>
#include <SimpleRotary.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <input/SimpleRotaryAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>
#include "config.h"
#include "diag.h"

class CommandStationClass {
  public:
    CommandStationClass();

    void init();

    uint16_t getLocoAddress(uint8_t id);

    LocoClass *addLoco(uint16_t addr, uint8_t id, const char *name);
    
    ThrottleClass *addThrottle(uint16_t addr, const char *name);
    
    const char *getLocoName(uint16_t addr);
    void setLocoName(uint16_t addr, const char *name);

    uint8_t getLocoStatus(uint16_t addr);
    void setLocoStatus(uint16_t addr, uint8_t status);
    
    uint8_t getLocoSpeed(uint16_t addr);
    void setLocoSpeed(uint16_t addr, uint8_t speed, uint8_t src);

    uint8_t getLocoDirection(uint16_t addr);
    void setLocoDirection(uint16_t addr, uint8_t dir, uint8_t src);

    uint16_t getLocoFunctions(uint16_t addr);
    void setLocoFunctions(uint16_t addr, uint16_t functions, uint8_t src);

    void setSensor(uint16_t addr, uint8_t state, uint8_t src);

    void setSwitch(uint16_t addr, uint8_t out, uint8_t dir, uint8_t src);

    uint8_t getPower() { return _power; };
    void setPower(uint8_t power, uint8_t src);

    void EmergencyStop(uint8_t src);

    void check();

  private:
    uint8_t _power;
    LocoClass *_locos = nullptr;
    ThrottleClass *_throttles = nullptr;
    ThrottleClass *_leftThrottle = nullptr;
    ThrottleClass *_rightThrottle = nullptr;
    LocoClass *_getLocoById(uint8_t id);
    LocoClass *_getLocoByAddress(uint16_t addr);
    ThrottleClass *_getThrottleByAddress(uint16_t addr);
};

extern CommandStationClass CommandStation;