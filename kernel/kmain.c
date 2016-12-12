#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "multiboot.h"
#include "system.h"
#include "tty.h"
//#include "gdt.h"

extern void halt();

int kmain (multiboot_t *multiboot_info, uintptr_t esp) {
  tty_init();
  puts("Hello World !\n :D \n");
  char buffer[1024];
  itoa(multiboot_info->flags, &buffer[0], 2);
  puts(&buffer[0]);
  putc('\n');
  for (int i = 0; i < 33; i++) {
    putc(&multiboot_info+i);
  }
  putc('\n');

  //gdt_init();

  puts("Nothing has blown up yet \\o/\n");

  return 1;
}
