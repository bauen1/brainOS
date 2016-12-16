#include "system.h"
#include "string.h"
#include "tty.h"

unsigned char inportb (unsigned short port) {
  unsigned char rv;
  __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
  return rv;
}

void outportb (unsigned short port, unsigned char data) {
  __asm__ __volatile__ ("outb %1, %0" : : "dN" (port), "a" (data));
}

void putc (char c) {
  tty_putc(c);
}

void puts (char *str) {
  for (int i = 0; i < strlen(str); i++) {
    putc(str[i]);
  }
}

// http://wiki.osdev.org/Printing_To_Screen#Printing_Integers
char * itoa( int value, char * str, int base )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}

static char get_hex_low(uint8_t v) {
  return "0123456789abcdef"[v & 0x0f];
}

static char get_hex_high(uint8_t v) {
  return "0123456789abcdef"[(v >> 4) & 0x0f];
}

void puthex(char * name, uint32_t v) {
  puts(name);
  puts("0x");
  putc(get_hex_high(v >> 24));
  putc(get_hex_low(v  >> 24));
  putc(get_hex_high(v >> 16));
  putc(get_hex_low(v  >> 16));
  putc(get_hex_high(v >> 8));
  putc(get_hex_low(v  >> 8));
  putc(get_hex_high(v >> 0));
  putc(get_hex_low(v  >> 0));
  putc('\n');
}