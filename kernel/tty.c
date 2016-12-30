#include "tty.h"
#include "system.h"
#include "vga.h"
#include "basic_font.h"

static void memsetw(uint16_t * destination, uint16_t v, size_t num) {
  uint16_t * d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }
}

uint8_t * video_memory;
uint8_t attribute = get_attribute(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

uint8_t x, y;

static uint32_t width, height, pitch, bytes_per_pixel;
static uint32_t char_width, char_height;

// TODO: parse framebuffer_red_mask_size etc
static inline void putpixel_16m(uint32_t x, uint32_t y, uint32_t color) {
  uint8_t * dest = video_memory + x * bytes_per_pixel + y * pitch;
  dest[0] = color & 0xFF;
  dest[1] = (color >> 8) & 0xFF;
  dest[2] = (color >> 16) & 0xFF;
}

static inline void put_rect_fill(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint32_t w, uint32_t h) {
  uint8_t * dest = video_memory + x * bytes_per_pixel + y * pitch;
  for (uint32_t i = 0; i < w; i++) {
    for (uint32_t j = 0; j < h; j++) {
      dest[j * bytes_per_pixel] = r;
      dest[j * bytes_per_pixel + 1] = g;
      dest[j * bytes_per_pixel + 2] = b;
    }
    dest += pitch;
  }
}

static void put_v_at(unsigned char c, uint8_t attribute, uint8_t x, uint8_t y) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (font8x8_basic[c][i] & (1 << j)) {
        putpixel_16m(x * 8 + j, y * 8 + i, 0x00);
      }
    }
  }
}

void tty_init(struct multiboot_info * mbi) {
  video_memory = (uint8_t *)(uint32_t)mbi->framebuffer_addr;
  x = 0;
  y = 0;
  bytes_per_pixel = mbi->framebuffer_bpp / 8;
  width = mbi->framebuffer_width;
  char_width = width / 8;
  height = mbi->framebuffer_height;
  char_height = height / 8;
  pitch = mbi->framebuffer_pitch;
  tty_clear();
  puthex("framebuffer_addr:     ", mbi->framebuffer_addr);
  puthex("framebuffer_pitch:    ", mbi->framebuffer_pitch);
  puthex("framebuffer_width:    ", mbi->framebuffer_width);
  puthex("framebuffer_height:   ", mbi->framebuffer_height);
  puthex("framebuffer_bpp:      ", mbi->framebuffer_bpp);
  puthex("framebuffer_type:     ", mbi->framebuffer_type & 0xFF);
  puthex("char_width:           ", char_width);
}

inline void tty_clear() {
  put_rect_fill(0, 0, 255, 255, 255, height, width);
  x = 0;
  y = 0;
}

static void scroll() {
  if (y >= char_height) {
    uint32_t tmp = y - char_height + 1;
    tmp *= 8;
    // TODO: copy everything 1 row up and fill the last row again
    for (uint32_t y = tmp; y < height; y++) {
      memcpy((void *)(video_memory + y * pitch), (void *)(video_memory + (y - tmp) * pitch), width * bytes_per_pixel);
    }
    put_rect_fill(0, height - tmp, 255, 255, 255, tmp, width);
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
    put_v_at(c, attribute, x, y);
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

inline void tty_set_attribute(uint8_t v) {
  attribute = v;
}

inline uint8_t tty_get_attribute() {
  return attribute;
}
