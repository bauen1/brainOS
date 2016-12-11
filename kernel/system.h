#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
unsigned char *memset(unsigned char *dest, unsigned char val, int count);
unsigned char *memsetw(unsigned short *dest, unsigned short val, int count);
int strlen(const char *str);

unsigned char inportb (unsigned short port);
void outportb (unsigned short port, unsigned char data);

void putc (unsigned char c);

#endif
