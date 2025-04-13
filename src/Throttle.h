#pragma once

#include "Arduino.h"
#include "Loco.h"

class ThrottleClass {
  public:
    ThrottleClass(LocoClass *loco);

    uint16_t getAddress() { return _loco->getAddress(); }
    //void setAddress(uint16_t addr) { _addr = addr; }

    int8_t getReverser() { return _reverser; }
    void incReverser();
    void decReverser();

    int8_t getBrake() { return _brake; }
    int8_t getThrottle() { return _throttle; }
    void incGear();
    void decGear();

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
    int8_t _brake = 0;
    int8_t _throttle = 0;
    int8_t _reverser = 0;
    static ThrottleClass *_first;
    ThrottleClass *_next = nullptr;

    /// @brief Remove the throttle from the list
    /// @param throttle Pointer to the throttle to remove
    void _removeFromList(ThrottleClass *throttle);
};