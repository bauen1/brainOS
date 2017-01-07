#include "tty.h"
#include "system.h"
#include "basic_font.h"

static void memsetw(uint16_t * destination, uint16_t v, size_t num) {
  uint16_t * d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }
}

uint8_t * video_memory;

uint8_t x, y;

static uint32_t width, height, pitch, bytes_per_pixel;
static uint32_t char_width, char_height;

// TODO: parse framebuffer_red_mask_size etc
/*static inline*/ void putpixel_16m(uint32_t x, uint32_t y, uint32_t color) {
  uint8_t * dest = video_memory + x * bytes_per_pixel + y * pitch;
  dest[0] = color & 0xFF;
  dest[1] = (color >> 8) & 0xFF;
  dest[2] = (color >> 16) & 0xFF;
}

static inline void put_rect_fill(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint32_t w, uint32_t h) {
  uint8_t * dest = video_memory + x * bytes_per_pixel + y * pitch;
  for (uint32_t i = 0; i < h; i++) {
    for (uint32_t j = 0; j < w; j++) {
      dest[j * bytes_per_pixel] = r;
      dest[j * bytes_per_pixel + 1] = g;
      dest[j * bytes_per_pixel + 2] = b;
    }
    dest += pitch;
  }
}

static void put_v_at(unsigned char c, uint8_t x, uint8_t y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (font8x8_basic[c][i] & (1 << j)) {
        putpixel_16m(x * 8 + j, y * 8 + i, 0x00);
      }
    }
  }
}

void tty_init(uintptr_t framebuffer_addr, uint8_t framebuffer_bpp, uint32_t framebuffer_width, uint32_t framebuffer_height, uint32_t framebuffer_pitch, uint8_t framebuffer_type) {
  video_memory = (uint8_t *)framebuffer_addr;
  x = 0;
  y = 0;
  bytes_per_pixel = framebuffer_bpp / 8;
  width = framebuffer_width;
  char_width = width / 8;
  height = framebuffer_height;
  char_height = height / 8;
  pitch = framebuffer_pitch;
  tty_clear();
  kprintf("framebuffer:\n"
    "addr:    0x%x\n"
    "pitch:   0x%x\n"
    "width:   0x%x\n"
    "height:  0x%x\n"
    "bpp:     0x%x\n"
    "type:    %d\n",
    framebuffer_addr,
    framebuffer_pitch,
    framebuffer_width,
    framebuffer_height,
    framebuffer_bpp,
    (framebuffer_type & 0xFF)
  );
}

inline void tty_clear() {
  put_rect_fill(0, 0, 255, 255, 255, width, height);
  x = 0;
  y = 0;
}

static void scroll() {
  if (y >= char_height) {
    // TODO: implement
    tty_clear();
  }
}

inline void tty_putc(char c) {
  if (c == 0x08) { // backspace
    if (x != 0) {
      x--;
    }
  } else if (c == 0x09) { // tab
    x = (x + 8) & ~(8 - 1);
    if (x >= char_width) {
      x = 0;
      y++;
    }
  } else if (c == '\r') { // carriage return
    x = 0;
  } else if (c == '\n') { // newline
    x = 0; // also assume a carriage return
    y++;
  } else if (c >= ' ') { // printable
    if (x >= char_width) {
      x = 0;
      y++;
    }
    put_v_at(c, x, y);
    x++;
  }

  scroll();
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
