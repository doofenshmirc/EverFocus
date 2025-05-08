#ifndef CommandStation_h
#define CommandStation_h

#include "DCCEXInterface.h"
#include "LocoNetInterface.h"
#include "XpressNetInterface.h"
#include "Throttle.h"
#include "Slot.h"

#include <ItemInput.h>
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

#include "LiquidCrystal_I2C.h"
#include "NewEncoder.h"
#include "Wire.h"
#include "I2CKeyPad8x8.h"
#include "config.h"
#include "diag.h"
#include "string_utils.h"

enum OperationMode {
  opDrive = 0,
  opMenu = 1
};

enum ButtonCode {
  btnRec      = 0,
  btnStop_    = 1,
  btnDRV      = 2,
  btnTrack    = 3,
  btnF1       = 4,
  btnHold     = 5,
  btnPosition = 6,
  btnDisplay  = 7,
  btnPlay     = 8,
  btnHome     = 9,
  btnCAM      = 10,
  btnFocusF   = 11,
  btnF3       = 12,
  btnFocusN   = 13,
  btnA_Pan    = 14,
  btnMode     = 15,
  btnCopy     = 16,
  btnClr      = 17,
  btn4        = 18,
  btnSet      = 19,
  btn_20      = 20,
  btn1        = 21,
  btn7        = 22,
  btnSeq      = 23,
  btnMenu     = 24,
  btnEnter    = 25,
  btn6        = 26,
  btn_27      = 27,
  btn_28      = 28,
  btn3        = 29,
  btn9        = 30,
  btnEsc      = 31,
  btn_32      = 32,
  btn_33      = 33,
  btn_34      = 34,
  btn_35      = 35,
  btn_36      = 36,
  btn_37      = 37,
  btn_38      = 38,
  btn_39      = 39,
  btnCall     = 40,
  btn0        = 41,
  btn5        = 42,
  btnShift    = 43,
  btn_44      = 44,
  btn2        = 45,
  btn8        = 46,
  btnSearch   = 47,
  btnPause    = 48,
  btn_49      = 49,
  btn_50      = 50,
  btnZoomIn   = 51,
  btnF4       = 52,
  btnZoomOut  = 53,
  btn_54      = 54,
  btnZoom     = 55,
  btnStop     = 56,
  btn_57      = 57,
  btnMON      = 58,
  btnIris1    = 59,
  btnF2       = 60,
  btnIris2    = 61,
  btnTour     = 62,
  btnSelect   = 63,
  btn_64      = 64,
  btn_Error   = 65,
};

class CommandStationClass {
  public:
    CommandStationClass();

    void init();
    
    uint16_t getSlotAddress(uint8_t id);

    SlotClass *addSlot(uint16_t addr, uint8_t id);
    
    LocoClass *addLoco(uint16_t addr, const char *name);
    void delLoco(uint16_t addr);

    ThrottleClass *addThrottle(uint16_t addr, const char *name, DriveMode driveMode, uint8_t button);
    
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

    void check();

  private:
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
    ThrottleClass *getThrottleByAddress(uint16_t addr);
    ThrottleClass *getThrottleByButton(uint8_t button);
};

extern CommandStationClass CommandStation;

#endif