#ifndef TTY_H
#define TTY_H

#include "system.h"

void tty_init(uintptr_t framebuffer_addr, uint8_t framebuffer_bpp, uint32_t framebuffer_width, uint32_t framebuffer_height, uint32_t framebuffer_pitch, uint8_t framebuffer_type);
void tty_clear();
void tty_putc(char c);

uint8_t tty_get_cursor_x();
uint8_t tty_get_cursor_y();
void tty_set_cursor_x(uint8_t _x);
void tty_set_cursor_y(uint8_t _y);

#endif
