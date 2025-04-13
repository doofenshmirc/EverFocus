#include "Throttle.h"

ThrottleClass *ThrottleClass::_first = nullptr;

ThrottleClass::ThrottleClass(LocoClass *loco) {
  _loco = loco;
 
  if (!_first) {
    _first = this;
  } else {
    ThrottleClass *current = _first;
    while (current->_next != nullptr) {
      current = current->_next;
    }
    current->_next = this;
  }
}

void ThrottleClass::incReverser() { 
  if ( _loco->getSpeed() == 0 && _reverser < 1) { _reverser++; }
}

void ThrottleClass::decReverser() { 
  if ( _loco->getSpeed() == 0 && _reverser > -1) { _reverser--; }
}

void ThrottleClass::incGear() {
  if ( _brake < 0 ) { 
    _brake++; 
  } else if ( _throttle < 5 ) {
    _throttle++;
  }
}

void ThrottleClass::decGear() {
  if ( _throttle > 0 ) { 
    _throttle--; 
  } else if ( _brake > -5 ) {
    _brake--;
  }
}

void ThrottleClass::check() {
  // Routine calls here included in the loop to read encoder or other inputs
}

ThrottleClass *ThrottleClass::getFirst() { return _first; }

void ThrottleClass::setNext(ThrottleClass *throttle) { _next = throttle; }

ThrottleClass *ThrottleClass::getNext() { return _next; }

ThrottleClass *ThrottleClass::getByAddress(uint16_t addr) {
  for (ThrottleClass *t = ThrottleClass::getFirst(); t; t = t->getNext()) {
    if (t->getAddress() == addr) {
      return t;
    }
  }
  return nullptr;
}

void ThrottleClass::clearThrottleList() {
  // Count Throttle in list
  int throttleCount = 0;
  ThrottleClass *currentThrottle = ThrottleClass::getFirst();
  while (currentThrottle != nullptr) {
    throttleCount++;
    currentThrottle = currentThrottle->getNext();
  }

  // Store Turnout pointers in an array for clean up
  ThrottleClass **deleteThrottle = new ThrottleClass *[throttleCount];
  currentThrottle = ThrottleClass::getFirst();
  for (int i = 0; i < throttleCount; i++) {
    deleteThrottle[i] = currentThrottle;
    currentThrottle = currentThrottle->getNext();
  }

  // Delete each Turnout
  for (int i = 0; i < throttleCount; i++) {
    delete deleteThrottle[i];
  }

  // Clean up the array of pointers
  delete[] deleteThrottle;

  // Reset first pointer
  ThrottleClass::_first = nullptr;
}

ThrottleClass::~ThrottleClass() {
  _removeFromList(this);

  _next = nullptr;
}

// Private methods

void ThrottleClass::_removeFromList(ThrottleClass *throttle) {
  if (!throttle) {
    return;
  }

  if (getFirst() == throttle) {
    _first = throttle->getNext();
  } else {
    ThrottleClass *currentThrottle = _first;
    while (currentThrottle && currentThrottle->getNext() != throttle) {
      currentThrottle = currentThrottle->getNext();
    }
    if (currentThrottle) {
      currentThrottle->setNext(throttle->getNext());
    }
  }
}