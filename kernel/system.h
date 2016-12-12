#ifndef SYSTEM_H
#define SYSTEM_H

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *destination, const void *source, size_t num);
void *memset(void *destination, int c, size_t num);
unsigned short *memsetw(unsigned short *destination, unsigned short v, size_t num);
int strlen(char *str);

unsigned char inportb (unsigned short port);
void outportb (unsigned short port, unsigned char data);

void putc (char c);
void puts (char *str);

char * itoa( int value, char * str, int base );

#endif
