#include "string_utils.h"

char* rtrim(char* str, char junk)
{
  char* original = str + strlen(str);
  
  while(*--original == junk);
  *(original + 1) = '\0';

  return str;
}

char* ltrim(char* str, char junk)
{
  while(str[0] == junk) str++;
  
  return str;
}

char* substr(const char* str, int pos, int length) {
    if (pos < 0 || length < 0 || str == nullptr) return nullptr;
    int src_len = strlen(str);
    if (pos >= src_len) return nullptr; // start position out of bounds

    // Adjust length if it goes past the end of the source string
    if (pos + length > src_len) length = src_len - pos;

    char* result = (char*)malloc(length + 1); // +1 for null terminator
    if (!result) return nullptr; // allocation failed

    std::memcpy(result, str + pos, length);
    result[length] = '\0'; // null-terminate
    return result;
}

uint16_t convertStringToUint16(const char* str) {
    // Convert string to unsigned long
    unsigned long value = strtoul(str, NULL, 10);
    
    // Check for range and return as uint16_t
    if (value <= UINT16_MAX) {
        return (uint16_t)value;
    } else {
        // Handle error - value out of range
        return 0; // or some error value
    }
}