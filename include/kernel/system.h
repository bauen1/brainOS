#ifndef SYSTEM_H
#define SYSTEM_H

#include "string.h"
#include "cpu.h"
#include <stddef.h>
#include <stdint.h>

void putc (char c);
int puts (char *str);

char * itoa( int value, char * str, int base );

void puthex(char * name, uint32_t v);
void _puthex_8(uint8_t v);

void kprintf(const char * format, ...);

// Because some people are lazy
#define printf(...) kprintf(__VA_ARGS__)

#endif
