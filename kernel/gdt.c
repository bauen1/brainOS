#include "gdt.h"
#include "system.h"

extern void gdt_flush(uint32_t);
extern void tss_flush();

struct gdt_entry gdt_entries[6];

static void gdt_set_gate(uint32_t i, uint32_t base, uint32_t limit, uint8_t access, uint8_t garnularity) {
  gdt_entries[i].limit = limit & 0xFFFF;
  gdt_entries[i].base = base & 0xFFFF;
  gdt_entries[i].base2 = (base >> 16) & 0xFF;
  gdt_entries[i].access = access;
  gdt_entries[i].garnularity = (limit >> 16) & 0x0F;
  gdt_entries[i].garnularity |= garnularity & 0xF0;
  gdt_entries[i].base3 = (base >> 24) & 0xFF;
}

static struct {
  uint16_t size;
  uint32_t offset;
} __attribute__ ((packed)) gdt_p;

static struct gdt_tss_entry tss;

// TODO:
uint8_t kernel_stack[4096] __attribute__((aligned(0x04))); // 4kb

void gdt_set_kernel_stack(uint32_t stack) {
  tss.esp0 = stack;
}

void gdt_init() {
  memset((void*)&tss, 0x00, sizeof(tss));
  tss.ss0 = 0x10; // Kernel data segment // TODO: create a macro for figuring out the offset
  tss.esp0 = (uint32_t)&kernel_stack;
  tss.IOPB_offset = sizeof(struct gdt_tss_entry);

  gdt_set_gate(0, 0, 0xFFFFFFFF, 0x00, 0x00); // NULL segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xC0); // Kernel Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC0); // Kernel Data segment
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xF8, 0xC0); // User Code segment
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xC0); // User Data segment
  gdt_set_gate(5, (uint32_t)&tss, sizeof(struct gdt_tss_entry), 0x89, 0x40); // TSS segment

  gdt_p.offset = (uint32_t)&gdt_entries;
  gdt_p.size = sizeof(struct gdt_entry) * 5 - 1;

  puts("Flushing GDT table\n");
  gdt_flush((uint32_t)&gdt_p);
  tss_flush();
}
