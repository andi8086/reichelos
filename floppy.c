#include "floppy.h"
#include "cmos.h"
#include "conio.h"

fd_parameter_block fd_drives[2];

const char *fdnames[6] = {"None", "360K", "1.2M", "720K", "1.44M", "2.88M"};

void fdd_init(void)
{
	uint8_t fd_cmos = read_cmos(0x10);

	fd_drives[0].type = fd_cmos >> 4;
	fd_drives[1].type = fd_cmos & 0xF;

	printf("FDD: drive 0 is %s, drive 1 is %s\n",
		fdnames[fd_drives[0].type], fdnames[fd_drives[1].type]);
}
