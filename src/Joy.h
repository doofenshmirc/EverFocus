#ifndef Joy_h
#define Joy_h

#include "Arduino.h"
#include "config.h"

enum JoyCommand {
  joyNO = 0x00,
  joyLeft = 0x01,
  joyRight = 0x02,
  joyUp = 0x04,
  joyDown = 0x08
};  

class JoyClass {
  public:
    JoyClass(uint8_t x_pin, uint8_t y_pin);
    uint8_t getCommand();
    int16_t getDeltaX();
    int16_t getDeltaY();

  private:
    uint16_t _x_pin, _y_pin;
    uint16_t _pos_x, _pos_y = 0;
    uint8_t _command = JoyCommand::joyNO;
    uint8_t _last_command = JoyCommand::joyNO;

    void readPositions();
};

#endif