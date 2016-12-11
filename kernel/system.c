#include "system.h"
#include "tty.h"

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count) {
  while(count--) {
    dest[count] = src[count];
  }
  return (unsigned char *)dest;
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

int strlen(const char *str) {
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

void putc (unsigned char c) {
  tty_putc(c);
}
