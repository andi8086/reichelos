#include "dma.h"
#include "io.h"

// intel 8237A dma controller

void DMA_mask_channel(uint8_t channel)
{
	if (channel < 4) {
		outb(DMA_SCM_0_3, (1 << 2) | channel);
	} else {
		// FIXME
	}
}

void DMA_unmask_channel(uint8_t channel)
{
	if (channel < 4) {
		outb(DMA_SCM_0_3, (0 << 2) | channel);
	} else {
		// FIXME
	}
}


void DMA_floppy_init_mem(uint32_t phys_mem_addr, uint16_t count)
{
	DMA_mask_channel(2);
	outb(DMA_FLIPFLOP_0_3, 0xFF); // reset flipflop
	outb(DMA_START_ADDR_2, phys_mem_addr & 0xFF);
	outb(DMA_START_ADDR_2, (phys_mem_addr >> 8) & 0xFF);

	outb(DMA_FLIPFLOP_0_3, 0xFF); // reset flipflop
	outb(DMA_COUNT_2, count & 0xFF);
	outb(DMA_COUNT_2, count >> 8);

	outb(DMA_PAGE_2, (phys_mem_addr >> 16) & 0xFF);
	DMA_unmask_channel(2);
}

void DMA_floppy_write()
{
	DMA_mask_channel(2);
	outb(DMA_MODE_0_3, 0x5A); //DMAM_SINGLE_TRANS | DMAM_ADDR_INC | DMAM_AUTOINIT | DMAM_WRITE | 2);
	DMA_unmask_channel(2);
}

// FIXME: autoinit??

void DMA_floppy_read()
{
	DMA_mask_channel(2);
	outb(DMA_MODE_0_3, 0x56);
	DMA_unmask_channel(2);
}


