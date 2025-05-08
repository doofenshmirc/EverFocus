#include "Loco.h"

LocoClass *LocoClass::_first = nullptr;

LocoClass::LocoClass(uint16_t addr, const char* name) {
  setAddress(addr);
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
  