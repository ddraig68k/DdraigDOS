#include "display.h"

#include <malloc.h>
#include <string.h>
#include <printf.h>

#include "expansion.h"
#include "drivers/video/v9958.h"
#include "drivers/video/tms9918.h"
#include "drivers/video/gfxfpga.h"
#include "drivers/video/v9990.h"

#include "drivers/sys/keycodes.h"
#include "drivers/sys/duart.h"

#include "system/ostimer.h"

display_t g_display;
bool g_has_display = false;

void cursor_timer_callback(uint32_t count)
{
    toggle_cursor();
}

bool init_display(expansion_dev_t *slot)
{
    bool initialised = false;
    memset((void *)&g_display, 0, sizeof(display_t));

    g_display.base_address = slot->exp_base_address;

    switch (slot->device_id)
    {
        case DEVICE_TMS9918:
            tms9918_init_driver(&g_display);
            initialised = g_display.drv_initialise_display(&g_display);
            g_display.display_type = DEVICE_TMS9918;
            break;

        case DEVICE_FPGA:
            gfxfpga_init_driver(&g_display);
            initialised = g_display.drv_initialise_display(&g_display);
            g_display.display_type = DEVICE_FPGA;
            break;

        case DEVICE_V9990:
            v9990_init_driver(&g_display);
            initialised = g_display.drv_initialise_display(&g_display);
            g_display.display_type = DEVICE_V9990;
            break;

        case DEVICE_V9958:
            v9958_init_driver(&g_display);
            initialised = g_display.drv_initialise_display(&g_display);
            g_display.display_type = DEVICE_V9958;
            break;
    }

    if (initialised)
    {
        // Set the driver for the expansion slot
        slot->exp_driver = (void *)&g_display;

        g_display.disable_display = os_disable_display;
        g_display.enable_display = os_enable_display;

        // Set up the display
        uint16_t bufsize = g_display.display_width * g_display.display_height;
        g_display.display_buffer = malloc(bufsize);
        g_display.attrib_buffer = malloc(bufsize);
        memset(g_display.display_buffer, 0, bufsize);
        memset(g_display.attrib_buffer, 0, bufsize);
        reset_display();

        add_timer(6, true, cursor_timer_callback);

    }

    g_has_display = initialised;

    return initialised;
}

display_t *get_display_info()
{
    return &g_display;
}

void reset_display()
{
    clear_screen();
    g_display.position_x = 0;
    g_display.position_y = 0;
    g_display.current_attribute = 0;
}

void os_disable_display()
{
    g_display.cursor_on = false;
    g_has_display = false;
    if (g_display.drv_interrupt_disable)
        g_display.drv_interrupt_disable(&g_display);
}

void os_enable_display()
{
    if (g_display.display_type != 0)
    {
        g_display.drv_initialise_display(&g_display);
        debug_print("os_enable_display: Initialised display\n");
        g_has_display = true;
        g_display.cursor_on = true;
        if (g_display.drv_interrupt_enable)
            g_display.drv_interrupt_enable(&g_display);

        g_display.drv_copy_buffer(&g_display);
        debug_print("os_enable_display: Updated display buffer\n");
    }
    else
    {
        debug_print("os_enable_display: No display detected\n");
    }
}


void clear_screen()
{
    for (int i = 0; i < g_display.display_width * g_display.display_height; i++)
        g_display.display_buffer[i] = 0;

    g_display.drv_clear_display(&g_display);
}

void set_attribute(uint8_t attribute)
{
    g_display.current_attribute = attribute;
}

void set_attribute_at(uint16_t x, uint16_t y, uint8_t attribute)
{

}

void set_position(uint16_t x, uint16_t y)
{
    g_display.position_x = x;
    g_display.position_y = y;
}

void get_position(uint16_t *x, uint16_t *y)
{
    *x = g_display.position_x;
    *y = g_display.position_y;
}

void write_buffer_text(uint16_t x, uint16_t y, uint8_t *text, uint16_t len)
{
    if (x >= g_display.display_width || y >= g_display.display_height)
        return;

    uint16_t pos  = (y * g_display.display_width) + x;
    g_display.position_x = x;
    g_display.position_y = y;

    for (uint16_t i = 0; i < len; i++)
    {
        uint8_t key = *text++;
        if (key == PS2_ENTER || key == '\n')
        {
            if (g_has_display)
                g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, 0);

            g_display.position_y++;
            g_display.position_x = 0;
        }
        else if (key == '\r')
        {
            g_display.position_x = 0;
        }
        else if (key == PS2_BACKSPACE)
        {
            if (g_has_display && g_display.cursor_state)
            {
                g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, ' ');
            }

            if (g_display.position_x == 0)
            {
                if (g_display.position_y > 0)
                {
                    g_display.position_x = g_display.display_width - 1;
                    g_display.position_y--;
                }
            }
            else
                g_display.position_x--;

            pos  = (g_display.position_y * g_display.display_width) + g_display.position_x;
            g_display.display_buffer[pos] = 0;

            if (g_has_display)
                g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, ' ');
        }
        else
        {
            g_display.display_buffer[pos++] = key;

            if (g_has_display)
                g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, key);

            g_display.position_x++;
        }

        if (g_display.position_x == g_display.display_width)
        {
            g_display.position_y++;
            g_display.position_x = 0;
        }
        // If it's at the end of the buffer, scroll data up and clear next line
        if (g_display.position_y == g_display.display_height)
        {
            uint8_t register *src = g_display.display_buffer + g_display.display_width;
            uint8_t register *dest = g_display.display_buffer;
            uint16_t bytes = (g_display.display_width * g_display.display_height) - g_display.display_width;

            for (uint16_t j = 0; j < bytes; j++)
                *dest++ = *src++;

            // Clear the last line of data
            for (uint16_t j = 0; j < g_display.display_width; j++)
                *dest++ = 0;

            if (g_has_display)
                g_display.drv_copy_buffer(&g_display);

            g_display.position_y--;
        }
    }
    g_display.buffer_changed = true;
}

void write_text(char *text)
{
    write_buffer_text(g_display.position_x, g_display.position_y, (uint8_t *)text, strlen(text));
    //g_display.copy_buffer(&g_display);
}

void write_text_at(uint16_t x, uint16_t y, char *text)
{
    write_buffer_text(x, y, (uint8_t *)text, strlen(text));
    //g_display.copy_buffer(&g_display);
}

void write_char(char ch)
{
    write_buffer_text(g_display.position_x, g_display.position_y, (uint8_t *)&ch, 1);
    //g_display.copy_buffer(&g_display);
}

void write_char_at(uint16_t x, uint16_t y, char ch)
{
    write_buffer_text(x, y, (uint8_t *)&ch, 1);
    //g_display.copy_buffer(&g_display);
}


void display_refresh()
{
    if (g_has_display)
        g_display.drv_copy_buffer(&g_display);
}

void enable_cursor()
{
    g_display.cursor_on = true;
}

void disable_cursor()
{
    g_display.cursor_on = true;
}

void toggle_cursor()
{
    if (g_display.cursor_on && g_has_display)
    {
        if (g_display.cursor_state)
            g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, g_display.cursor_off_char);
        else
            g_display.drv_write_char(&g_display, g_display.position_x, g_display.position_y, g_display.cursor_on_char);
        g_display.cursor_state = !g_display.cursor_state;
    }
}
