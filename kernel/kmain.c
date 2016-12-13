#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "tty.h"
#include "gdt.h"

int kmain (multiboot_t *multiboot_info, uintptr_t esp) {
  tty_init();
  puthex("stack size: ", stack_size);
  puthex("esp:        ", esp);

  puts("Hello World !\n :D \n");
  puts("multiboot_info flags:");
  char buffer[1024];
  itoa(multiboot_info->flags, &buffer[0], 2);
  puts(&buffer[0]);
  putc('\n');

  puts("pre gdt_init();\n");
  gdt_init();
  puts("post gdt_init();\n");


  puts("Nothing has blown up yet \\o/\n");

  return 1;
}
