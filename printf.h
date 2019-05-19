#ifndef __PRINTF_H__
#define __PRINTF_H__

#define VIDEO 0xB8000

#define COLUMNS 80
#define LINES 24
#define ATTRIBUTE 7

void clrscr(void);
void itoa (char *buf, int base, int d);
//static void putchar (int c);
void printf (const char *format, ...);

#endif // __PRINTF_H__
