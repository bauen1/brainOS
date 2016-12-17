#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include <stdint.h>

uint8_t inportb (uint16_t port);
void outportb (uint16_t port, uint8_t data);
uint16_t inports (uint16_t port);
void outports (uint16_t port, uint16_t data);
uint32_t inportl (uint16_t port);
void outportl (uint16_t port, uint32_t data);

#endif
