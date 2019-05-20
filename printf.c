#include "printf.h"
#include "stdarg.h"
#include <stdint.h>

static int xpos;
static int ypos;

volatile unsigned char *video;

void clrscr(void)
{
       int i;
     
       video = (unsigned char *) VIDEO;
     
       for (i = 0; i < COLUMNS * LINES * 2; i+=2) {
         *(video + i) = 0x20;
	 *(video + i + 1) = 0x07;
       } 
       xpos = 0;
       ypos = 0;
}

void itoa (char *buf, int base, int d)
{
       char *p = buf;
       char *p1, *p2;
       unsigned long ud = d;
       int divisor = 10;

       /* If %d is specified and D is minus, put `-' in the head. */
       if (base == 10 && d < 0) {
           *p++ = '-';
           buf++;
           ud = -d;
       } else if (base == 16)
               divisor = 16;

       /* Divide UD by DIVISOR until UD == 0. */
       do {
           int remainder = ud % divisor;

           *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
       }
       while (ud /= divisor);

       /* Terminate BUF. */
       *p = 0;

       /* Reverse BUF. */
       p1 = buf;
       p2 = p - 1;
       while (p1 < p2) {
           char tmp = *p1;
           *p1 = *p2;
           *p2 = tmp;
           p1++;
           p2--;
       }
}

static void vga_text_scrollup(void)
{
	for (int i = 0; i < COLUMNS*2; i++)
	{
		for (int j = 1; j < LINES; j++)
		{
			*(video + (j-1)*COLUMNS*2 + i) = *(video + j*COLUMNS*2 + i);
		}
	}
	for (int i = 0; i < COLUMNS; i++)
	{
		*(video + (LINES-1)*COLUMNS*2 + i*2) = 0x20;
	}
}

static void putchar (int c)
{
	if (c == '\n' || c == '\r') {
        newline:
		xpos = 0;
		ypos++;
           	if (ypos >= LINES) {
              		vga_text_scrollup();
	      		ypos--;
           	}
	   	cursor(xpos, ypos);
           	return;
        } else if (c == 0x08) {
	   	if (xpos == 0) {
			if (ypos == 0) return;
			xpos = COLUMNS-1;
			ypos--;
		} else {
			xpos--;
		}
		cursor(xpos, ypos);
		*(video + (xpos + ypos * COLUMNS) * 2) = 0x20;	
		return;
	}
	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

        xpos++;
        if (xpos >= COLUMNS)
		goto newline;

       	cursor(xpos, ypos);
}

static void puts(char *s)
{
	while (*s) putchar(*s++);
}

char *convert(unsigned int num, int base)
{
	static char buffer[50];
	itoa(buffer, base, num);
	return buffer;
}

void printf(const char *format, ...)
{
	va_list arg;
	unsigned int i;
	char *s;

	va_start(arg, format);

	while(*format) {
		while (*format != '%' && *format != 0)
		{
			putchar(*format);
			format++;
		}
		if (*format == 0) break;
		format++;
		switch(*format++) {
		case 'c': i = va_arg(arg, int);
			putchar(i);
			break;
		case 'u': i = va_arg(arg, unsigned int);
			goto print_decimal;
		case 'd': i = va_arg(arg, int);
			if (i < 0) {
				i = -i;
				putchar('-');
			}
		print_decimal:
			puts(convert(i, 10));
			break;
		case 'o': i = va_arg(arg, unsigned int);
			puts(convert(i,8));
			break;
		case 's': s = va_arg(arg, char *);
			puts(s);
			break;
		case 'x': i = va_arg(arg, unsigned int);
			puts(convert(i, 16));
			break;
		}
	}
	//va_end(arg);
}


