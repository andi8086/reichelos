#ifndef __KPRINTF_H__
#define __KPRINTF_H__

#ifdef IN_KERNEL

#include "conio.h"
#define KPRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)

#else

#define KPRINTF(fmt, ...)

#endif

#endif // __KPRINTF_H__
