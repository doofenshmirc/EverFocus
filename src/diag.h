#ifndef diag_h
#define diag_h

void serialPrintf(const char *fmt, ...);

#include "config.h"

#ifdef DEBUG_EVERFOCUS
  #ifdef __AVR__
    #define DIAG(format, ...) serialPrintf(format, ##__VA_ARGS__) 
  #endif
  #ifdef ESP8266
    #define DIAG(format, ...) printf(format, ##__VA_ARGS__)
  #endif
  #ifdef ESP32
    #define DIAG(format, ...) printf(format, ##__VA_ARGS__)
  #endif
#else
  #define DIAG(format, ...)
#endif

#endif
