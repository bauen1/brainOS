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
#include "gdt.h"
#include "keyboard.h"

int kmain (multiboot_t *mboot, uint32_t stack_size, uintptr_t esp) {
  tty_init();
  tty_set_attribute(get_attribute(VGA_COLOR_WHITE, VGA_COLOR_CYAN));
  puts("+------------------------------------------------------------------------------+\n");
  puts("| brainOS v0.1 MIT Licence 2016 bauen1                                         |\n");
  puts("+------------------------------------------------------------------------------+\n");
  tty_set_attribute(get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  putc('\n');
  puts("boot information:\n");
  puthex("stack size:         ", stack_size);
  puthex("esp:                ", esp);

  putc('\n');
  puthex("mboot->flags:       ", mboot->flags);
  putc('\n');

  char buf[64];
  memset((void*)buf, 0, 20);
  itoa(mboot->mem_upper - mboot->mem_lower, (char*)&buf, 10);
  puts("calculated memory (from mboot->mem_upper - mboot->mem_lower):    ");
  puts(&buf[0]);
  puts("kb\n");

  gdt_init(esp);
  idt_install();
  keyboard_install();
  char *buf1 = "tesa";
  char *buf2 = "tesb";
  putc('\n');
  _puthex_8((uint8_t)memcmp((void*)buf1,(void*)buf2,5));

  __asm__ __volatile__("sti");  // enable interrupts

  for(;;){
    __asm__ __volatile__ ("hlt");
  }

  return 1;
}
