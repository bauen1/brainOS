#include "keyboard.h"

#include "ringbuffer.h"
#include "string.h"
#include "system.h"
#include "idt.h"
#include "tty.h"

static unsigned char keyboard_map[128] = {
  0, 27,
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=',
  '\b', // backspace
  '\t', // tab
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p',
  '[', ']', '\n', // newline
  0, // control
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
  '\'', '`', 0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n',
  'm', ',', '.', '/', 0,
  '*',
  0, // alt
  ' ', // space
  0, // caps lock
  0, // f1
  0, 0, 0, 0, 0, 0, 0, 0,
  0, // f10
  0, // numlock
  0, // scroll lock
  0, // home key
  0, // up arrow
  0, // page up
  '-',
  0, // left arrow
  0,
  0, // right arrow
  '+',
  0, // end key
  0, // down arrow
  0, // page down
  0, // insert key
  0, // delete key
  0, 0, 0,
  0, // f11
  0, // f12
  0, // TODO: implement the rest of em
  // 92:
  0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,

};

#define KEYBOARD_BUFFER_SIZE 512

static uint8_t _keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static buffer_t keyboard_buffer = { .head = 0, .tail = 0, .size = KEYBOARD_BUFFER_SIZE, .buffer = _keyboard_buffer};


char getc() {
  char c;
  for (;;) {
    c = buffer_read(&keyboard_buffer);
    if (c == -1) {
      __asm__ __volatile__ ("hlt"); // wait a bit
    } else {
      return c;
    }
  }
}

char * getsn(char * str, size_t max) {
  size_t i = 0;
  while (i < max) {
    uint8_t px = tty_get_cursor_x();
    char c = getc();
    if (c == '\n') {
      str[i] = 0;
      return str;
    } else if (c == '\b') { // FIXME: hacky
      if (i > 0) {
        putc(' ');
        putc('\b'); // clear the "delted" char on screen FIXME: hacky
        i = i - 1; // also account for the i++ at the end of the loop
      } else {
        uint8_t x = tty_get_cursor_x();
        if (x < px) {
          tty_set_cursor_x(px);
        }
      }
    } else {
      str[i] = c;
      i++;
    }
  }

  return NULL;
}

// This is the best way to create a stack smash vulnerability
char * gets(char * str) {
  return getsn(str, SIZE_MAX);
}

static void keyboard_irq1(struct registers * registers) {
  uint8_t status;
  char keycode;

  status = inportb(0x64);
  if (status & 0x01) {
    keycode = inportb(0x60);
    if (keycode < 0) {
      return;
    }

    char c = (char)keyboard_map[(uint8_t)keycode];
    buffer_write(&keyboard_buffer, c);
    putc(c);
  }
  return;
}

void keyboard_install() {
  set_irq_handler(1, (isr_t)&keyboard_irq1); // install the keyboard irq handler
}
