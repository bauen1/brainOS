#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

#include "system.h"
#include "tty.h"

extern void halt();

void kmain () {
  tty_init();
  tty_putc('H');
  tty_putc('E');
  tty_putc('L');
  tty_putc('L');
  tty_putc('O');
  tty_putc(' ');
  tty_putc('W');
  tty_putc('O');
  tty_putc('R');
  tty_putc('L');
  tty_putc('D');
  tty_putc('!');
  tty_putc('\n');
  
  gdt_install();

  tty_putc('!');
  tty_putc('\n');
  halt();
}
