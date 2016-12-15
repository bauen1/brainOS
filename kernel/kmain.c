#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "tty.h"
#include "idt.h"
#include "gdt.h"

int kmain (multiboot_t *multiboot_info, uint32_t stack_size, uintptr_t esp) {
  tty_init();
  puts("brainOS v0.1\n");
  puts("boot information\n");
  puthex("stack size: ", stack_size);
  puthex("esp:        ", esp);

  puts("multiboot_info flags:");
  char buffer[1024];
  itoa(multiboot_info->flags, &buffer[0], 2);
  puts(&buffer[0]);
  putc('\n');

  puts("pre gdt_init();\n");
  gdt_init(esp);
  puts("post gdt_init();\n");

  puts("pre idt_install();\n");
  idt_install(esp);
  puts("post idt_install();\n");

  puts("Nothing has blown up yet \\o/\n");

  return 1;
}
