#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

void *memcpy(void *dest, void *src, int count);
unsigned char *memset(unsigned char *dest, unsigned char val, int count);
unsigned char *memsetw(unsigned short *dest, unsigned short val, int count);
int strlen(char *str);

unsigned char inportb (unsigned short port);
void outportb (unsigned short port, unsigned char data);

void putc (char c);
void puts (char *str);

#endif
