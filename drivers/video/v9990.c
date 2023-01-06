#include "v9990.h"
#include "drivers/display.h"
#include "v9990_font.h"

#include <printf.h>

#define V9990_SCREEN_WIDTH 	    512
#define V9990_SCREEN_HEIGHT 	424

uint8_t     g_v9990_int_en = 0;
display_t   *g_v9990_display_struct = NULL;

void __attribute__((interrupt)) v9990_vblank_int(void)
{
    V9990_WRITE(V9990_INT_FLAG, 0x07);
}

void v9990_clear_display(display_t *display)
{
    V9990_BOX screen;
	screen.top = 0;
	screen.left = 0;
	screen.width = V9990_SCREEN_WIDTH;
	screen.height = V9990_SCREEN_HEIGHT;

    v9990_DrawFilledBox(&screen, 0);
    v9990_CmdWait();
    v9990_SetCmdColor(0xffff);
}

bool v9990_initialise_display(display_t *display)
{
    if (display->base_address == 0)
        return false;

    v9990_Reset();
    v9990_Init(display->base_address);
	v9990_SetScreenMode(V9990_MODE_B3, V9990_BPP16, V9990_WIDTH512, V9990_INTERLACE, V9990_PAL, 0);
	v9990_SpritesDisable();
	v9990_DisplayEnable();
    v9990_SetScrollX(0);
    v9990_SetScrollY(0);

    V9990_WRITE_REG(V9K_ARG, 0);
    V9990_WRITE_REG(V9K_LOP, V9K_LOP_WCSC);

	v9990_SetCmdWriteMask(0xffff);

    v9990_clear_display(display);

	// Copy the font data to VRAM
	v9990_SetVramWrite(0x70000);
	v9990_CopyRamToVram((uint8_t *)vff_font_data, VFF_FONT_DATASIZE);

	v9990_SetCmdBackColor(0);
	v9990_SetCmdColor(0xffff);

    g_v9990_display_struct = display;

    return true;
}

void v9990_copy_buffer(display_t *display)
{
    uint8_t *buffer = display->display_buffer;
    if (buffer == NULL)
    {
        printf("Display buffer not initialised");
        return;
    }

    int width = display->display_width;
    int height = display->display_height;

    //v9990_clear_display(display);
	v9990_SetCmdColor(0xffff);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
		    uint8_t ch = buffer[(y * width) + x];
            if (ch == 0)
                ch = ' ';

            uint16_t index = vff_offset_data[ch];

            while ((V9990_READ(V9990_STATUS) & V9K_STATUS_CE) != 0)
            {}

            V9990_BOX tocopy;
            tocopy.top = y * VFF_FONT_HEIGHT;
            tocopy.left = x * VFF_FONT_WIDTH;
            tocopy.width = VFF_FONT_WIDTH;
            tocopy.height = VFF_FONT_HEIGHT;
            v9990_CopyVRamCharToXY(&tocopy, 0x70000 + index);
        }
    }

}

void v9990_write_text(display_t *display, uint16_t x, uint16_t y, uint8_t *text, uint16_t len)
{
    while (len--)
    {
        uint8_t ch = *text++;
        uint16_t index = vff_offset_data[ch];

        while ((V9990_READ(V9990_STATUS) & V9K_STATUS_CE) != 0)
        {}

        if (x > display->display_width)
        {
            y++;
            x = 0;
        }

        V9990_BOX tocopy;
        tocopy.top = y * VFF_FONT_HEIGHT;
        tocopy.left = x * VFF_FONT_WIDTH;
        tocopy.width = VFF_FONT_WIDTH;
        tocopy.height = VFF_FONT_HEIGHT;
        v9990_CopyVRamCharToXY(&tocopy, 0x70000 + index);
        x++;
    }
}

void v9990_write_char(display_t *display, uint16_t x, uint16_t y, uint8_t ch)
{
    uint16_t index = vff_offset_data[ch];

    while ((V9990_READ(V9990_STATUS) & V9K_STATUS_CE) != 0)
    {}

    if (x > display->display_width)
    {
        y++;
        x = 0;
    }

    V9990_BOX tocopy;
    tocopy.top = y * VFF_FONT_HEIGHT;
    tocopy.left = x * VFF_FONT_WIDTH;
    tocopy.width = VFF_FONT_WIDTH;
    tocopy.height = VFF_FONT_HEIGHT;
    v9990_CopyVRamCharToXY(&tocopy, 0x70000 + index);
}

void v9990_init_driver(display_t *display)
{
    // Set the base register address
    g_v9990_base_address = display->base_address;

    // Set the display parameters
    display->display_width = V9990_SCREEN_WIDTH / VFF_FONT_WIDTH;
    display->display_height = V9990_SCREEN_HEIGHT / VFF_FONT_HEIGHT;
    display->cursor_on = false;
    display->cursor_state = false;
    display->cursor_on_char = '_';
    display->cursor_off_char = ' ';
    display->drv_initialise_display = v9990_initialise_display;
    display->drv_clear_display = v9990_clear_display;
    display->drv_copy_buffer = v9990_copy_buffer;
    display->drv_write_text = v9990_write_text;
    display->drv_write_char = v9990_write_char;
}
