#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>

struct gdt_entry {
  uint16_t limit;
  uint16_t base;
  uint8_t base2;
  uint8_t access;
  uint8_t garnularity;
  uint8_t base3;
} __attribute__((packed));

struct gdt_tss_entry {
  uint32_t link;
  uint32_t esp0;
  uint32_t ss0;
  uint32_t esp1;
  uint32_t ss1;
  uint32_t esp2;
  uint32_t ss2;
  uint32_t cr3;
  uint32_t eip;
  uint32_t eflags;
  uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
  uint32_t es;
  uint32_t cs;
  uint32_t ss;
  uint32_t ds;
  uint32_t fs;
  uint32_t gs;
  uint32_t ldtr;
  uint8_t IOPB_offset;
  uint8_t reserved;
} __attribute__((packed));

void gdt_init(uint32_t esp0);

#endif
