#ifndef string_utils_h
#define string_utils_h

#include "Arduino.h"
#include <cstring>   // for memcpy
#include <cstdlib>   // for malloc
#include <iostream>
#include <map>
#include <algorithm>
#define BOOL_STR(b) ((b) ? "X" : " ")

char* rtrim(char* c, char junk);
char* ltrim(char* c, char junk);
char* substr(const char* c, int pos, int length);
uint16_t convertStringToUint16(const char* str);

#endif