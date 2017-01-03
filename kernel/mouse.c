#include "mouse.h"
#include "idt.h"
#include "system.h"

static uint32_t x, y;

static inline void handle_mouse_move(uint8_t data[3]) {
  //puthex("data[0]: ", data[0]);
  //puthex("data[1]: ", data[1]);
  //puthex("data[2]: ", data[2]);
  signed long dx = data[1];
  signed long dy = data[2];


  if (data[0] & 0x20) {
    dy |= 0xFFFFFF00;
  }

  if (data[0] & 0x10) {
    dx |= 0xFFFFFF00;
  }

  /*
  puthex("dx: ", dx);
  puthex("dy: ", dy);
  puthex("x:  ", x);
  puthex("y:  ", y);
  */

  x += dx;
  y -= dy; // Invert Y

  if ((data[0] & 0x01) == 0) {
    putpixel_16m(x, y, 0x11679A);
  } else {
    putpixel_16m(x, y, 0xFF0000);
  }
}

static uint8_t mouse_i = 0;
static uint8_t mouse_data[3];

static void mouse_irq12(struct registers * registers) {
  uint8_t status = inportb(0x64);

  if (status & 0x01) {
    if (status & 0x20) { // There is a mouse !!
      switch(mouse_i) {
        case 0:
          mouse_data[0] = inportb(0x60);
          mouse_i++;
          break;
        case 1:
          mouse_data[1] = inportb(0x60);
          mouse_i++;
          //break; // FIXME: For some weird reason it works
        case 3:
          mouse_data[2] = inportb(0x60);
          handle_mouse_move(mouse_data);
          mouse_i = 0;
          break;
      }
    }
  }
}

static inline void mouse_wait(uint8_t type) {
  uint32_t time_out = 100000;
  if (type == 0) {
    while(time_out > 0) {
      if((inportb(0x64) & 1) == 1) {
        return;
      }
      time_out--;
    }
    return;
  } else {
    while(time_out > 0) {
      if((inportb(0x64) & 2) == 1) {
        return;
      }
      time_out--;
    }
    return;
  }
  return;
}

static inline void mouse_write(uint8_t v) {
  mouse_wait(1);
  outportb(0x64,0xD4);
  mouse_wait(1);
  outportb(0x60, v);
}

static uint8_t mouse_read() {
  mouse_wait(0);
  return inportb(0x60);
}

void mouse_init() {
  set_irq_handler(12, (isr_t)&mouse_irq12);

  uint8_t status = 0;
  mouse_wait(1);

  outportb(0x64, 0x20);
  mouse_wait(0);

  status = inportb(0x60);
  status |= 2;
  mouse_wait(1);
  outportb(0x64, 0x60);
  mouse_wait(1);
  outportb(0x60, status);

  mouse_write(0xF6); // default settings
  mouse_read(); // acknowledge

  // enable the mouse
  mouse_write(0xF4);
  mouse_read(); // acknowledge
}
