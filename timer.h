#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

extern volatile uint64_t syscounter;

void init_timer(void);

#endif // __TIMER_H__
