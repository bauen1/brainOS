#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

//#define NULL ((void*)0)

typedef uint32_t size_t;

void * memchr(const void * ptr, int value, size_t num);
int memcmp(const void * ptr1, const void * ptr2, size_t num);
void * memcpy(void * destination, const void * source, size_t num);
void * memmove(void * destination, const void * source, size_t num);
void * memset(void * ptr, int value, size_t num);

size_t strlen(const char * str);

char * strcpy(char * destination, const char * source);
char * strncpy(char * destination, const char * source, size_t num);
char * strcat(char * destination, const char * source);
char * strncat(char * destination, const char * source, size_t num);
int strcmp(const char * str1, const char * str2);
int strncmp(const char * str1, const char * str2, size_t num);

// TODO: implement:
// strcspn
// strpbrk
// strrchr
// strstr
// strtok

#define strerror(i) ("strerror not supported!")

#endif
