#ifndef TTY_H
#define TTY_H

#include "system.h"

typedef struct color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} color_t;

void tty_init(uintptr_t framebuffer_addr, uint8_t framebuffer_bpp, uint32_t framebuffer_width, uint32_t framebuffer_height, uint32_t framebuffer_pitch, uint8_t framebuffer_type);
void tty_clear();
void tty_putc(char c);

uint8_t tty_get_cursor_x();
uint8_t tty_get_cursor_y();
void tty_set_cursor_x(uint8_t _x);
void tty_set_cursor_y(uint8_t _y);

// Foreground color
void tty_set_foreground_color(color_t _fg);
color_t tty_get_foreground_color();

// Background color
void tty_set_background_color(color_t _bg);
color_t tty_get_background_color();


#endif
