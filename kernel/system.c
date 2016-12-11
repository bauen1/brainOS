#include "system.h"
#include "tty.h"

void *memcpy(void *dest, void *src, int count) {
  char* d = dest;
  char* s = src;
  for (int i = 0; i < count; i ++) {
    d[i] = s[i];
  }

  return dest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count) {
  while(count--) {
    dest[count] = val;
  }
  return (unsigned char *)dest;
}

unsigned char *memsetw(unsigned short *dest, unsigned short val, int count) {
  while(count--) {
    dest[count] = val;
  }
  return (unsigned char *)dest;
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
