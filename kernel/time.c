// TODO: rename to pit.*
#include "time.h"

#include "system.h"
#include "idt.h"
#include <stddef.h>
#include <stdint.h>

static volatile uint32_t ticks;

static void timer_irq0(struct registers * registers) {
  ticks++;
  if ((ticks % 18) == 0) {
    //puts("about 1 second passed.\n");
  }
  return;
}

void time_init(uint32_t frequency) {
  ticks = 0;
  uint16_t divisor = PIT_CLOCK_FREQUENCY / frequency; // uint32_t divisor = PIT_CLOCK_FREQUENCY / 50;//= PIT_CLOCK_FREQUENCY / 1000000; // 1Hz

  outportb(0x43, 0x36);
  outportb(0x40, (uint8_t)(divisor & 0xFF));
  outportb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
  set_irq_handler(0, timer_irq0);
}

void _wait(uint32_t t) {
  uint32_t target_ticks = ticks + t;
  while (ticks < target_ticks) {
    __asm__ __volatile__ ("hlt");
  }
}

uint32_t get_time() {
  return ticks;
}
