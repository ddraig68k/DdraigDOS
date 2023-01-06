#include "keyboard.h"
#include "drivers/sys/vt8242.h"
#include "libraries/ringbuffer.h"

bool g_has_keyboard = false;

bool init_keyboard()
{
    int retries = 5;

    vt8242_init_buffers();

    while (retries--)
    {
        if (vt8242_init())
        {
            g_has_keyboard = true;
            break;
        }
    }

    return g_has_keyboard;
}

uint8_t keyboard_get_key()
{
    if (!g_has_keyboard)
        return 0;

    return vt8242_get_key();
}

bool keyboard_has_key()
{
    if (!g_has_keyboard)
        return 0;

    return vt8242_has_data();
}

void enable_keyboard_interrupt()
{
    vt8242_enable_port1_interrupt();
}

void disable_keyboard_interrupt()
{
    vt8242_disable_port1_interrupt();
}

void keyboard_flush()
{
    vt8242_flush();
}

void process_keyboard()
{
    if (is_ring_buf_empty(&g_buf_scancode) == false)
    {
        uint8_t ch = ring_buf_get(&g_buf_scancode);
        vt8242_process_scancode(ch);
    }
}
