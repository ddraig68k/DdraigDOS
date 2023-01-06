#include "syscall_sys.h"

#include "drivers/sys/rtc.h"
#include "system/ostimer.h"
#include "drivers/expansion.h"
#include "drivers/display.h"
#include "shell/conio.h"
#include "drivers/sys/keycodes.h"
#include "drivers/sys/duart.h"

int bios_sys_timedate(syscall_data *sys)
{
    if (sys->a0 == NULL)
        return -1;

    debug_print("bios_sys_timedate: Reading time from the RTC\n");
    rtc_date_t *rtc = sys->a0;
    rtc_get_time(rtc);
    return 0;
}

int bios_sys_tickcount(syscall_data *sys)
{
    debug_printf("bios_sys_tickcount: count = %ld\n", get_tick_count());
    sys->d0 = get_tick_count();
    return sys->d0;
}

// Character input/output
int bios_sys_input_has_char(syscall_data *sys)
{
    sys->d0 = has_key();
    return sys->d0;
}

// Reads character(s) from input buffer
// D0 - Number of bytes to read
// A0 - Pointer to buffer
// returns
//      D0 - number of bytes read
int bios_sys_input_read(syscall_data *sys)
{
    int timeout = 1000;
    int charcount = 0;
    uint32_t lasttick = get_tick_count();
    uint8_t *buffer = (uint8_t *)sys->a0;
    while (timeout)
    {
        if (lasttick != get_tick_count())
        {
            lasttick = get_tick_count();
            timeout--;
        }

        if (has_key())
        {
            *buffer++ = get_key();
            charcount++;
            if (charcount == sys->d0)
                break;
            timeout = 1000;
        }
    }

    sys->d0 = charcount;
    return sys->d0;
}

int bios_sys_output_write(syscall_data *sys)
{
    // Not implemented yet
    return 0;
}


// Expansion slot OS support
int bios_sys_get_exp_count(syscall_data *sys)
{
    sys->d0 = num_exp_boards_installed();
    debug_printf("bios_sys_get_exp_count: count = %ld\n", sys->d0);
    return sys->d0;

}

// Returns -1 if no board connected
// Return Board ID, sys->a0 = Board description if board present
int bios_sys_check_exp_id(syscall_data *sys)
{
    uint8_t slot = 0xFF;
    uint8_t id = (uint8_t)sys->d0;

    debug_printf("num_exp_boards_installed: finding id = %ld\n", sys->d0);

    for (uint8_t i = 0; i < MAX_EXP_DEVICES; i++)
        if (read_exp_id(i) == id)
        {
            slot = i;
            break;
        }

    if (slot == 0xFF)
    {
        sys->d0 = -1;
        sys->a0 = NULL;
    }
    else
    {
        sys->d0 = slot;
        sys->a0 = get_expid_description(slot);
    }
    return sys->d0;
}

int bios_sys_expansion_info(syscall_data *sys)
{
    debug_printf("bios_sys_expansion_info: slot num = %ld\n", sys->d0);

    if (sys->d0 >= MAX_EXP_DEVICES)
        return 0;

    sys->a0 = (void *)get_expansion_info((uint8_t)sys->d0);
    debug_printf("bios_sys_expansion_info: expansion addr = %06lx\n", (uint32_t)sys->a0);
    return 1;
}

/* Get the display into struct for the the installed card */

int bios_sys_display_info(syscall_data *sys)
{
    debug_print("bios_sys_display_info\n");

    if (!has_display())
    {
        sys->a0 = NULL;
        return 0;
    }

    sys->a0 = (void *)get_display_info();
    return 1;
}

/* Disable the OS control for the display card */
int bios_sys_disable_display(syscall_data *sys)
{
    debug_print("bios_sys_display_disable\n");

    if (!has_display())
    {
        sys->a0 = NULL;
        return 0;
    }

    g_display.disable_display();
    return 1;
}

/* Disable the OS control for the display card */
int bios_sys_enable_display(syscall_data *sys)
{
    debug_print("bios_sys_display_enable\n");

    g_display.enable_display();
    return 0;
}

int bios_sys_basic_has_char(syscall_data *sys)
{
    debug_print("bios_sys_basic_has_char\n");

    sys->d0 = has_key();
    return sys->d0;
}

void _putchar_serial(char key);

int bios_sys_basic_get_char(syscall_data *sys)
{
    debug_print("bios_sys_basic_get_char\n");

// NOTE:
// This was using get_key() to read they character but it seemed to
// have issues with EhBASIC when it was checking for CTRL-C when
// running. Because get_key blocks it stopped the program running until
// a key was pressed. Needs to be investigated but this issue was notice
// over serial.

    uint8_t key = 0;

    if (keyboard_has_key())
        key = keyboard_get_key();
    else if (serial_has_char())
        key = serial_get_key();

    if (key == 0x7F)        // If DELETE
        key = 0x08;         // change to BACKSPACE
    else if (key == 0x0A)   // If LF
        key = 0x0D;         // change to CR

    sys->d0 = key;
    return sys->d0;
}

int bios_sys_basic_put_char(syscall_data *sys)
{
    debug_print("bios_sys_basic_put_char\n");

    char c = (char)sys->d0;

    _putchar_serial((char)c);

    if (has_display())
    {
        if (c == 0x08) // Backspace
            c = PS2_DELETE;
        if (c != '\r')
            write_char(c);
    }


    return 0;
}

// Reset the computer
int bios_sys_reset(syscall_data *sys)
{
	__asm__ volatile(
	"move.l	0x00f80000, %sp\n"
	"move.l 0x00f80004, %a0\n"
    "jmp    (%a0)"
	);
    // Should never return
    return 0;
}
