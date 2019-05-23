#ifndef __CMOS_H__
#define __CMOS_H__

#include <stdint.h>

#define CMOS_FLOPPY	0x10

uint8_t read_cmos(uint8_t index);

#endif // __CMOS_H__
