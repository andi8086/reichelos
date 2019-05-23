#include "io.h"
#include "8042.h"
#include "conio.h"

#define KBD_CFG_INT_FIRST (1 << 0)
#define KBD_CFG_INT_SECOND (1 << 1)
#define KBD_CFG_CLOCK1_DISABLED (1 << 4)
#define KBD_CFG_CLOCK2_DISABLED (1 << 5)
#define KBD_CFG_TRANSLATION (1 << 6)

uint8_t keyboard_buffer[KBD_BUFF_MAX];
uint32_t kbd_buff_idx;

bool key_released;
bool enhanced_key;

uint32_t keyboard_status;
//                                0x15                                                                      0x5D
static const char table[]       = "q1   zsaw2  cxde43   vftr5  nbhgy6   mju78  ,kio09  ./l;p-   ' [=     ] \\";
static const char table_shift[] = "Q!   ZSAW@  CXDE$#   VFTR%  NBHGY^   MJU&*  <KIO)(  >?L:P_   \" {+     } |";

static char translate(uint8_t code)
{
	if ((code & 0x7F) == 0x5A) return 0x0A;
	if (code >= 0x15 && code <= 0x5D) {
		code -= 0x15;
		return keyboard_status & (KBD_LSHIFT | KBD_RSHIFT) ? table_shift[code] : table[code];
	}
	if (code == 0x66) return 0x08;
	return 0xFF;
}

void kbd_buff_push(uint8_t scancode)
{
	if (scancode == 0xF0) {
		key_released = true;
		return;
	} else if (scancode == 0xE0) {
		enhanced_key = true;
		return;
	} else if (enhanced_key && !key_released) {
		scancode |= 0x80;
	} else if (key_released) {
		if (enhanced_key) scancode |= 0x80;
		key_released = false;
		enhanced_key = false;
		switch(scancode) {
		case 0x12: keyboard_status &= ~KBD_LSHIFT; return;
		case 0x59: keyboard_status &= ~KBD_RSHIFT; return;
		case 0x14: keyboard_status &= ~KBD_LCTRL; return;
		case 0x94: keyboard_status &= ~KBD_RCTRL; return;
		case 0x11: keyboard_status &= ~KBD_LALT; return;
		case 0x91: keyboard_status &= ~KBD_RALT; return;
		case 0x9F: keyboard_status &= ~KBD_LGUI; return;
		case 0xA7: keyboard_status &= ~KBD_RGUI; return;
		case 0xAF: keyboard_status &= ~KBD_APPS; return;
		}
		return;
	}
	switch(scancode)
	{
	case 0x12: keyboard_status |= KBD_LSHIFT; return;
	case 0x59: keyboard_status |= KBD_RSHIFT; return;
	case 0x14: keyboard_status |= KBD_LCTRL; return;
	case 0x94: keyboard_status |= KBD_RCTRL; return;
	case 0x11: keyboard_status |= KBD_LALT; return;
	case 0x91: keyboard_status |= KBD_RALT; return;
	case 0x9F: keyboard_status |= KBD_LGUI; return;
	case 0xA7: keyboard_status |= KBD_RGUI; return;
	case 0xAF: keyboard_status |= KBD_APPS; return;
	}

	// here we may have enhanced key or key
	if (kbd_buff_idx >= KBD_BUFF_MAX) return;

	char c = translate(scancode);
	if (c == 0xFF) return;

	for (uint32_t idx = kbd_buff_idx; idx > 0; idx--) {
		keyboard_buffer[idx] = keyboard_buffer[idx-1];
	}
	keyboard_buffer[0] = c;
	kbd_buff_idx++;
}

uint8_t kbd_buff_pop(void)
{
	if (kbd_buff_idx == 0) return 0;

	uint8_t code = keyboard_buffer[0];
	for (uint32_t idx = 0; idx < kbd_buff_idx-1; idx++) {
		keyboard_buffer[idx] = keyboard_buffer[idx+1];
	}
	kbd_buff_idx--;
	return code;
}

uint8_t _8042_send_command(uint8_t cmd, bool await_response)
{
	uint8_t res;
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL); 
	outb(PS2_COMMAND_PORT, cmd);
 	if (!await_response) return 0;
	while(!(inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_FULL));
	return inb(PS2_DATA_PORT);
}

uint8_t _8042_get_config_byte(void)
{
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_COMMAND_PORT, 0x20);	// read byte 0 from 8042 internal RAM
	while(!(inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_FULL));
	return inb(PS2_DATA_PORT);
}

void _8042_set_config_byte(uint8_t cfg_byte)
{
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_COMMAND_PORT, 0x60);	// write byte 0 of 8042 internal RAM
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_DATA_PORT, cfg_byte);
}

void _8042_enable_port1_interrupt(void)
{
	uint8_t cfg = _8042_get_config_byte();
	_8042_set_config_byte(cfg | KBD_CFG_INT_FIRST);
}

void _8042_enable_port2_interrupt(void)
{
	uint8_t cfg = _8042_get_config_byte();
	_8042_set_config_byte(cfg | KBD_CFG_INT_SECOND);
}

void _8042_disable_port1_interrupt(void)
{
	uint8_t cfg = _8042_get_config_byte();
	_8042_set_config_byte(cfg & ~KBD_CFG_INT_FIRST);
}

void _8042_disable_port2_interrupt(void)
{
	uint8_t cfg = _8042_get_config_byte();
	_8042_set_config_byte(cfg & ~KBD_CFG_INT_SECOND);
}

void _8042_disable_for_init(void)
{
	uint8_t cfg = _8042_get_config_byte();
	_8042_set_config_byte(cfg & ~(KBD_CFG_INT_FIRST | KBD_CFG_INT_SECOND |
				     KBD_CFG_TRANSLATION));
}	

void init_8042(void)
{
	_8042_send_command(0xAD, false); // disable first port
	_8042_send_command(0xA7, false); // disable 2nd port

	inb(0x60);			 // flush buffer

	_8042_disable_for_init();
	if (_8042_send_command(0xAA, true) != 0x55) {
		printf("ERROR: PS/2 keyboard controller failed.\n");	
	}

	_8042_send_command(0xA8, false); // enable 2nd port
	if (!(_8042_get_config_byte() & KBD_CFG_CLOCK2_DISABLED)) {
		printf("PS/2 controller has 2 channels.\n");
		_8042_send_command(0xA7, false);
	}
	
	if (_8042_send_command(0xAB, true) != 0x00) {
		printf("ERROR: Check keyboard!\n");
	}
	// enable first PS/2 port
	_8042_send_command(0xAE, false);

	uint8_t init_response;
	init_response = keyboard_send_command(0xFF);
	if (init_response != 0xFA) {
		printf("ERROR: Keyboard reset error\n");
		goto skip_reset;
	}
	while (!inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_FULL);
	init_response = inb(PS2_DATA_PORT);
	if (init_response != 0xAA) {
		printf("ERROR: Keyboard self test failed\n");
		if (init_response == 0xFC) {
			printf("Basic assurance test failed (0xFC)\n");
		}
		return;
	}	
skip_reset:
	_8042_enable_port1_interrupt();
	printf("Keyboard initialized (IRQ1).\n");
}

uint8_t keyboard_send_command(uint8_t cmd)
{
resend_command:
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_DATA_PORT, cmd);
	while(!inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_FULL);
	uint8_t res = inb(PS2_DATA_PORT);
	if (res == 0xFE) {
		// resend request
		goto resend_command;
	}
	return res;
}

uint8_t mouse_send_command(uint8_t cmd)
{
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_COMMAND_PORT, 0xD4);
	while(inb(PS2_STATUS_PORT) & PS2_INPUT_BUFFER_FULL);
	outb(PS2_DATA_PORT, cmd);
	while(!inb(PS2_STATUS_PORT) & PS2_OUTPUT_BUFFER_FULL);
	uint8_t res = inb(PS2_DATA_PORT);
	return res;
}

uint8_t keyboard_read(void)
{
	return inb(0x60);
}
