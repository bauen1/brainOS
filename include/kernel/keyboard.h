#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "string.h"

void keyboard_install();

char getc();
char * gets(char * str);
char * getsn(char * str, size_t max);

#endif
