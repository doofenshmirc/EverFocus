#pragma once

#include "Arduino.h"
#include "Loco.h"
#include "config.h"
//#include "diag.h"

enum DriveMode {
  styleAC = 0,
  styleDC = 1,
  styleCab = 2
};

class ThrottleClass {
  public:
    ThrottleClass(LocoClass *loco);

    uint16_t getAddress() { return _loco->getAddress(); }
    uint8_t getSpeed() { return _loco->getSpeed(); }
    uint8_t getTargetSpeed();
    uint8_t getDirection() { return _loco->getDirection(); }
    const char *getName() { return _loco->getName(); }
    DriveMode getDriveMode() { return _driveMode; }

    int8_t getReverser() { return _reverser; }
    void incReverser();
    void decReverser();

    int8_t getBrake() { return _brake; }
    int8_t getThrottle() { return _throttle; }
    void incDrive();
    void decDrive();

    void setDriveMode(DriveMode driveMode) { _driveMode = driveMode; }

    double getSpeedChangeTime() { return _speedChangeTime; }
    void setSpeedChangeTime(double speedChangeTime) { _speedChangeTime = speedChangeTime; }
    
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

  /// @brief Clear the list of turnouts
    static void clearThrottleList();

  /// @brief Destructor for a Turnout
    ~ThrottleClass();

  private:
    LocoClass *_loco;
    DriveMode _driveMode = DriveMode::styleCab;
    int8_t _brake = 0;
    int8_t _throttle = 0;
    int8_t _reverser = 0;
    static ThrottleClass *_first;
    ThrottleClass *_next = nullptr;
    double _speedChangeTime = -1;
    //double startMomentumTimerMillis = -1;

    /// @brief Remove the throttle from the list
    /// @param throttle Pointer to the throttle to remove
    void _removeFromList(ThrottleClass *throttle);
};