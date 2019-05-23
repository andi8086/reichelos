#include "dma.h"
#include "io.h"

// intel 8237A dma controller

void DMA_floppy_init_mem(uint32_t phys_mem_addr, uint16_t count)
{
	outb(DMA_SCM_0_3, 0x06); // mask DMA channel 2 and 0 (assuming 0 is already masked)
	outb(DMA_FLIPFLOP_0_3, 0xFF); // reset flipflop
	outb(DMA_START_ADDR_2, phys_mem_addr & 0xFF);
	outb(DMA_START_ADDR_2, (phys_mem_addr >> 8) & 0xFF);

	outb(DMA_FLIPFLOP_0_3, 0xFF); // reset flipflop
	outb(DMA_COUNT_2, count & 0xFF);
	outb(DMA_COUNT_2, count >> 8);

	outb(DMA_PAGE_2, (phys_mem_addr >> 16) & 0xFF);
	outb(DMA_SCM_0_3, 0x02); // unmask DMA channel 2
}

void DMA_floppy_write()
{
	outb(DMA_SCM_0_3, 0x06);
	outb(DMA_MODE_0_3, 0x5A); //DMAM_SINGLE_TRANS | DMAM_ADDR_INC | DMAM_AUTOINIT | DMAM_WRITE | 2);
	outb(DMA_SCM_0_3, 0x02);
}

// FIXME: autoinit??

void DMA_floppy_read()
{
	outb(DMA_SCM_0_3, 0x06);
	outb(DMA_MODE_0_3, 0x56);
	outb(DMA_SCM_0_3, 0x02);
}


