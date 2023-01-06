#include <stdio.h>
#include <printf.h>
#include <ctype.h>
#include <stdbool.h>

#include "vt8242.h"
#include "keycodes.h"
#include "drivers/keyboard.h"

uint8_t vt8242_flush();

uint8_t g_vt8242_key_mode;
uint8_t g_vt8242_state;

#define SCANCODE_BUFFER_SIZE 64
#define KEYPRESS_BUFFER_SIZE 64

uint8_t g_scancode_data[SCANCODE_BUFFER_SIZE];
uint8_t g_keypress_data[KEYPRESS_BUFFER_SIZE];

ringbuffer_t g_buf_scancode;
ringbuffer_t g_buf_keypress;

//#define PS2_DEBUG 1
#define PS2_TIMEOUT 2000

//	keyboard interrupt handler
void __attribute__((interrupt)) vt8242_keyboard_interrupt()
{
    uint8_t register sc;
    sc = PS2_REG(PS2_DATA);
    ring_buf_put(&g_buf_scancode, sc);
}

static void vt8242_delay(unsigned long d)
{
    volatile unsigned long wait = d;
    while (wait--);
}

void vt8242_set_leds(uint8_t leds)
{
#ifdef PS2_DEBUG
    printf("Setting LEDS to %02X .....", leds);
#endif
	while(PS2_REG(PS2_STAT) & PS2_STAT_IBF);
	PS2_REG(PS2_DATA) = PS2_KBD_CMD_LED;

	while(!PS2_REG(PS2_STAT) & PS2_STAT_OBF);
	PS2_REG(PS2_DATA) = leds;
#ifdef PS2_DEBUG
    printf("Done\n");
#endif
}

uint8_t vt8242_send_command(uint8_t cmd, bool wait_response)
{
    while (PS2_REG(PS2_STAT) & PS2_STAT_IBF);
    PS2_REG(PS2_CMD) = cmd;

    if (!wait_response)
        return 0;

    while (!(PS2_REG(PS2_STAT) & PS2_STAT_OBF));
    return PS2_REG(PS2_DATA);
}

uint8_t vt8242_get_config_byte(void)
{
	while(PS2_REG(PS2_STAT) & PS2_STAT_IBF);
	PS2_REG(PS2_CMD) = 0x20;	// read byte 0 from 8042 internal RAM

    while (!(PS2_REG(PS2_STAT) & PS2_STAT_OBF));
    return PS2_REG(PS2_DATA);
}

void vt8242_set_config_byte(uint8_t cfg_byte)
{
	while(PS2_REG(PS2_STAT) & PS2_STAT_IBF);
	PS2_REG(PS2_CMD) = 0x60;	// write byte 0 of 8042 internal RAM

    while(PS2_REG(PS2_STAT) & PS2_STAT_IBF);
	PS2_REG(PS2_DATA) = cfg_byte;
}

void vt8242_disable_for_init(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~(PS2_CMD_BYTE_KBD_INT | PS2_CMD_BYTE_AUX_INT | PS2_CMD_BYTE_TRANS));
}

uint8_t keyboard_send_command(uint8_t cmd)
{
    uint8_t res;

    while(PS2_REG(PS2_STAT) & PS2_STAT_IBF);
    PS2_REG(PS2_DATA) = cmd;

    vt8242_delay(20);

    while(!PS2_REG(PS2_STAT) & PS2_STAT_OBF);
    res = PS2_REG(PS2_DATA);
	return res;
}

void vt8242_enable_port1_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | PS2_CMD_BYTE_KBD_INT);
}

void vt8242_enable_port2_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg | PS2_CMD_BYTE_AUX_INT);
}

void vt8242_disable_port1_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~PS2_CMD_BYTE_KBD_INT);
}

void vt8242_disable_port2_interrupt(void)
{
	uint8_t cfg = vt8242_get_config_byte();
	vt8242_set_config_byte(cfg & ~PS2_CMD_BYTE_AUX_INT);
}

bool vt8242_init()
{
    vt8242_disable_port1_interrupt();
    vt8242_disable_port2_interrupt();

	vt8242_send_command(PS2_CMD_KBD_OFF, false); // disable first port
	vt8242_send_command(PS2_CMD_AUX_OFF, false); // disable 2nd port

	ISR_VECT_PS2(vt8242_keyboard_interrupt);

	vt8242_disable_for_init();

	vt8242_flush();			 // flush buffer

	if (vt8242_send_command(PS2_CMD_DIAG, true) != PS2_KBD_STAT_DIAG_OK)
    {
#ifdef PS2_DEBUG
		printf("ERROR: PS/2 keyboard controller failed.\n");
#endif
        return 0;
	}

	vt8242_send_command(PS2_CMD_AUX_ON, false); // enable 2nd port
	if (!(vt8242_get_config_byte() & PS2_CMD_BYTE_AUX_OFF))
    {
#ifdef PS2_DEBUG
		printf("PS/2 controller has 2 channels.\n");
#endif
		vt8242_send_command(PS2_CMD_AUX_OFF, false);
	}

	if (vt8242_send_command(PS2_CMD_KBD_TEST, true) != 0x00)
    {
#ifdef PS2_DEBUG
		printf("ERROR: Check keyboard!\n");
#endif
	}
	// enable first PS/2 port
	vt8242_send_command(PS2_CMD_KBD_ON, false);
    vt8242_flush();
    //TODO:: Work out what the other code is doing on the keyboard reset as it seems to work

    int retries = 30;
    uint8_t init_response;

    while (retries--)
    {
        init_response = keyboard_send_command(PS2_KBD_CMD_RST);
        if (init_response == PS2_KBD_STAT_RESEND)
        {
    #ifdef PS2_DEBUG
            printf("ERROR: Keyboard reset resending\n");
    #endif
            continue;
        }
        else if ((init_response != PS2_KBD_STAT_RST_OK) && (init_response != PS2_KBD_STAT_ACK))
        {
    #ifdef PS2_DEBUG
            printf("ERROR: Keyboard reset error, resp = %02x\n", init_response);
    #endif
            //return 0;
        }
    }

    while (!PS2_REG(PS2_STAT) & PS2_STAT_OBF)
        {}

	init_response = PS2_REG(PS2_DATA);

    if ((init_response != PS2_KBD_STAT_RST_OK) && (init_response != PS2_KBD_STAT_ACK))
    {
#ifdef PS2_DEBUG
		printf("ERROR: Keyboard self test failed, resp = %02X\n", init_response);
		if (init_response == 0xFC)
        {
			printf("Basic assurance test failed (0xFC)\n");
		}
#endif
		return 0;
	}

    return 1;
}

uint8_t vt8242_flush()
{
    int timeout = PS2_TIMEOUT;
#ifdef PS2_DEBUG
	printf("vt8242_flush : Flushing buffer ");
#endif
    // Clear the Output Buffer
    while (timeout)
	{
        if ((PS2_REG(PS2_STAT) & PS2_STAT_OBF))
            PS2_REG(PS2_DATA);
        else
            break;
        timeout--;
#ifdef PS2_DEBUG
		printf(".");
#endif
    }

	if (timeout == 0)
	{
        printf("keyboard output buffer flush timed out - Controller Failure?\n");
		return -1;
	}
	else
    {
#ifdef PS2_DEBUG
	    printf("done!\n");
#endif
    }
	return 0;
}

bool vt8242_has_data()
{
    process_keyboard();
    if (is_ring_buf_empty(&g_buf_keypress))
        return false;

    return true;
}

uint8_t vt8242_get_key()
{
    process_keyboard();
    if (is_ring_buf_empty(&g_buf_keypress))
        return 0;

    uint8_t key = ring_buf_get(&g_buf_keypress);
    return key;
}

uint8_t vt8242_get_key_polled()
{
	uint8_t register chr;
	uint8_t register tmp;

	while (true)
    {
        //TODO: To make interrupt, remove while and add ring buffer at end
        while ((PS2_REG(PS2_STAT) & PS2_STAT_OBF) == 0x00)
            {}

		tmp = PS2_REG(PS2_DATA);
#ifdef PS2_DEBUG
        printf("Raw scancode = %02X\n", tmp);
#endif

		if (tmp == 0xF0)
			g_vt8242_state |= SCAN_MODE_BREAK;
		else if (tmp == 0xE0)
			g_vt8242_state |= SCAN_MODE_MODIFIER;
		else
        {
			if (g_vt8242_state & SCAN_MODE_BREAK)
            {
				if (tmp == 0x12)
					g_vt8242_state &= ~SCAN_MODE_SHIFT_L;
				else if (tmp == 0x59)
					g_vt8242_state &= ~SCAN_MODE_SHIFT_R;
				else if (tmp == 0x11 && (g_vt8242_state & SCAN_MODE_MODIFIER))
					g_vt8242_state &= ~SCAN_MODE_ALTGR;

                g_vt8242_state &= ~(SCAN_MODE_BREAK | SCAN_MODE_MODIFIER);
				continue;
			}

            if (tmp == 0x12)
            {
				g_vt8242_state |= SCAN_MODE_SHIFT_L;
				continue;
			}
            else if (tmp == 0x59)
            {
				g_vt8242_state |= SCAN_MODE_SHIFT_R;
				continue;
			}
            else if (tmp == 0x11 && (g_vt8242_state & SCAN_MODE_MODIFIER))
            {
				g_vt8242_state |= SCAN_MODE_ALTGR;
			}

			if (tmp == 0x58)
            {
				g_vt8242_key_mode ^= KEY_MODE_CAPSLOCK;
				vt8242_set_leds(g_vt8242_key_mode);
			}
            else if (tmp == 0x77)
            {
				g_vt8242_key_mode ^= KEY_MODE_NUMLOCK;
				vt8242_set_leds(g_vt8242_key_mode);
			}
            else if (tmp == 0x7E)
            {
				g_vt8242_key_mode ^= KEY_MODE_SCROLLLOCK;
				vt8242_set_leds(g_vt8242_key_mode);
			}

			chr = 0;
			if (g_vt8242_state & SCAN_MODE_MODIFIER)
            {
				switch (tmp)
                {
					case 0x70: chr = PS2_INSERT;      	break;
					case 0x6C: chr = PS2_HOME;        	break;
					case 0x7D: chr = PS2_PAGEUP;      	break;
					case 0x71: chr = PS2_DELETE;      	break;
					case 0x69: chr = PS2_END;         	break;
					case 0x7A: chr = PS2_PAGEDOWN;    	break;
					case 0x75: chr = PS2_UPARROW;     	break;
					case 0x6B: chr = PS2_LEFTARROW;   	break;
					case 0x72: chr = PS2_DOWNARROW;   	break;
					case 0x74: chr = PS2_RIGHTARROW;  	break;
					case 0x4A: chr = '/';             	break;
					case 0x5A: chr = PS2_ENTER; 		break;
					default: break;
				}

			}
            else if (g_vt8242_state & (SCAN_MODE_SHIFT_L | SCAN_MODE_SHIFT_R))
            {
				if (tmp < PS2_KEYMAP_SIZE)
                    chr = g_ps2_keymap_shift[tmp];
			}
            else
            {
				if (tmp < PS2_KEYMAP_SIZE)
                    chr = g_ps2_keymap[tmp];
			}

			if (g_vt8242_key_mode & KEY_MODE_CAPSLOCK)
                chr = toupper(chr);

			g_vt8242_state &= ~(SCAN_MODE_BREAK | SCAN_MODE_MODIFIER);
			if (chr != 0)
            {
				return chr;
			}
		}
	}
}

void vt8242_process_scancode(uint8_t sc)
{
	uint8_t register chr;

#ifdef PS2_DEBUG
    printf("Raw scancode = %02X\n\r", sc);
#endif

    if (sc == 0xF0)
        g_vt8242_state |= SCAN_MODE_BREAK;
    else if (sc == 0xE0)
        g_vt8242_state |= SCAN_MODE_MODIFIER;
    else
    {
        if (g_vt8242_state & SCAN_MODE_BREAK)
        {
            if (sc == 0x12)
                g_vt8242_state &= ~SCAN_MODE_SHIFT_L;
            else if (sc == 0x59)
                g_vt8242_state &= ~SCAN_MODE_SHIFT_R;
            else if (sc == 0x11 && (g_vt8242_state & SCAN_MODE_MODIFIER))
                g_vt8242_state &= ~SCAN_MODE_ALTGR;
            g_vt8242_state &= ~(SCAN_MODE_BREAK | SCAN_MODE_MODIFIER);
            return;
        }

        if (sc == 0x12)
        {
            g_vt8242_state |= SCAN_MODE_SHIFT_L;
            return;
        }
        else if (sc == 0x59)
        {
            g_vt8242_state |= SCAN_MODE_SHIFT_R;
            return;
        }
        else if (sc == 0x11 && (g_vt8242_state & SCAN_MODE_MODIFIER))
        {
            g_vt8242_state |= SCAN_MODE_ALTGR;
        }

        if (sc == 0x58)
        {
            g_vt8242_key_mode ^= KEY_MODE_CAPSLOCK;
            vt8242_set_leds(g_vt8242_key_mode);
        }
        else if (sc == 0x77)
        {
            g_vt8242_key_mode ^= KEY_MODE_NUMLOCK;
            vt8242_set_leds(g_vt8242_key_mode);
        }
        else if (sc == 0x7E)
        {
            g_vt8242_key_mode ^= KEY_MODE_SCROLLLOCK;
            vt8242_set_leds(g_vt8242_key_mode);
        }

        chr = 0;
        if (g_vt8242_state & SCAN_MODE_MODIFIER)
        {
            switch (sc)
            {
                case 0x70: chr = PS2_INSERT;      	break;
                case 0x6C: chr = PS2_HOME;        	break;
                case 0x7D: chr = PS2_PAGEUP;      	break;
                case 0x71: chr = PS2_DELETE;      	break;
                case 0x69: chr = PS2_END;         	break;
                case 0x7A: chr = PS2_PAGEDOWN;    	break;
                case 0x75: chr = PS2_UPARROW;     	break;
                case 0x6B: chr = PS2_LEFTARROW;   	break;
                case 0x72: chr = PS2_DOWNARROW;   	break;
                case 0x74: chr = PS2_RIGHTARROW;  	break;
                case 0x4A: chr = '/';             	break;
                case 0x5A: chr = PS2_ENTER; 		break;
                default: break;
            }

        }
        else if (g_vt8242_state & (SCAN_MODE_SHIFT_L | SCAN_MODE_SHIFT_R))
        {
            if (sc < PS2_KEYMAP_SIZE)
                chr = g_ps2_keymap_shift[sc];
        }
        else
        {
            if (sc < PS2_KEYMAP_SIZE)
                chr = g_ps2_keymap[sc];
        }

        if (g_vt8242_key_mode & KEY_MODE_CAPSLOCK)
            chr = toupper(chr);

        g_vt8242_state &= ~(SCAN_MODE_BREAK | SCAN_MODE_MODIFIER);
        if (chr != 0)
        {
            ring_buf_put(&g_buf_keypress, chr);
        }
	}
}

void vt8242_init_buffers()
{
    ring_buf_init(&g_buf_scancode, g_scancode_data, SCANCODE_BUFFER_SIZE);
    ring_buf_init(&g_buf_keypress, g_keypress_data, KEYPRESS_BUFFER_SIZE);
}
