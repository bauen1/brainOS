#include "system.h"
#include "string.h"
#include "tty.h"
#include <stdarg.h>

void putc (char c) {
  tty_putc(c);
}

int puts (char * str) {
  size_t i = 0;
  while(str[i] != 0) {
    putc(str[i++]);
  }

  return i;
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

void _puthex_8(uint8_t v) {
  putc(get_hex_high(v));
  putc(get_hex_low(v));
}

void vkprintf(const char * format, va_list args) {
  char buf[256];
  memset((void *)buf, 0, 256);

  while (* format != 0) {
    if (* format == '%') {
      format++;
      if (* format == 0) {
        break;
      }
      int temp = 0;
      char * temp2 = 0;
      int temp3 = 0;
      switch (* format) {
        case 'x':
          temp3 = va_arg(args, int);
          //itoa(temp3, (char *)&buf[0], 16);
          //puts(&buf[0]);
          putc(get_hex_high(temp3 >> 24));
          putc(get_hex_low(temp3  >> 24));
          putc(get_hex_high(temp3 >> 16));
          putc(get_hex_low(temp3  >> 16));
          putc(get_hex_high(temp3 >> 8));
          putc(get_hex_low(temp3  >> 8));
          putc(get_hex_high(temp3 >> 0));
          putc(get_hex_low(temp3  >> 0));
          break;
        case 'd':
          temp3 = va_arg(args, int);
          itoa(temp3, buf, 10);
          puts(buf);
        case 's':
          temp2 = va_arg(args, char *);
          puts((char *)temp2);
          break;
        case 'c':
          temp = va_arg(args, int);
          putc(temp);
          break;
        default:
          break;
      }
    } else {
      putc(* format);
    }
    format++;
  }
  return;
}

void kprintf(const char * format, ...) {
  va_list args;
  va_start(args, format);
  vkprintf(format, args);
  va_end(args);
  return;
}
