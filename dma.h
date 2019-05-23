#ifndef __DMA_H__
#define __DMA_H__

#include <stdint.h>

#define DMA_START_ADDR_0	0x00
#define DMA_START_ADDR_4	0xC0
#define DMA_COUNT_0		0x01
#define DMA_COUNT_4		0xC2
#define DMA_START_ADDR_1	0x02
#define DMA_START_ADDR_5	0xC4
#define DMA_COUNT_1		0x03
#define DMA_COUNT_5		0xC6
#define DMA_START_ADDR_2	0x04
#define DMA_START_ADDR_6	0xC8
#define DMA_COUNT_2		0x05
#define DMA_COUNT_6		0xCA
#define DMA_START_ADDR_3	0x06
#define DMA_START_ADDR_7	0xCC
#define DMA_COUNT_3		0x07
#define DMA_COUNT_7		0xCE
#define DMA_STATUS_0_3		0x08
#define DMA_STATUS_4_7		0xD0
#define DMA_CMD_0_3		0x08
#define DMA_CMD_4_7		0xD0
#define DMA_REQ_0_3		0x09
#define DMA_REQ_4_7		0xD2
#define DMA_SCM_0_3		0x0A // single channel mask
#define DMA_SCM_4_7		0xD4
#define DMA_MODE_0_3		0x0B
#define DMA_MODE_4_7		0xD6
#define DMA_FLIPFLOP_0_3	0x0C
#define DMA_FLIPFLOP_4_7	0xD8
#define DMA_INTERMED_0_3	0x0D
#define DMA_INTERMED_4_7	0xDA
#define DMA_MASTER_RESET_0_3	0x0D
#define DMA_MASTER_RESET_4_7	0xDA
#define DMA_MASK_RESET_0_3	0x0E
#define DMA_MASK_RESET_4_7	0xDC
#define DMA_MULTICHAN_MASK_0_3	0x0F
#define DMA_MULTICHAN_MASK_4_7	0xDE

#define DMA_PAGE_0		0x87 // unusable
#define DMA_PAGE_1		0x83
#define DMA_PAGE_2		0x81
#define DMA_PAGE_3		0x82
#define DMA_PAGE_4		0x8F // unusable
#define DMA_PAGE_5		0x8B
#define DMA_PAGE_6		0x09
#define DMA_PAGE_7		0x8A

void DMA_floppy_init_mem(uint32_t phys_mem_addr, uint16_t count);
void DMA_floppy_write();
void DMA_floppy_read();

#endif // __DMA_H__
