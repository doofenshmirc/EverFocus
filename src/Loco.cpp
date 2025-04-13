#include "Loco.h"

LocoClass *LocoClass::_first = nullptr;

LocoClass::LocoClass(uint16_t addr, uint8_t slot_id, const char *name) {
  _slot_id = slot_id;
  _addr = addr;
  setName(name);

  if (!_first) {
    _first = this;
  } else {
    LocoClass *current = _first;
    while (current->_next != nullptr) {
      current = current->_next;
    }
    current->_next = this;
  }
}

void LocoClass::setName(const char *name) {
  if (_name) {
    delete[] _name;
    _name = nullptr;
  }
  int nameLength = strlen(name);
  _name = new char[nameLength + 1];
  strcpy(_name, name);
}


void LocoClass::setSpeed(uint8_t speed, uint8_t src) {
  if ( _speed != speed) {
    setSpeedStatus(SLOT_SPEED_BIT);
    setSourceStatus(src);
    _speed = speed;
  }
}

void LocoClass::setDirection(uint8_t dir, uint8_t src) {
  if ( _dir != dir) {
    setDirectionStatus(SLOT_DIR_BIT);
    setSourceStatus(src);
    _dir = dir;
  }
}

void LocoClass::setFunctions(uint16_t functions, uint8_t src) {
  if ( _functions != functions) {
    if (getSlotStatus() == SLOT_STAT_INIT) setFunctionsStatus(0xFFFF);
    else setFunctionsStatus(_functions ^ functions);
    setSourceStatus(src);
    _functions = functions;
  }
}

LocoClass *LocoClass::getFirst() { return _first; }

void LocoClass::setNext(LocoClass *next) { _next = next; }

LocoClass *LocoClass::getNext() { return _next; }

LocoClass::~LocoClass() {
  _removeFromList(this);
  _next = nullptr;
}

// Private methods

void LocoClass::_removeFromList(LocoClass *loco) {
  if (!loco) {
    return;
  }

  if (getFirst() == loco) {
    _first = loco->getNext();
  } else {
    LocoClass *currentLoco = _first;
    while (currentLoco && currentLoco->getNext() != loco) {
      currentLoco = currentLoco->getNext();
    }
    if (currentLoco) {
      currentLoco->setNext(loco->getNext());
    }
  }
}