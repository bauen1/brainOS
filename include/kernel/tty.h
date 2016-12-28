#ifndef TTY_H
#define TTY_H

#include "system.h"
#include "multiboot.h"

void tty_init(struct multiboot_info * mbi);
void tty_clear();
void tty_putc(char c);

void tty_set_attribute(uint8_t v);
uint8_t tty_get_attribute();
uint8_t tty_get_cursor_x();
uint8_t tty_get_cursor_y();
void tty_set_cursor_x(uint8_t _x);
void tty_set_cursor_y(uint8_t _y);

#endif
