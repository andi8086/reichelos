#ifndef __8042_H__
#define __8042_H__

#define PS2_OUTPUT_BUFFER_FULL 1
#define PS2_INPUT_BUFFER_FULL 2

#define PS2_COMMAND_PORT	0x64
#define PS2_STATUS_PORT		0x64
#define PS2_DATA_PORT		0x60

#define KBD_BUFF_MAX		16	

#define KBD_LSHIFT	(1 << 0)
#define KBD_RSHIFT	(1 << 1)
#define KBD_LCTRL	(1 << 2)
#define KBD_RCTRL	(1 << 3)
#define KBD_LALT	(1 << 4)
#define KBD_RALT	(1 << 5)
#define KBD_LGUI	(1 << 6)
#define KBD_RGUI	(1 << 7)
#define KBD_APPS	(1 << 8)

#include <stdbool.h>

extern uint8_t keyboard_buffer[KBD_BUFF_MAX];
extern uint32_t kbd_buff_idx;
extern uint32_t keyboard_status;

void kbd_buff_push(uint8_t scancode);
uint8_t kbd_buff_pop(void);
uint8_t _8042_send_command(uint8_t cmd, bool await_response);
uint8_t _8042_get_config_byte(void);
void _8042_set_config_byte(uint8_t cfg_byte);
void _8042_enable_port1_interrupt(void);
void _8042_enable_port2_interrupt(void);
void _8042_disable_port1_interrupt(void);
void _8042_disable_port2_interrupt(void);
void init_8042(void);
uint8_t keyboard_send_command(uint8_t cmd);
uint8_t keyboard_read(void);
uint8_t mouse_send_command(uint8_t cmd);

#endif // __8042_H__
