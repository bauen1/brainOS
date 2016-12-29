#include "tty.h"
#include "system.h"
#include "vga.h"

static void memsetw(uint16_t * destination, uint16_t v, size_t num) {
  uint16_t * d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }
}

uint16_t * video_memory;
uint8_t attribute = get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

uint8_t x, y;

static void put_v_at(unsigned char c, uint8_t attribute, uint8_t x, uint8_t y) {
  video_memory[y * VGA_WIDTH + x] = get_vga_v(c, attribute);
}

void tty_init(struct multiboot_info * mbi) {
  video_memory = (uint16_t *)(uint32_t)mbi->framebuffer_addr;
  x = 0;
  y = 0;
  tty_clear();
}

void tty_clear() {
  for (int i = 0; i < VGA_HEIGHT; i++) {
    memsetw((short unsigned int *)(video_memory + i * VGA_WIDTH), get_vga_v(' ', attribute), VGA_WIDTH);
  }
}

static void scroll() {
  unsigned char tmp;
  if (y >= VGA_HEIGHT) {
    tmp = y - VGA_HEIGHT + 1;
    memcpy(video_memory, video_memory + (tmp * VGA_WIDTH), (VGA_HEIGHT - tmp) * VGA_WIDTH * 2);

    memsetw((short unsigned int *)(video_memory + (VGA_HEIGHT - tmp) * VGA_WIDTH), get_vga_v(' ', attribute), VGA_WIDTH);
    y = VGA_HEIGHT - 1;
  }
}

inline void tty_putc(char c) {
  if (c == 0x08) { // backspace
    if (x != 0) {
      x--;
    }
  } else if (c == 0x09) { // tab
    x = (x + 8) & ~(8 - 1);
  } else if (c == '\r') {
    x = 0;
  } else if (c == '\n') {
    x = 0;
    y++;
  } else if (c >= ' ') {
    // printable
    put_v_at(c, attribute, x, y);
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

}

inline uint8_t tty_get_cursor_x() {
  return x;
}

inline uint8_t tty_get_cursor_y() {
  return y;
}

inline void tty_set_cursor_x(uint8_t _x) {
  x = _x;
}

inline void tty_set_cursor_y(uint8_t _y) {
  y = _y;
}

inline void tty_set_attribute(uint8_t v) {
  attribute = v;
}

inline uint8_t tty_get_attribute() {
  return attribute;
}
