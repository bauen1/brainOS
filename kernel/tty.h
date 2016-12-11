#ifndef TTY_H
#define TTY_H

#include "system.h"

void tty_init();
void tty_putc(char c);
void tty_set_attribute(uint8_t v);
uint8_t tty_get_attribute();

#endif
