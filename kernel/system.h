#ifndef SYSTEM_H
#define SYSTEM_H

#include "string.h"

#include <stddef.h>
#include <stdint.h>

unsigned char inportb (unsigned short port);
void outportb (unsigned short port, unsigned char data);

void putc (char c);
void puts (char *str);

char * itoa( int value, char * str, int base );

void puthex(char * name, uint32_t v);

#endif
