#ifndef __FLOPPY_H__
#define __FLOPPY_H__

#include "cmos.h"
#include "dma.h"

#define FDD_STATUS_REG_A	0x3F0
#define FDD_STATUS_REG_B	0x3F1
#define FDD_DOR			0x3F2 // digital output register
#define FDD_TDR			0x3F3 // tape drive register
#define FDD_MSR			0x3F4 // main status register
#define FDD_DSR			0x3F4 // datarate select register
#define FDD_FIFO		0x3F5
#define FDD_DIR			0x3F7 // digital input register
#define FDD_CCR			0x3F7 // configuration control register

typedef enum {
	FD_NONE = 0,
	FD_360K = 1,
	FD_1_2M = 2,
	FD_720K = 3,
	FD_1_44M = 4,
	FD_2_88M = 5
} FD_TYPE;

typedef struct {
	FD_TYPE	type;
} fd_parameter_block;

extern fd_parameter_block fd_drives[2];
void fdd_init(void);

#endif // __FLOPPY_H__
