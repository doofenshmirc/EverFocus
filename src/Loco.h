#ifndef Loco_H
#define Loco_H

#include <Arduino.h>

//Sources
#define SRC_NULL      0x00
#define SRC_LOCONET   0x01
#define SRC_XPRESSNET 0x02
#define SRC_DCCEX     0x03
#define SRC_Z21       0x04
#define SRC_THROTTLE  0x05

//Slot status
#define SLOT_SRC_BIT       0x07
#define SLOT_SPEED_BIT     0x08
#define SLOT_DIR_BIT       0x10
#define SLOT_STAT_BIT      0xE0
#define SLOT_STAT_INIT     0x00
#define SLOT_STAT_READY    0x20
#define SLOT_STAT_UNSYNCED 0x40

//Function status
#define FUNCT_STAT_IDLE    0x00

class LocoClass {
public:
  LocoClass(uint16_t addr, uint8_t slot_id, const char *name);

  uint8_t getSlotId() { return _slot_id; }
  void setSlotId(uint8_t slot_id) { _slot_id = slot_id; }

  uint16_t getAddress() { return _addr; }
  void setAddress(uint16_t addr) { _addr = addr; }

  const char *getName() { return _name; }
  void setName(const char *name);
  
  uint8_t getSlotStatus() { return _slot_status & SLOT_STAT_BIT; }
  void setSlotStatus(uint8_t status) { _slot_status = (_slot_status & ~SLOT_STAT_BIT) | status; }

  uint8_t getSourceStatus() { return _slot_status & SLOT_SRC_BIT; }
  void setSourceStatus(uint8_t status) { _slot_status = (_slot_status & ~SLOT_SRC_BIT ) | status; }

  uint8_t getSpeedStatus() { return (_slot_status & SLOT_SPEED_BIT); }
  void setSpeedStatus(uint8_t status) { _slot_status = (_slot_status & ~SLOT_SPEED_BIT) | status; }
  
  uint8_t getDirectionStatus() { return _slot_status & SLOT_DIR_BIT; }
  void setDirectionStatus(uint8_t status) { _slot_status = (_slot_status & ~SLOT_DIR_BIT) | status; }
  
  uint16_t getFunctionsStatus() { return _func_status; }
  void setFunctionsStatus(uint16_t status) { _func_status = status; };
  
  uint8_t getSpeed() { return _speed; }
  void setSpeed(uint8_t speed, uint8_t src);

  uint8_t getDirection() { return _dir; }
  void setDirection(uint8_t dir, uint8_t src);

  uint16_t getFunctions() { return _functions; }
  void setFunctions(uint16_t functions, uint8_t src);

  static LocoClass *getFirst();
  void setNext(LocoClass *slot);
  LocoClass *getNext();

  /// @brief Clear the list of Slot
  static void clearSlotList();

  /// @brief Destructor for a Slot
  ~LocoClass();

private:
  uint8_t _slot_id;
  uint8_t _slot_status = SLOT_STAT_READY;
  uint16_t _addr;
  char *_name = nullptr;
  uint8_t _speed = 0;
  uint8_t _dir = 0;
  uint16_t _functions = 0;
  uint16_t _func_status = 0xFFFF;
  static LocoClass *_first;
  LocoClass *_next = nullptr;

  void _removeFromList(LocoClass *slot);
};

#endif