#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include <stdint.h>

// WARNING: this is x86 specific atm

#define interrupts_disable() __asm__ __volatile__ ("cli")
#define interrupts_enable() __asm__ __volatile__ ("sti")

uint8_t inportb (uint16_t port);
void outportb (uint16_t port, uint8_t data);
uint16_t inports (uint16_t port);
void outports (uint16_t port, uint16_t data);
uint32_t inportl (uint16_t port);
void outportl (uint16_t port, uint32_t data);

__attribute__((noreturn)) void halt();

#endif
