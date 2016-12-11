#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "system.h"
#include "tty.h"
#include "gdt.h"

extern void halt();

void kmain () {
  tty_init();
  puts("Hello World !\n :D \n");

  gdt_init();

  puts("GDT up and running !\n");
  puts("Free Cake for everyone!\n");

  halt();
}
