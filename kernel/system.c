#include "system.h"
#include "tty.h"

void *memcpy(void *destination, const void *source, size_t num) {
  char* d = destination;
  char* s = (void*)source;
  for (size_t i = 0; i < num; i++) {
    d[i] = s[i];
  }

  return destination;
}

void *memset(void *destination, int c, size_t num) {
  char* d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = c;
  }

  return destination;
}

unsigned short *memsetw(unsigned short *destination, unsigned short v, size_t num) {
  unsigned short* d = destination;
  for (size_t i = 0; i < num; i++) {
    d[i] = v;
  }

  return destination;
}

int strlen(char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {}
  return i;
}

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
