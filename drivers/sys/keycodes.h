#ifndef DDRAIG_OS_KEYCODES_H
#define DDRAIG_OS_KEYCODES_H

#include <stdint.h>

#define KBRSP_ERR_DETECTION		0x00
#define KBRSP_SELFTEST_PASS		0xAA
#define KBRSP_ECHO				0xEE
#define KBRSP_ACK				0xFA
#define KBRSP_SELFTEST_FAIL		0xFC
#define KBRSP_SELFTEST_ERR		0xFD
#define KBRSP_RESEND			0xFE
#define KBRSP_ERR_OVERFLOW		0xFF

#define KBCMD_SET_LEDS			0xED
#define KBCMD_ECHO				0xEE
#define KBCMD_GET_KEYSET		0xF0
#define KBCMD_IDENTIFY			0xF2
#define KBCMD_SET_TYPEMATIC		0xF3
#define KBCMD_ENABLE_SCANNING	0xF4
#define KBCMD_DISABLE_SCANNING	0xF5
#define KBCMD_RESET				0xFF

#define PS2_TAB				0x09
#define PS2_ENTER			0x0D
#define PS2_LINEFEED        0x0A
#define PS2_BACKSPACE		0x7F
#define PS2_ESC				0x1B
#define PS2_INSERT			0x00
#define PS2_DELETE			0x7F
#define PS2_HOME			0x80
#define PS2_END				0x81
#define PS2_PAGEUP			0x82
#define PS2_PAGEDOWN		0x83
#define PS2_UPARROW			0x84
#define PS2_LEFTARROW		0x85
#define PS2_DOWNARROW		0x86
#define PS2_RIGHTARROW		0x87
#define PS2_F1				0x88
#define PS2_F2				0x89
#define PS2_F3				0x8A
#define PS2_F4				0x8B
#define PS2_F5				0x8C
#define PS2_F6				0x8D
#define PS2_F7				0x8E
#define PS2_F8				0x8F
#define PS2_F9				0x90
#define PS2_F10				0x91
#define PS2_F11				0x92
#define PS2_F12				0x93
#define PS2_SCROLL			0x94

#define PS2_KEYMAP_SIZE 136

extern const uint8_t g_ps2_keymap[];
extern const uint8_t g_ps2_keymap_shift[];


#endif
