#include "Throttle.h"

int target_speeds[] = TARGET_SPEEDS;
int brake_delay_times[] = BRAKE_DELAY_TIMES;
int accel_delay_times[] = ACCEL_DELAY_TIMES;

ThrottleClass *ThrottleClass::_first = nullptr;

ThrottleClass::ThrottleClass(SlotClass *slot, LocoClass *loco, uint8_t driveMode, uint8_t button) {
  _slot = slot;
  _loco = loco;
  _driveMode = driveMode;
  _button = button;
 
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

uint8_t ThrottleClass::getTargetSpeed() { 
  return target_speeds[_throttle]; 
}

void ThrottleClass::incReverser() { 
  if ( _reverser == -1 ) {
    _reverser = 0;
    _throttle = 0;
    if ( _driveMode != DriveMode::drvCAB ) _slot->setSpeed(0, SRC_THROTTLE);
  } else if ( _reverser == 0 && _slot->getSpeed() == 0 ) { 
    _reverser = 1; 
    _slot->setDirection(1, SRC_THROTTLE);
  }
}

void ThrottleClass::decReverser() { 
  if ( _reverser == 1 ) {
    _reverser = 0;
    _throttle = 0;
    if ( _driveMode != DriveMode::drvCAB ) _slot->setSpeed(0, SRC_THROTTLE);
  } else if ( _reverser == 0 && _slot->getSpeed() == 0 ) { 
    _reverser = -1; 
    _slot->setDirection(0, SRC_THROTTLE);
  }  
}

void ThrottleClass::incDrive(uint8_t amount) {
  switch (_driveMode) {
    case DriveMode::drvCAB:
      if ( _brake > 0 ) _brake--; 
      else if ( (_throttle < (sizeof(target_speeds)/sizeof(target_speeds[0]))-1) && _reverser != 0) _throttle++;
      break;
    case DriveMode::drvDC:
      if ( _slot->getSpeed() == 0 ) incReverser();
      _slot->getDirection() == 0 ? _slot->decSpeed(amount, SRC_THROTTLE) : _slot->incSpeed(amount, SRC_THROTTLE);
      if ( _slot->getSpeed() == 0 ) incReverser();
      break;
    case DriveMode::drvAC:
      if ( _reverser != 0) _slot->incSpeed(amount, SRC_THROTTLE);
      break;
  }
}

void ThrottleClass::decDrive(uint8_t amount) {
  switch (_driveMode) {
    case DriveMode::drvCAB:
      if ( _throttle > 0 ) _throttle--; 
      else if (_brake < (sizeof(brake_delay_times)/sizeof(brake_delay_times[0])-1)) {
        _brake++;
      }
      break;
    case DriveMode::drvDC:
      if ( _slot->getSpeed() == 0 ) decReverser();
      _slot->getDirection() == 0 ? _slot->incSpeed(amount, SRC_THROTTLE) : _slot->decSpeed(amount, SRC_THROTTLE);
      if ( _slot->getSpeed() == 0 ) decReverser();
      break;
    case DriveMode::drvAC:
      _slot->decSpeed(amount, SRC_THROTTLE);
      break;
  }
}

void ThrottleClass::chgFunction(uint8_t function) {
  bool current = _slot->getFunction(function);
  _slot->setFunction(function, !current, SRC_THROTTLE);
}

void ThrottleClass::EmergencyStop(uint8_t src) {
  _throttle = 0;
  _slot->setSpeed(0, src);
}

void ThrottleClass::check() {
  int8_t changeAmount = 0;
  switch (_driveMode) {
    case DriveMode::drvCAB:
      if (_slot->getSpeed()!=target_speeds[_throttle]) {
        if (_slot->getSpeed()<target_speeds[_throttle]) {  // need to accelerate
          if (millis() - _speedChangeTime >= accel_delay_times[_throttle]) {
            _speedChangeTime = millis();
            changeAmount = 1 * SPEED_AMOUNT;
            if (_slot->getSpeed()+changeAmount > target_speeds[_throttle]) changeAmount = target_speeds[_throttle]-_slot->getSpeed();  // only relevant if the speed change is greater that 1
          }
        }

        if (_slot->getSpeed()>target_speeds[_throttle]) {  // need to brake
          if (millis() - _speedChangeTime >= brake_delay_times[_brake]) {   // Check to see if the delay period has elasped.
            changeAmount = -1 * SPEED_AMOUNT;
            if (_slot->getSpeed()-changeAmount < target_speeds[_throttle]) changeAmount = target_speeds[_throttle]-_slot->getSpeed();  // only relevant if the speed change is greater that 1
          }
        }

        if (changeAmount!=0) {  
          _speedChangeTime = millis(); 
          _slot->setSpeed(_slot->getSpeed()+changeAmount, SRC_THROTTLE);
        }
      } else { _speedChangeTime = -1; }
      break;
      case DriveMode::drvDC:
      case DriveMode::drvAC:
      break;
  }
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

ThrottleClass *ThrottleClass::getByButton(uint8_t button) {
  for (ThrottleClass *t = ThrottleClass::getFirst(); t; t = t->getNext()) {
    if (t->getButton() == button) {
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