#include "string.h"
#include <stddef.h>
#include <stdint.h>

// TODO: Implement
void * memchr(const void * ptr, int value, size_t num) {
  return 0;
}

int memcmp(const void * ptr1, const void * ptr2, size_t num) {
  unsigned char * a = (unsigned char *)ptr1;
  unsigned char * b = (unsigned char *)ptr2;
  for (size_t i = 0; i < num; i++) {
    unsigned char v = a[i] - b[i];
    if (v != 0) return v;
  }
  return 0;
}

void * memcpy(void * destination, const void * source, size_t num) {
  char * d = destination;
  char * s = (void *)source;
  for (size_t i = 0; i < num; i++) {
    d[i] = s[i];
  }

  return destination;
}

// FIXME:
void * memmove(void * destination, const void * source, size_t num) {
  return memcpy(destination, source, num);
}

void * memset(void * ptr, int value, size_t num) {
  char * d = ptr;
  for (size_t i = 0; i < num; i++) {
    d[i] = value;
  }

  return ptr;
}

size_t strlen(const char * str) {
  size_t length = 0;
  while (str[length] != 0) {
    length++;
  }

  return length;
}

char * strcpy(char * destination, const char * source) {
  size_t i = 0;
  while(source[i] != 0) {
    destination[i] = source[i];
    i++;
  }
  return destination;
}

char * strncpy(char * destination, const char * source, size_t num) {
  size_t i = 0;
  while((source[i] != 0) && (i < num)) {
    destination[i] = source[i];
    i++;
  }

  while(i < num) {
    destination[i] = (char)0;
    i++;
  }

  return destination;
}

char * strcat(char * destination, const char * source) {
  return strcpy((char *)destination + (strlen(destination)+1), source);
}

char * strncat(char * destination, const char * source, size_t num) {
  size_t s_size = strlen(source);
  return strncpy((char *)destination + (strlen(destination)+1), source, (num > s_size) ? s_size : num);
}

int strcmp(const char * str1, const char * str2) {
  return memcmp((void *)str1, (void *)str2, strlen(str1));
}

int strncmp(const char * str1, const char * str2, size_t num) {
  return memcmp((void *)str1, (void *)str2, num);
}
