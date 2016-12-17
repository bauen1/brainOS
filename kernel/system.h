#ifndef SYSTEM_H
#define SYSTEM_H

#include "string.h"

#include <stddef.h>
#include <stdint.h>

uint8_t inportb (uint16_t port);
void outportb (uint16_t port, uint8_t data);
uint16_t inports (uint16_t port);
void outports (uint16_t port, uint16_t data);
uint32_t inportl (uint16_t port);
void outportl (uint16_t port, uint32_t data);

void putc (char c);
void puts (char *str);

char * itoa( int value, char * str, int base );

void puthex(char * name, uint32_t v);

#endif
