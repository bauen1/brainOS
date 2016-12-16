#include "string.h"
#include <stddef.h>
#include <stdint.h>

void* memchr(const void* ptr, int value, size_t num) {
  return 0x00;
}

int memcmp(const void* ptr1, const void* ptr2, size_t num) {
  return 0x00;
}

void* memcpy(void* destination, const void* source, size_t num) {
  char* d = destination;
  char* s = (void*)source;
  for (size_t i = 0; i < num; i++) {
    d[i] = s[i];
  }

  return destination;
}

void* memmove(void* destination, const void* source, size_t num) {
  return memcpy(destination, source, num); // TODO: FIXME:
}

void* memset(void* ptr, int value, size_t num) {
  char* d = ptr;
  for (size_t i = 0; i < num; i++) {
    d[i] = value;
  }

  return ptr;
}






size_t strlen(const char *str) {
  size_t length = 0;
  while (str[length]) {
    length++;
  }

  return length;
}
