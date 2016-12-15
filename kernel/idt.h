#ifndef IDT_H
#define IDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct idt_entry {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t zero;
  uint8_t type_attr;
  uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

void idt_install();

#endif
