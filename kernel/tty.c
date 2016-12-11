#include "tty.h"
#include "system.h"
#include "vga.h"

uint16_t *video_memory;
uint8_t attribute = get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

uint8_t x, y;

uint16_t blank;

void put_v_at(unsigned char c, uint8_t attribute, uint8_t x, uint8_t y);

void tty_init() {
  video_memory = (uint16_t*)0xb8000;
  x = 0;
  y = 0;
  blank = get_vga_v(' ', attribute);

  for (int i = 0; i < VGA_HEIGHT; i++) {
    memsetw(video_memory + i * VGA_WIDTH, blank, VGA_WIDTH);
  }

  //video_memory[0] = '!' | (attribute << 8);
  //put_v_at(':', attribute, 0, 0);
  //put_v_at('D', attribute, 1, 0);
  tty_putc(':');
  tty_putc('D');
  tty_putc('\n');
}

void move_csr() {
  unsigned int v = y * VGA_WIDTH + x;
  outportb(0x3D4, 14);
  outportb(0x3D5, v >> 8);
  outportb(0x3D4, 15);
  outportb(0x3D5, v);
}

void scroll() {
  unsigned char tmp;
  if (y >= VGA_HEIGHT) {
    tmp = y - VGA_HEIGHT + 1;
    memcpy(video_memory, video_memory + tmp * VGA_WIDTH, (VGA_HEIGHT - tmp) * VGA_HEIGHT * 2);

    memsetw(video_memory + (VGA_HEIGHT - tmp) * VGA_WIDTH, blank, VGA_WIDTH);
  }
}

void put_v_at(unsigned char c, uint8_t attribute, uint8_t x, uint8_t y) {
  const uint8_t i = y * VGA_WIDTH + x;
  video_memory[i] = get_vga_v(c, attribute);
}

void tty_putc(char c) {
  if (c == 0x08) { // backspace
    if (x != 0) {
      x--;
    }
  } else if (c == 0x09) {
    x = (x + 8) & ~(8 - 1);
  } else if (c == '\r') {
    x = 0;
  } else if (c == '\n') {
    x = 0;
    y++;
  } else if (c >= ' ') {
    // printable
    video_memory[y * VGA_WIDTH + x] = get_vga_v(c, attribute);
    x++;
  }

  if (x >= VGA_WIDTH) {
    x = 0;
    y++;
  }

  scroll();
  move_csr();
}

void tty_set_attribute(uint8_t v) {
  attribute = v;
}

uint8_t tty_get_attribute() {
  return attribute;
}
