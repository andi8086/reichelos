#include <stdint.h>
#include "timer.h"
#include "io.h"

volatile uint64_t syscounter;
volatile int32_t kerneltimer;

void init_timer(void)
{
	outb(0x43, 0x34);	// counter 0 mode 4
	outb(0x40, 0x77);	// divider 77 (119 gives 1.19318 MHz / 119 = 10.02 kHz)
	outb(0x40, 0x00);
}
