#include "tty.h"
#include "system.h"
#include "vga.h"

static void memsetw(unsigned short *destination, unsigned short v, size_t num) {
  unsigned short* d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }
}

uint16_t *video_memory;
uint8_t attribute = get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

uint8_t x, y;

uint16_t blank;

static void put_v_at(unsigned char c, uint8_t attribute, uint8_t x, uint8_t y) {
  video_memory[y * VGA_WIDTH + x] = get_vga_v(c, attribute);
}

void tty_init() {
  video_memory = (uint16_t*)0xb8000;
  x = 0;
  y = 0;
  blank = get_vga_v(' ', attribute);
  tty_clear();
}

void tty_clear() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    memsetw((short unsigned int*)video_memory + i * VGA_WIDTH, blank, VGA_WIDTH);
  }
}

static void scroll() {
  unsigned char tmp;
  if (y >= VGA_HEIGHT) {
    tmp = y - VGA_HEIGHT + 1;
    memcpy(video_memory, video_memory + (tmp * VGA_WIDTH), (VGA_HEIGHT - tmp) * VGA_WIDTH * 2);

    memsetw((short unsigned int*)video_memory + (VGA_HEIGHT - tmp) * VGA_WIDTH, blank, VGA_WIDTH);
    y = VGA_HEIGHT - 1;
  }
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

  if (x > VGA_WIDTH) {
    x = 0;
    y++;
  }

  scroll();

  // Move the cursor
  unsigned int v = y * VGA_WIDTH + x;
  outportb(0x3D4, 14);
  outportb(0x3D5, v >> 8);
  outportb(0x3D4, 15);
  outportb(0x3D5, v);

  // Display cursor position in the top left corner

  unsigned char buf[3];
  memset(buf, 0, 3);
  itoa(x, (void*)&buf, 10);

  put_v_at('C'   , attribute, 72, 0);
  put_v_at(':'   , attribute, 73, 0);
  put_v_at(' '   , attribute, 74, 0);
  put_v_at(buf[0], attribute, 75, 0);
  put_v_at(buf[1], attribute, 76, 0);
  memset(buf, 0, 3);

  itoa(y, (void*)&buf, 10);
  put_v_at(' '   , attribute, 77, 0);
  put_v_at(buf[0], attribute, 78, 0);
  put_v_at(buf[1], attribute, 79, 0);
}

void tty_set_attribute(uint8_t v) {
  attribute = v;
}

uint8_t tty_get_attribute() {
  return attribute;
}
