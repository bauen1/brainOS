#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include <stddef.h>
#include <stdint.h>

uint16_t* terminal_buffer;

void kmain () {

  terminal_buffer = (uint16_t*)0xB8000;
  terminal_buffer[0]=0x2f4f;
  terminal_buffer[1]=0x2f4b;
}
