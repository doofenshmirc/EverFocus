#include "Slot.h"

SlotClass *SlotClass::_first = nullptr;

SlotClass::SlotClass(uint16_t addr, uint8_t slot_id) {
  _slot_id = slot_id;
  _addr = addr;

  if (!_first) {
    _first = this;
  } else {
    SlotClass *current = _first;
    while (current->_next != nullptr) {
      current = current->_next;
    }
    current->_next = this;
  }
}

void SlotClass::setSpeed(uint8_t speed, uint8_t src) {
  if ( _speed != speed) {
    setSpeedStatus(SLOT_SPEED_BIT);
    setSourceStatus(src);
    _speed = speed;
  }
}

void SlotClass::incSpeed(uint8_t amount, uint8_t src) {
  if ( _speed + amount <= 127 ) { 
    setSpeedStatus(SLOT_SPEED_BIT);
    setSourceStatus(src);
    _speed=_speed + amount; 
  }
  else { _speed = 127; }
}

void SlotClass::decSpeed(uint8_t amount, uint8_t src) {

  if ( _speed - amount >= 0 ) { 
    setSpeedStatus(SLOT_SPEED_BIT);
    setSourceStatus(src);
    _speed=_speed - amount; 
  }
  else { _speed = 0; }
}

void SlotClass::setDirection(uint8_t dir, uint8_t src) {
  if ( _dir != dir) {
    setDirectionStatus(SLOT_DIR_BIT);
    setSourceStatus(src);
    _dir = dir;
  }
}
bool SlotClass::getFunction(uint8_t function) {
  return (_functions >> function) && 0x01;
}

void SlotClass::setFunction(uint8_t function, uint16_t value, uint8_t src) {
  uint16_t mask = 0x01 << function;

  if ( (_functions & mask) ^ (value << function) ) {
    if (getStatus() == SLOT_STAT_INIT) setFunctionsStatus(0xFFFF);
    else setFunctionsStatus(mask);
    setSourceStatus(src);
    value==0 ? _functions &= !mask : _functions |= mask;
  } 
}

void SlotClass::setFunctions(uint16_t functions, uint8_t src) {
  if ( _functions != functions) {
    if (getStatus() == SLOT_STAT_INIT) setFunctionsStatus(0xFFFF);
    else setFunctionsStatus(_functions ^ functions);
    setSourceStatus(src);
    _functions = functions;
  }
}

SlotClass *SlotClass::getFirst() { return _first; }

void SlotClass::setNext(SlotClass *next) { _next = next; }

SlotClass *SlotClass::getNext() { return _next; }

SlotClass::~SlotClass() {
  _removeFromList(this);
  _next = nullptr;
}

// Private methods

void SlotClass::_removeFromList(SlotClass *slot) {
  if (!slot) {
    return;
  }

  if (getFirst() == slot) {
    _first = slot->getNext();
  } else {
    SlotClass *currentSlot = _first;
    while (currentSlot && currentSlot->getNext() != slot) {
      currentSlot = currentSlot->getNext();
    }
    if (currentSlot) {
      currentSlot->setNext(slot->getNext());
    }
  }
}