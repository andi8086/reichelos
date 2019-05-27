#include "kcmd.h"
#include "mem.h"
#include "vga.h"
#include "vga_tools.h"
#include "usleep.h"

#define OS_STRING	"ReichelOS i386, v0.1 - experimental"

char buffer[256];
char command[256];
int argc;
char *argv[128]; // maximally 127 arguments expected

#define KCMD_OK			0
#define KCMD_TOO_MANY_ARGS	-2

int parse_args(char *buffer)
{
	char *tmp = buffer;

	argc = 0;
	rmemset(argv, 0, 128);

	while(*buffer && *buffer == 0x20) buffer++;
	if (!*buffer) return KCMD_OK;
	argv[argc++] = buffer;
	while(*buffer) {
		if (*(buffer++) == 0x20) {
			*(buffer-1) = 0;
			if (buffer == tmp + 256) break;
			if (argc > 127) {
				return KCMD_TOO_MANY_ARGS;
			}
			while (*buffer && *buffer == 0x20) buffer++;
			if (!*buffer) break;
			argv[argc++] = buffer;
		}
	}
	return KCMD_OK;
}

const char *err_msg_syntax = "Syntax error\n";

int strlen(char *s)
{
	int n = 0;
	while(*(s++)) n++;
	return n;
}

int strcmp(char *s1, char *s2)
{
	int slen1 = strlen(s1);
	int slen2 = strlen(s2);
	if (slen1 != slen2) return slen1 - slen2;
	while(*s1) {
		if (*s1 != *s2) return *s1 - *s2; 
		s1++;
		s2++;
	}
	return 0;
}

int check_argc(int argc, int should)
{
	if (argc != should) {
		printf("Syntax error\n");
		return -1;
	}
	return 0;
}

int32_t kexec_cmd(int argc, char **argv)
{
	if (argc == 0) return 0;
	printf("\n");
	if (strcmp(argv[0], "ver") == 0) {
		if (check_argc(argc, 1)) return -1;
		printf("%s\n", OS_STRING);
	} else if (strcmp(argv[0], "tem") == 0) {
		if (check_argc(argc, 2)) return -1;
		set_text_mode(atoi(argv[1]));
		clrscr();
		printf("Columns = %d, Lines = %d\n", COLUMNS, LINES);
	} else if (strcmp(argv[0], "cls") == 0) {
		if (check_argc(argc, 1)) return -1;
		clrscr();
	} else if (strcmp(argv[0], "clr") == 0) {
		if (check_argc(argc, 3)) return -1;
		ATTRIBUTE = (atoi(argv[1]) & 0xF) << 4 | (atoi(argv[2]) & 0xF);
	} else if (strcmp(argv[0], "sleep") == 0) {
		if (check_argc(argc, 2)) return -1;
		uint32_t delaytime = atoi(argv[1]);
		usleep(delaytime);
	} else if (strcmp(argv[0], "echo") == 0) {
		for (uint8_t i = 1; i < argc; i++) {
			printf("%s", argv[i]);
			if (i < argc - 1) printf(" ");
		}
	}
	else {
		printf("Unknown command\n");
		return -1;
	}
	return 0;
}

void kcmdloop(void)
{
	int32_t ret;
	printf("Kernel command line\n");
	while(1) {
		printf("\n0:>");
		scanf("%256s", buffer);
		switch(parse_args(buffer)) {
		case KCMD_TOO_MANY_ARGS:
			printf("Too many arguments (127 max)\n");
			break;
		default:
			ret = kexec_cmd(argc, argv);
			break;
		}
	}
}
