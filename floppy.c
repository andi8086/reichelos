#include "floppy.h"
#include "cmos.h"
#include "conio.h"
#include "io.h"
#include "usleep.h"
#include "timer.h"

fd_parameter_block fd_drives[2];

const char *fdnames[6] = {"None", "360K", "1.2M", "720K", "1.44M", "2.88M"};

volatile bool fdd_received_irq = false;

#define MSR_RQM		0x80
#define MSR_BUSY	0x10

#define DOR_IRQDMA	0x08
#define DOR_NORMAL	0x04

uint8_t	st0;
uint8_t	st1;

typedef enum
{
   READ_TRACK =                 2,	// generates IRQ6
   SPECIFY =                    3,      // * set drive parameters
   SENSE_DRIVE_STATUS =         4,
   WRITE_DATA =                 5,      // * write to the disk
   READ_DATA =                  6,      // * read from the disk
   RECALIBRATE =                7,      // * seek to cylinder 0
   SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
   WRITE_DELETED_DATA =         9,
   READ_ID =                    10,	// generates IRQ6
   READ_DELETED_DATA =          12,
   FORMAT_TRACK =               13,     // *
   DUMPREG =                    14,
   SEEK =                       15,     // * seek both heads to cylinder X
   VERSION =                    16,	// * used during initialization, once
   SCAN_EQUAL =                 17,
   PERPENDICULAR_MODE =         18,	// * used during initialization, once, maybe
   CONFIGURE =                  19,     // * set controller parameters
   LOCK =                       20,     // * protect controller params from a reset
   VERIFY =                     22,
   SCAN_LOW_OR_EQUAL =          25,
   SCAN_HIGH_OR_EQUAL =         29
} floppy_cmd;

uint8_t fdd_read_byte(void)
{
	while((inb(FDD_MSR) & 0xD0) != 0xD0) asm volatile("nop");
	return inb(FDD_FIFO);
}

void fdd_write_byte(uint8_t byte)
{
	while((inb(0x3F4) & 0xC0) != 0x80) asm volatile("nop");	
	outb(FDD_FIFO, byte);
}

void fdd_wait_for_irq(void)
{
	while (!fdd_received_irq) asm volatile("nop");
}

void fdd_reset(void)
{
	// Enter, then exit reset mode.
	outb(FDD_DOR, 0x00);

	outb(FDD_CCR,0x00);	// 500Kbps -- for 1.44M floppy
	fdd_received_irq = false; 	// This will prevent the FDC from being faster than us!
	
	outb(FDD_DOR, DOR_IRQDMA | DOR_NORMAL);

	fdd_wait_for_irq();

	for (uint8_t x = 0; x < 4; x++) {
		fdd_write_byte(SENSE_INTERRUPT);
		fdd_read_byte();
		fdd_read_byte();
	}


	// configure the drive

	bool nodma = false;
	uint8_t srt_ms = 8;
	uint8_t hut_ms = 240;
	uint8_t hlt_ms = 10;

	uint32_t datarate = 500000;
	uint8_t srt = 16 - (uint8_t)((uint32_t) srt_ms * (datarate / 500000));
	uint8_t hut = (uint8_t) ((uint32_t) hut_ms * datarate / 8000000);
	uint8_t hlt = (uint8_t) ((uint32_t) hlt_ms * datarate / 1000000);

	fdd_write_byte(SPECIFY);

//	fdd_send_data(srt << 4 | hut);
//	fdd_send_data(hlt << 1 | nodma);
	fdd_write_byte(0xAF);
	fdd_write_byte(0x02);
}

void fdd_read_sector(uint8_t head, uint8_t track, uint8_t sector)
{
	

}

void fdd_activate_drive(uint8_t drive)
{
	outb(FDD_DOR, (0x10 << drive) | 0x0C | drive);
}

void fdd_disable_drives(void)
{
	outb(FDD_DOR, 0x0C);
}

void fdd_sense_interrupt(void)
{
	fdd_write_byte(SENSE_INTERRUPT);
	st0 = fdd_read_byte();
	printf(" ST0 = %x\n", st0);
	st1 = fdd_read_byte();	
	printf(" ST1 = %x\n", st1);
}

void fdd_seek_track(uint8_t drive, uint8_t head, uint8_t track)
{
	fdd_received_irq = false;
	
	fdd_write_byte(SEEK);
	fdd_write_byte(head << 2 | drive);
	fdd_write_byte(track);

	fdd_wait_for_irq();
	fdd_sense_interrupt();
}

void fdd_seektest(void)
{
	printf("FDD: seek track 39\n");
	fdd_seek_track(0, 0, 39);
	printf("FDD: seek track 0\n");
	fdd_seek_track(0, 0, 0);
	printf("FDD: seek track 79\n");
	fdd_seek_track(0, 0, 79);
	printf("FDD: seek track 0\n");
	fdd_seek_track(0, 0, 0);
	
	fdd_disable_drives();
}

void fdd_recalibrate(uint8_t drive)
{

	printf("activate drive 0\n");
	fdd_activate_drive(0);

	for (uint8_t i = 0; i < 8; i++) {
		fdd_received_irq = false;
		printf("write byte %d\n", RECALIBRATE);
		fdd_write_byte(RECALIBRATE);
		printf("write byte %u\n", drive);
		fdd_write_byte(drive);
		printf("wait for irq\n");
		fdd_wait_for_irq();
		printf("sense interrupt\n");
		fdd_sense_interrupt();
		printf("st0 = %x\n", st0);
		if (st0 & 0x20) break;
	}
	printf("done\n");
}


void fdd_init(void)
{
	uint8_t fd_cmos = read_cmos(0x10);

	fd_drives[0].type = fd_cmos >> 4;
	fd_drives[1].type = fd_cmos & 0xF;

	printf("FDD: drive 0 is %s, drive 1 is %s\n",
		fdnames[fd_drives[0].type], fdnames[fd_drives[1].type]);

	fdd_reset();
	printf("FDD: Recalibrating drive 0\n");
	fdd_recalibrate(0);
	printf("FDD: Seek test\n");
	fdd_seektest();
}


