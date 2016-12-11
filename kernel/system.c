#include "system.h"
#include "tty.h"

void *memcpy(void *destination, const void *source, size_t num) {
  char* d = destination;
  char* s = (void*)source;
  for (size_t i = 0; i < num; i++) {
    d[i] = s[i];
  }

  return destination;
}

void *memset(void *destination, int c, size_t num) {
  char* d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = c;
  }

  return destination;
}

unsigned short *memsetw(unsigned short *destination, unsigned short v, size_t num) {
  unsigned short* d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }

  return destination;
}

int strlen(char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {}
  return i;
}

unsigned char inportb (unsigned short port) {
  unsigned char rv;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
  return rv;
}

void outportb (unsigned short port, unsigned char data) {
  __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

void putc (char c) {
  tty_putc(c);
}

void puts (char *str) {
  for (int i = 0; i < strlen(str); i++) {
    putc(str[i]);
  }
}
