#ifndef Throttle_h
#define Throttle_h

#include "Arduino.h"
#include "Slot.h"
#include "Loco.h"
#include "config.h"

enum DriveMode {
  drvAC = 0,
  drvDC = 1,
  drvCAB = 2
};  

class ThrottleClass {
  public:
    ThrottleClass(SlotClass *slot, LocoClass *loco, uint8_t driveMode, uint8_t button);

    uint8_t getButton() { return _button; }
    void setButton(uint8_t button) { _button = button; }

    LocoClass* getLoco() { return _loco; }
    void setLoco(LocoClass* loco) { _loco = loco; }

    uint16_t getAddress() { return _slot->getAddress(); }

    uint8_t getSpeed() { return _slot->getSpeed(); }
    uint8_t getTargetSpeed();

    uint8_t getDirection() { return _slot->getDirection(); }
    
    const char *getName() { return _loco->getName(); }

    int8_t getReverser() { return _reverser; }
    void incReverser();
    void decReverser();

    int8_t getBrake() { return _brake; }
    
    int8_t getThrottle() { return _throttle; }
    
    void incDrive();
    void decDrive();

    uint8_t getDriveMode() { return _driveMode; }
    void setDriveMode(uint8_t driveMode) { _driveMode = driveMode; }

    double getSpeedChangeTime() { return _speedChangeTime; }
    void setSpeedChangeTime(double speedChangeTime) { _speedChangeTime = speedChangeTime; }

    bool getFunction(uint8_t function) { return _slot->getFunction(function); };

    void chgFunction(uint8_t function);

    void check();

  /// @brief Get first turnout object
  /// @return Pointer to the first Turnout object
    static ThrottleClass *getFirst();

  /// @brief Set the next turnout in the list
  /// @param turnout Pointer to the next Turnout
    void setNext(ThrottleClass *throttle);

  /// @brief Get next turnout object
  /// @return Pointer to the next Turnout object
    ThrottleClass *getNext();

  /// @brief Get turnout object by turnout ID
  /// @param id ID of the turnout to retrieve
  /// @return Pointer to the turnout object or nullptr if not found
    static ThrottleClass *getByAddress(uint16_t addr);

    static ThrottleClass *getByButton(uint8_t button);

  /// @brief Clear the list of turnouts
    static void clearThrottleList();

  /// @brief Destructor for a Turnout
    ~ThrottleClass();

  private:
    SlotClass *_slot = nullptr;
    LocoClass *_loco = nullptr;
    uint8_t _driveMode = DriveMode::drvCAB;
    int8_t _brake = 0;
    int8_t _throttle = 0;
    int8_t _reverser = 0;
    uint8_t _button = 0;
    static ThrottleClass *_first;
    ThrottleClass *_next = nullptr;
    double _speedChangeTime = -1;
    //double startMomentumTimerMillis = -1;

    /// @brief Remove the throttle from the list
    /// @param throttle Pointer to the throttle to remove
    void _removeFromList(ThrottleClass *throttle);
};

#endif