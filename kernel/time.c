#include "time.h"

#include "system.h"
#include "idt.h"
#include <stddef.h>
#include <stdint.h>

#define PIT_CLOCK_FREQUENCY 1193180

static uint32_t time_since_installed;

static void timer_irq0(struct registers * registers) {
  time_since_installed++;
  return;
}

void time_init() {
  set_irq_handler(0, timer_irq0);

  outportb(0x43, 0x36);

  uint32_t divisor = PIT_CLOCK_FREQUENCY / 50;//= PIT_CLOCK_FREQUENCY / 1000000; // 1Hz

  outportb(0x40, (uint8_t)(divisor & 0xF));
  outportb(0x40, (uint8_t)((divisor >> 8) & 0xF));
}

uint32_t get_time() {
  return (uint32_t)time_since_installed;
}
