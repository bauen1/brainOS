#ifndef TIME_H
#define TIME_H

#include <stddef.h>
#include <stdint.h>

#define PIT_CLOCK_FREQUENCY 1193180

void time_init(uint32_t frequency);

void _wait(uint32_t t);

uint32_t get_time();

#endif
