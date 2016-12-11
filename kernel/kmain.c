#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

uint16_t* terminal_buffer;



extern void halt();

void kmain () {
  halt();
}
