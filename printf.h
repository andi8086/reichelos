#ifndef __PRINTF_H__
#define __PRINTF_H__


#include "vga.h"

void clrscr(void);
void itoa (char *buf, int base, int d);
//static void putchar (int c);
void printf (const char *format, ...);

#endif // __PRINTF_H__
