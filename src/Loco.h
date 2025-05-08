#ifndef Loco_h
#define Loco_h

#include <Arduino.h>

class LocoClass {
  public:
    LocoClass(uint16_t addr, const char* name);

    uint16_t getAddress() { return _addr; };
    void setAddress(uint16_t addr) { _addr = addr; };

    char* getName() { return _name; };
    void setName(const char* name);

    static LocoClass *getFirst();
    void setNext(LocoClass *slot);
    LocoClass *getNext();

    /// @brief Clear the list of Loco
    static void clearSlotList();

    /// @brief Destructor for a Loco
    ~LocoClass();

  private:
    uint16_t _addr;
    char* _name = nullptr;
    static LocoClass *_first;
    LocoClass *_next = nullptr;

    void _removeFromList(LocoClass *slot);
};

#endif