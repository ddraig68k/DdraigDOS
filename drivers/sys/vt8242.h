#ifndef DDRAIGOS_VT8242_DRIVER_H
#define DDRAIGOS_VT8242_DRIVER_H

#include "ddraig.h"
#include "libraries/ringbuffer.h"

#define PS2_REG(x)      (*((volatile uint8_t *) PS2_BASE + x))

// Register Offsets
#define PS2_DATA		  		0x00
#define PS2_CMD 				0x02
#define PS2_STAT				0x02

// 8542B Commands (Sent to the Command Port)
#define PS2_CMD_SET_BYTE		0x60	// Set the command byte
#define PS2_CMD_GET_BYTE		0x20	// Get the command byte
#define PS2_CMD_KBD_OBUFF		0xD2	// Write to HT8542 Keyboard Output Buffer
#define PS2_CMD_AUX_OBUFF		0xD3	// Write to HT8542 Mouse Output Buffer
#define PS2_CMD_AUX_WRITE		0xD4	// Write to Mouse Port
#define PS2_CMD_AUX_OFF			0xA7	// Disable Mouse Port
#define PS2_CMD_AUX_ON			0xA8	// Re-Enable Mouse Port
#define PS2_CMD_AUX_TEST		0xA9	// Test for the presence of a Mouse
#define PS2_CMD_DIAG			0xAA	// Start Diagnostics
#define PS2_CMD_KBD_TEST		0xAB	// Test for presence of a keyboard
#define PS2_CMD_KBD_OFF			0xAD	// Disable Keyboard Port
#define PS2_CMD_KBD_ON			0xAE	// Re-Enable Keyboard Port

// HT8542B command byte set by KBD_CMD_SET_BYTE and retrieved by KBD_CMD_GET_BYTE
#define PS2_CMD_BYTE_TRANS		0x40
#define PS2_CMD_BYTE_AUX_OFF	0x20	// 1 = mouse port disabled, 0 = enabled
#define PS2_CMD_BYTE_KBD_OFF	0x10	// 1 = keyboard port disabled, 0 = enabled
#define PS2_CMD_BYTE_OVER		0x08	// 1 = override keyboard lock
#define PS2_CMD_BYTE_RES		0x04	// reserved
#define PS2_CMD_BYTE_AUX_INT	0x02	// 1 = enable mouse interrupt
#define PS2_CMD_BYTE_KBD_INT	0x01	// 1 = enable keyboard interrupt

// Keyboard Commands (Sent to the Data Port)
#define PS2_KBD_CMD_LED			0xED	// Set Keyboard LEDS with next byte
#define PS2_KBD_CMD_ECHO		0xEE	// Echo - we get 0xFA, 0xEE back
#define PS2_KBD_CMD_MODE		0xF0	// set scan code mode with next byte
#define PS2_KBD_CMD_ID			0xF2	// get keyboard/mouse ID
#define PS2_KBD_CMD_RPT			0xF3	// Set Repeat Rate and Delay with Second Byte
#define PS2_KBD_CMD_ON			0xF4	// Enable keyboard
#define PS2_KBD_CMD_OFF			0xF5	// Disables Scanning and Resets to Defaults
#define PS2_KBD_CMD_DEF			0xF6	// Reverts keyboard to default settings
#define PS2_KBD_CMD_RST			0xFF	// Reset - we should get 0xFA, 0xAA back

// Set LED second bit defines
#define PS2_KBD_CMD_LED_SCROLL	0x01	// Set SCROLL LOCK LED on
#define PS2_KBD_CMD_LED_NUM		0x02	// Set NUM LOCK LED on
#define PS2_KBD_CMD_LED_CAPS	0x04	// Set CAPS LOCK LED on

// Set Mode second byte defines
#define PS2_KBD_CMD_MODE_STAT	0x00	// get current scan code mode
#define PS2_KBD_CMD_MODE_SCAN1	0x01	// set mode to scan code 1 - PC/XT
#define PS2_KBD_CMD_MODE_SCAN2	0x02	// set mode to scan code 2
#define PS2_KBD_CMD_MODE_SCAN3	0x03	// set mode to scan code 3

// Keyboard/Mouse ID Codes
#define PS2_KBD_CMD_ID_1ST		0xAB	// first byte is 0xAB, second is actual ID
#define PS2_KBD_CMD_ID_KBD		0x83	// Keyboard
#define PS2_KBD_CMD_ID_MOUSE	0x00	// Mouse

// Keyboard Data Return Defines
#define PS2_KBD_STAT_OVER		0x00	// Buffer Overrun
#define PS2_KBD_STAT_DIAG_OK	0x55	// Internal Self Test OK
#define PS2_KBD_STAT_RST_OK		0xAA	// Reset Complete
#define PS2_KBD_STAT_ECHO		0xEE	// Echo Command Return
#define PS2_KBD_STAT_BRK		0xF0	// Prefix for Break Key Code
#define PS2_KBD_STAT_ACK		0xFA	// Received after all commands
#define PS2_KBD_STAT_DIAG_FAIL	0xFD	// Internal Self Test Failed
#define PS2_KBD_STAT_RESEND		0xFE	// Resend Last Command

// HT8542B Status Register Bit Defines
#define PS2_STAT_OBF			0x01	// 1 = output buffer (controller to cpu) has data
#define PS2_STAT_IBF			0x02	// 1 = input buffer (cpu to controller) has data
#define PS2_STAT_SYS			0x04	// system flag - unused
#define PS2_STAT_CMD			0x08	// 1 = command in input buffer, 0 = data
#define PS2_STAT_INH			0x10	// 1 = Inhibit - unused
#define PS2_STAT_TX				0x20	// 1 = Transmit Timeout has occured
#define PS2_STAT_RX				0x40	// 1 = Receive Timeout has occured
#define PS2_STAT_PERR			0x80	// 1 = Parity Error from Keyboard

// Key codes for control keys
/*
#define KEY_L_SHIFT		0x2a
#define KEY_R_SHIFT		0x36
#define KEY_CTL			0x1d
#define KEY_ALT			0x38
#define KEY_CAPS		0x3a
#define KEY_NUM_LK		0x45
#define KEY_SCROLL_LK	0x46
#define KEY_EXT0		0xE0
#define KEY_EXT1		0xE1
#define KEY_END			0x4f
*/

#define SCAN_MODE_BREAK			0x01
#define SCAN_MODE_MODIFIER		0x02
#define SCAN_MODE_SHIFT_L		0x04
#define SCAN_MODE_SHIFT_R		0x08
#define SCAN_MODE_ALTGR 		0x10
#define SCAN_MODE_CTRL          0x20

#define KEY_MODE_SCROLLLOCK		0x01
#define KEY_MODE_NUMLOCK		0x02
#define KEY_MODE_CAPSLOCK		0x04

// Key codes for control keys
#define KEY_L_SHIFT		0x2a
#define KEY_R_SHIFT		0x36
#define KEY_CTL			0x1d
#define KEY_ALT			0x38
#define KEY_CAPS		0x3a
#define KEY_NUM_LK		0x45
#define KEY_SCROLL_LK	0x46
#define KEY_EXT0		0xE0
#define KEY_EXT1		0xE1
#define KEY_END			0x4f

extern ringbuffer_t g_buf_scancode;
extern ringbuffer_t g_buf_keypress;

bool vt8242_init();
bool vt8242_has_data();
uint8_t vt8242_get_key();
uint8_t vt8242_flush();

void vt8242_init_buffers();
void vt8242_set_config_byte(uint8_t cfg_byte);
void vt8242_process_scancode(uint8_t sc);

void vt8242_enable_port1_interrupt(void);
void vt8242_enable_port2_interrupt(void);
void vt8242_disable_port1_interrupt(void);
void vt8242_disable_port2_interrupt(void);


#endif
