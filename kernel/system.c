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
  // Check for supported base.
  if ( base < 2 || base > 36 )
  {
    * str = '\0';
  } else {
    char * ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
      * ptr++ = '-';
    }
    // Remember where the numbers start.
    char * low = ptr;
    // The actual conversion.
    do
    {
      // Modulo is negative for negative value. This trick makes abs() unnecessary.
      * ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
      value /= base;
    } while ( value );
    // Terminating the string.
    * ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
      char tmp = * low;
      * low++ = * ptr;
      * ptr-- = tmp;
    }
  }

  return str;
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

#define _vkprintf_putc(v) putc((v)); len++

size_t vkprintf(const char * format, va_list args) {
  size_t len = 0;
  char buf[256];
  memset((void *)buf, 0, 256);

  size_t format_len = strlen(format);

  for (size_t i = 0; i < format_len; i++) {
    if (format[i] == '%') {
      i++;
      if (i >= format_len) {
        break;
      }

      int tmp = 0;
      char * tmp2 = 0;

      switch(format[i]) {
        case 'x':
          tmp = va_arg(args, int);
          itoa(tmp, buf, 16);
          len += puts(buf);
          break;
        case 'u': // unsigned int
          tmp = va_arg(args, int);
          tmp = (i < 0 ? -i : i); // basic abs() implementation
          itoa(tmp, buf, 10);
          len += puts(buf);
          break;
        case 'o': // i don't really know why anyone would need this but hey i implemented it anyway :P
          tmp = va_arg(args, int);
          itoa(tmp, buf, 8);
          len += puts(buf);
          break;
        case 'd':
          tmp = va_arg(args, int);
          itoa(tmp, buf, 10);
          len += puts(buf);
          break;
        case 'c':
          tmp = va_arg(args, int);
          _vkprintf_putc(tmp);
          break;
        case 's':
          tmp2 = va_arg(args, char *);
          len += puts(tmp2);
          break;
        case '%':
          _vkprintf_putc('%');
          break;
        default:
          _vkprintf_putc(format[i]);
          break;
      }
    } else {
      _vkprintf_putc(format[i]);
    }
  }

  return len;
}

__attribute__((format(__printf__, 1, 2))) void kprintf(const char * format, ...) {
  va_list args;
  va_start(args, format);
  vkprintf(format, args);
  va_end(args);
  return;
}
