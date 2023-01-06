#include "conio.h"
#include "drivers/sys/keycodes.h"
#include "drivers/sys/duart.h"
#include "drivers/display.h"
#include <printf.h>
#include <string.h>
#include "system/debug.h"

uint8_t serial_get_key()
{
    uint8_t key = serial_get();

    if (key == '\b' || key == 0x7f)
        return PS2_DELETE;
    else if (key == '\r' || key == '\n')
        return PS2_ENTER;

    return key;
}

uint8_t get_key()
{
    uint8_t key = 0xFF;

    while (key == 0xFF)
    {
        if (keyboard_has_key())
            key = keyboard_get_key();
        else if (serial_has_char())
            key = serial_get_key();
    }

    return key;
}

bool has_key()
{
    return serial_has_char() || keyboard_has_key();
}

void _putchar(char c)
{
    _putchar_serial((char)c);

    if (has_display())
        write_char(c);
}

void _putchar_hex(char c)
{
    printf("%02X", (int)c);
}
