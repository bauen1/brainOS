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

static color_t fg, bg;

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
      dest[j * bytes_per_pixel] = b;
      dest[j * bytes_per_pixel + 1] = g;
      dest[j * bytes_per_pixel + 2] = r;
    }
    dest += pitch;
  }
}

static void put_v_at(unsigned char c, uint8_t x, uint8_t y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      uint8_t * dest = video_memory + (x * 8 + j) * bytes_per_pixel + (y * 8 + i) * pitch;
      if (font8x8_basic[c][i] & (1 << j)) {
        //putpixel_16m(x * 8 + j, y * 8 + i, 0x00);
        dest[0] = fg.b;
        dest[1] = fg.g;
        dest[2] = fg.r;
      } else {
        dest[0] = bg.b;
        dest[1] = bg.g;
        dest[2] = bg.r;
      }
    }
  }
}

void tty_init(uintptr_t framebuffer_addr, uint8_t framebuffer_bpp, uint32_t framebuffer_width, uint32_t framebuffer_height, uint32_t framebuffer_pitch, uint8_t framebuffer_type) {
  fg.r = 0x00;
  fg.g = 0x00;
  fg.b = 0x00;

  bg.r = 0xFF;
  bg.g = 0xFF;
  bg.b = 0xFF;

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
}

inline void tty_clear() {
  put_rect_fill(0, 0, bg.r, bg.g, bg.b, width, height);
  x = 0;
  y = 0;
}

static void scroll() {
  if (y >= char_height) {
    uint32_t tmp1 = y - char_height;
    /*uint32_t tmp = tmp1 * 8;
    uint8_t * from = video_memory + (height - tmp) * pitch;
    uint8_t * to = video_memory + (height - tmp * 2) * pitch;

    for (uint32_t i = 0; i < tmp; i++) {
      memcpy(from + (i * pitch), to + (i * pitch), width);
    }*/

    y -= tmp1;

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

// Foreground color
inline void tty_set_foreground_color(color_t _fg) {
  fg = _fg;
}

inline color_t tty_get_foreground_color() {
  return fg;
}

// Background color
inline void tty_set_background_color(color_t _bg) {
  bg = _bg;
}

inline color_t tty_get_background_color() {
  return bg;
}
