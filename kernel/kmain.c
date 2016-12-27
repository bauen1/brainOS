#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "vga.h"
#include "tty.h"
#include "idt.h"
#include "time.h"
#include "gdt.h"
#include "keyboard.h"
#include "pci.h"
#include "pmm.h"
#include "vmm.h"

const char* exception[] = {
  "Division by Zero",
  "Debug Exception",
  "NMI",
  "Breakpoint",
  "into detected overflow",
  "Out of Bounds",
  "Invalid Opcode",
  "No coprocessor",
  "Double Fault",
  "coprocessor segment overrun",
  "Bad TSS",
  "Segment not present",
  "Stack fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown interrupt exception",
  "coprocessor fault",
  "alignment check exception",
  "machine check exception",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
  "Reserved",
};

extern uint32_t end;
extern uint32_t start;

void kpanic (struct registers * registers) {
  // TODO: Make this more epic, blue background etc everyone loves a good old BSOD
  tty_set_attribute(get_attribute(VGA_COLOR_WHITE, VGA_COLOR_BLUE));
  tty_clear();
  puts("KERNEL PANIC:\n");
  puts((char *)exception[registers->isr_num]);
  putc('\n');
  puthex("ds:       ", registers->ds);
  puthex("edi:      ", registers->edi);
  puthex("esi:      ", registers->esi);
  puthex("ebp:      ", registers->ebp);
  puthex("esp:      ", registers->esp);
  puthex("ebx:      ", registers->ebx);
  puthex("edx:      ", registers->edx);
  puthex("ecx:      ", registers->ecx);
  puthex("eax:      ", registers->eax);
  puthex("isr_num:  ", registers->isr_num);
  puthex("err_code: ", registers->err_code);
  puthex("eip:      ", registers->eip);
  puthex("cs:       ", registers->cs);
  puthex("eflags:   ", registers->eflags);
  puthex("useresp:  ", registers->useresp);
  puthex("ss:       ", registers->ss);

  __asm__ __volatile__ ("cli");
  for(;;){
    __asm__ __volatile__ ("hlt");
  }
}

int kmain (multiboot_info_t * mbi, uint32_t stack_size, uintptr_t esp) {
  // TODO: implement printf, this is a mess
  // TODO: really need to do the above
  gdt_init();
  idt_install();

  tty_init();
  tty_set_attribute(get_attribute(VGA_COLOR_WHITE, VGA_COLOR_CYAN));
  puts("+------------------------------------------------------------------------------+\n");
  puts("| brainOS v0.1 MIT Licence 2016 bauen1                                         |\n");
  puts("+------------------------------------------------------------------------------+\n");
  tty_set_attribute(get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  putc('\n');
  puts("boot information:\n");
  puthex("mboot->flags:       ", mbi->flags);
  puthex("stack size:         ", stack_size);
  puthex("esp:                ", esp);
  puthex("modules count:      ", mbi->mods_count);
  puthex("memmap size:        ", mbi->mmap_length);
  putc('\n');

  char buf[64];
  memset((void *)buf, 0, 20);
  itoa(mbi->mem_upper + mbi->mem_lower, (char *)&buf, 10);
  puts("calculated memory (from mboot->mem_upper + mboot->mem_lower):    ");
  puts(&buf[0]);
  puts("kb\n");

  keyboard_install();
  pmm_init(mbi->mem_upper * 1024, (uint32_t)&end);
  time_init(1); // setup the clock to fire every 18.222 times per second (default)
  pci_install();

  for (int i = 0; i < 32; i++) {
    set_isr_handler(i, kpanic);
  }

  putc('\n');
  puts("Parsing Memory Map:\n");
  multiboot_memory_map_t *mmap;
  int region = 0;
  for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
       (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
       mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                + mmap->size + sizeof (mmap->size))) {
    puts("Region 0x");
    _puthex_8(region);
    puts(": start: ");
    _puthex_8(mmap->addr>>24);
    _puthex_8(mmap->addr>>16);
    _puthex_8(mmap->addr>>8);
    _puthex_8(mmap->addr);
    puts(" length: ");
    _puthex_8(mmap->len>>24);
    _puthex_8(mmap->len>>16);
    _puthex_8(mmap->len>>8);
    _puthex_8(mmap->len);
    puts(" type: ");
    _puthex_8(mmap->type);
    putc('\n');

    if (mmap->type == 1) {
      pmm_free_region(mmap->addr, mmap->len);
    } else {
      pmm_alloc_region(mmap->addr, mmap->len);
    }

    region++;
  }
  // The length of the kernel from start to end
  uint32_t kernel_length = (((uint32_t)&end) - ((uint32_t)&start));

  // memory below 1 mb is for special purpose (virtual 8086 mode)
  pmm_alloc_region(0x00000000, 0x00100000);

  // lets just say we don't want to "allocate" the space where our code lives
  pmm_alloc_region((uint32_t)&start, kernel_length);
  puthex("kernel_length:      ", kernel_length);
  puthex("(uint32_t)&start:   ", (uint32_t)&start);
  puthex("(uint32_t)&end:     ", (uint32_t)&end);

  // Physical Memory Manager Test:
  // pmm_used_blocks should be the same on both prints
  puthex("pmm_used_blocks: ", pmm_get_pmm_used_blocks());

  void * p = pmm_alloc_block();
  puthex("p:   ", (uint32_t)p);
  void * p2 = pmm_alloc_blocks(2);
  puthex("p2:  ", (uint32_t)p2);
  void * p3 = pmm_alloc_block();
  puthex("p3:  ", (uint32_t)p2);
  pmm_free_block(p3);
  puts("reallocating p\n");
  pmm_free_block(p);
  p = pmm_alloc_block();
  puthex("p3:  ", (uint32_t)p);
  pmm_free_block(p);
  pmm_free_blocks(p2, 2);
  puthex("pmm_used_blocks: ", pmm_get_pmm_used_blocks());

  __asm__ __volatile__("sti");  // enable interrupts

  vmm_init();

  char buffer[1024];
  while (true) {
    putc('>');
    getsn(&buffer[0], 1024);
    puts("You typed: ");
    puts(buffer);
    puts("\n");
    if (strncmp(buffer, "modules", 7) == 0) {
      puthex("number of modules: ", (uint32_t)mbi->mods_count);
    } else if (strncmp(buffer, "wait", 4) == 0) {
      puts("Waiting for 10 seconds\n");
      _wait(2 * 18.22 * 10);
    } else if (strncmp(buffer, "listpci", 7) == 0) {
      pci_list();
    }
  }

  return 1;
}
