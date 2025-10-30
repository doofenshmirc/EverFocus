#include "Joy.h"


JoyClass::JoyClass(uint8_t x_pin, uint8_t y_pin) {
  _x_pin = x_pin;
  _y_pin = y_pin;

  pinMode(_x_pin, INPUT);
  pinMode(_y_pin, INPUT);
}

void JoyClass::readPositions() {
  _pos_x = analogRead(_x_pin);
  _pos_y = analogRead(_y_pin);
}

uint8_t JoyClass::getCommand() {
  uint8_t command = JoyCommand::joyNO;

  readPositions();

  _command = JoyCommand::joyNO;
  if ( _pos_x > JOY_RIGHT_THRESHOLD) { _command = JoyCommand::joyRight; }
  if ( _pos_x < JOY_LEFT_THRESHOLD) { _command = JoyCommand::joyLeft; }
  if ( _pos_y < JOY_UP_THRESHOLD) { _command = JoyCommand::joyUp; }
  if ( _pos_y > JOY_DOWN_THRESHOLD) { _command = JoyCommand::joyDown; }

  if ( _last_command != _command ) { 
    _last_command = _command; 
    command = _command;
  }
 
  return command;
}

int16_t JoyClass::getDeltaX() {
  readPositions();

  return _pos_x - JOY_X_CENTER;
}

int16_t JoyClass::getDeltaY() {
  readPositions();
  
  return _pos_y - JOY_Y_CENTER;
}