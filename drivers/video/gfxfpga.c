#include "gfxfpga.h"
#include "printf.h"

uint8_t *g_gfxfpga_base = NULL;

#define VGA_REG_WRITE(x, y)  (*((uint16_t *) (g_gfxfpga_base + (x))) = (y))
#define VGA_REG_READ(x)      (*((volatile uint16_t *) (g_gfxfpga_base + (x))))

void gfxfpga_write_control_reg(uint16_t data)
{
    VGA_REG_WRITE(REG_CONTROL, data);
}

void gfxfpga_set_text_area(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    VGA_REG_WRITE(REG_PARAM_DATA0, x0);
    VGA_REG_WRITE(REG_PARAM_DATA1, y0);
    VGA_REG_WRITE(REG_PARAM_DATA2, x1);
    VGA_REG_WRITE(REG_PARAM_DATA3, y1);

	VGA_REG_WRITE(REG_COMMAND, CMD_SET_TEXTAREA);
}

void gfxfpga_wait_ready()
{
	uint16_t status;
	do
    {
        status = VGA_REG_READ(REG_STATUS);
    }
    while ((status & 0x0001) == 0);
}

void gfxfpga_wait_vblank()
{
	uint16_t status;
	do
    {
        status = VGA_REG_READ(REG_STATUS);
    }
    while ((status & 0x8000) == 0);
}

void gfxfpga_clear_text()
{
    int count = 2400;
    VGA_REG_WRITE(REG_PARAM_DATA0, 0);

    while (count--)
    {
        VGA_REG_WRITE(REG_PARAM_DATA1, ' ');
        VGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
    }
}

void gfxfpga_write_text(uint16_t posx, uint16_t posy, char *text)
{
    uint16_t pos = (posy * 80) + posx;

    VGA_REG_WRITE(REG_PARAM_DATA0, pos);
    while (*text != 0)
    {
        VGA_REG_WRITE(REG_PARAM_DATA1, *text++);
        VGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
    }
}

void gfxfpga_write_char(uint16_t posx, uint16_t posy, char text)
{
    uint16_t pos = (posy * 80) + posx;

    VGA_REG_WRITE(REG_PARAM_DATA0, pos);
    VGA_REG_WRITE(REG_PARAM_DATA1, text);
    VGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
}

void gfxfpga_clear_screen(uint16_t color)
{
	VGA_REG_WRITE(REG_PARAM_COLOR, color);
	VGA_REG_WRITE(REG_COMMAND, CMD_CLEAR_SCREEN);
}

void gfxfpga_write_text_color(uint16_t color)
{
    VGA_REG_WRITE(REG_PARAM_DATA0, color);
    VGA_REG_WRITE(REG_COMMAND, CMD_SET_TEXTCOLOR);
}

void gfxfpga_init_text_mode()
{
    uint16_t textcolor = 0x0FFF;
    gfxfpga_write_control_reg(0);   // Set text mode display
    gfxfpga_clear_screen(0);
    gfxfpga_clear_text();
    gfxfpga_set_text_area(0, 0, 640, 480);
    gfxfpga_write_text_color(textcolor);

}

bool gfxfpga_initialise_display(display_t *display)
{
    if (g_gfxfpga_base == NULL)
        return false;

    gfxfpga_init_text_mode();

    return true;
}

void gfxfpga_clear_display(display_t *display)
{
    gfxfpga_clear_text();
}

void gfxfpga_copy_buffer(display_t *display)
{
    int width = display->display_width;
    int height = display->display_height;

    uint8_t register *buffer = display->display_buffer;

    VGA_REG_WRITE(REG_PARAM_DATA0, 0);
    for (uint16_t i = 0; i < width * height; i++)
    {
        VGA_REG_WRITE(REG_PARAM_DATA1, (uint16_t)*buffer++);
        VGA_REG_WRITE(REG_COMMAND, CMD_SET_CHARACTER);
    }
}

void gfxfpga_direct_write_text(display_t *display, uint16_t x, uint16_t y, uint8_t *text, uint16_t len)
{
    gfxfpga_write_text(x, y, (char *)text);
}

void gfxfpga_direct_write_char(display_t *display, uint16_t x, uint16_t y, uint8_t ch)
{
    gfxfpga_write_char(x, y, ch);
}

void gfxfpga_init_driver(display_t *display)
{
    // Set the base register address
    g_gfxfpga_base = (uint8_t *)display->base_address;

    // Set the display parameters
    display->display_width = 80;
    display->display_height = 30;
    display->cursor_on = false;
    display->cursor_state = false;
    display->cursor_on_char = 0x16;
    display->cursor_off_char = 0;
    display->drv_initialise_display = gfxfpga_initialise_display;
    display->drv_clear_display = gfxfpga_clear_display;
    display->drv_copy_buffer = gfxfpga_copy_buffer;
    display->drv_write_text = gfxfpga_direct_write_text;
    display->drv_write_char = gfxfpga_direct_write_char;
}
