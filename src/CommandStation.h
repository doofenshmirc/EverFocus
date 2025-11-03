#ifndef CommandStation_h
#define CommandStation_h

#include "DCCEXInterface.h"
#include "LocoNetInterface.h"
#include "XpressNetInterface.h"
#include "Throttle.h"
#include "Slot.h"
#include "Joy.h"

#include <ItemValue.h>
#include <ItemBool.h>
#include <ItemBack.h>
#include <ItemCommand.h>
#include <ItemList.h>
#include <ItemSubMenu.h>
#include <LcdMenu.h>
#include <ItemWidget.h>
#include <ItemInputCharset.h>

//#include <widget/WidgetBool.h>
#include <widget/WidgetList.h>
#include <widget/WidgetRange.h>
#include <MenuScreen.h>
#include <display/LiquidCrystal_I2CAdapter.h>
#include <renderer/CharacterDisplayRenderer.h>

#include "WiFi.h"
#include "Preferences.h"
#include "LiquidCrystal_I2C.h"
#include "NewEncoder.h"
#include "Wire.h"
#include "I2CKeyPad8x8.h"
#include "config.h"
#include "diag.h"
#include "string_utils.h"

enum OperationMode {
  opDrive = 0,
  opFunction = 1,
  opMenu = 2
};

class CommandStationClass {
  public:
    bool wifi = WIFI;
    String wifiSSID = WIFI_SSID;
    String wifiPass = WIFI_PASS;

    CommandStationClass();
    void init();
    
    uint16_t getSlotAddress(uint8_t id);

    SlotClass *addSlot(uint16_t addr, uint8_t id);
    
    LocoClass *addLoco(uint16_t addr, const char *name);
    void delLoco(uint16_t addr);

    ThrottleClass *addThrottle(uint16_t addr, uint8_t driveMode, uint8_t button);
    void delThrottle(uint16_t addr);

    const char *getLocoName(uint16_t addr);
    void setLocoName(uint16_t addr, const char *name);
    uint8_t getLocoCount();

    uint8_t getSlotStatus(uint16_t addr);
    void setSlotStatus(uint16_t addr, uint8_t status);
    
    uint8_t getSlotSpeed(uint16_t addr);
    void setSlotSpeed(uint16_t addr, uint8_t speed, uint8_t src);

    uint8_t getSlotDirection(uint16_t addr);
    void setSlotDirection(uint16_t addr, uint8_t dir, uint8_t src);

    uint16_t getSlotFunctions(uint16_t addr);
    void setSlotFunctions(uint16_t addr, uint16_t functions, uint8_t src);

    void setSensor(uint16_t addr, uint8_t state, uint8_t src);

    void setSwitch(uint16_t addr, uint8_t out, uint8_t dir, uint8_t src);

    uint8_t getPower() { return _power; };
    void setPower(uint8_t power, uint8_t src);

    void EmergencyStop(uint8_t src);

    void updateLocoMenu();
    void updateLocoInputMenu(uint16_t addr);

    void updateThrottleMenu();
    void updateThrottleInputMenu(uint16_t addr);

    void inputLoop();
    void interfacesLoop();
    void throttleLoop();
    void displayLoop();

    void saveConfig();
    void loadConfig();

    void check();

  private:
    LiquidCrystal_I2C *_lcd;
    OperationMode _operationMode = OperationMode::opDrive;
    uint8_t _power;
    SlotClass *_slots = nullptr;
    LocoClass *_locos = nullptr;
    ThrottleClass *_throttles = nullptr;
    ThrottleClass *_leftThrottle = nullptr;
    ThrottleClass *_rightThrottle = nullptr;
    SlotClass *getSlotById(uint8_t id);
    SlotClass *getSlotByAddress(uint16_t addr);
    LocoClass *getLocoByAddress(uint16_t addr);
    LocoClass *getLocoByName(const char* name);
    ThrottleClass *getThrottleByAddress(uint16_t addr);
    ThrottleClass *getThrottleByButton(uint8_t button);
    uint8_t _getFunctionChar(uint8_t functions, uint8_t side);
};

extern CommandStationClass CommandStation;

#endif