#ifndef __CONIO_H__
#define __CONIO_H__

#include "vga.h"

void clrscr(void);
void itoa(char *buf, int base, int d);
uint32_t atoi(char *s);
static void putchar(int c);
static void puts(char *s);
void printf(const char *format, ...);
void scanf(const char *format, ...);

#endif // __CONIO_H__
