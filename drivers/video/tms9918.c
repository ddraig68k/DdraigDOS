#include "tms9918.h"
#include "printf.h"

uint8_t *g_tms9918_base = NULL;

#include "tms9918_font.h"

#define	PORT_DATA       *((volatile uint8_t  *) (g_tms9918_base + 0x00))
#define	PORT_REGISTER   *((volatile uint8_t  *) (g_tms9918_base + 0x02))

uint8_t g_tms_colors = (TMS_CYAN << 4) | TMS_TRANSPARENT;

void tms9918_delay(uint8_t delay)
{
    volatile uint8_t d = delay;
    while (d--);
}

void tms9918_write_register(uint8_t reg, uint8_t data)
{
    PORT_REGISTER = data;
    tms9918_delay(1);
    PORT_REGISTER = reg;
    tms9918_delay(1);
}

void tms9918_write_data(uint8_t data)
{
    PORT_DATA = data;
    tms9918_delay(1);
}

void tms9918_write_text_data(uint8_t data)
{
    PORT_DATA = data;
    tms9918_delay(1);
}

static void tms9918_background(uint8_t color)
{
    g_tms_colors = (g_tms_colors & 0xF0) | color;
    tms9918_write_register(TMS_COLOR_REG, g_tms_colors);
}

static void tms9918_text_color(uint8_t color)
{
    g_tms_colors = (g_tms_colors & 0x0F) | (color << 4);
    tms9918_write_register(TMS_COLOR_REG, g_tms_colors);
}

void tms9918_set_write_addr(uint16_t addr)
{
    uint8_t upper = (uint8_t)(addr >> 8);
    uint8_t lower = (uint8_t)addr;
    upper &= 0x3F;
    upper |= TMS_WRITE_BIT;
    PORT_REGISTER = lower;
    tms9918_delay(1);
    PORT_REGISTER = upper;
    tms9918_delay(1);
}

void tms9918_set_read_addr(uint16_t addr)
{
    uint8_t upper = (uint8_t)(addr >> 8);
    uint8_t lower = (uint8_t)addr;
    upper &= 0x3F;
    PORT_REGISTER = lower;
    tms9918_delay(1);
    PORT_REGISTER = upper;
    tms9918_delay(1);
}

void tms9918_fill_data(uint8_t val, uint16_t addr, uint16_t len)
{
    tms9918_set_write_addr(addr);
    for (int i = 0; i < len; i++)
        tms9918_write_data(val);
}

void tms9918_write(uint8_t *src, uint16_t addr, uint16_t len)
{
    tms9918_set_write_addr(addr);
    for (int i = 0; i < len; i++)
        tms9918_write_data(*src++);
}

void tms9918_text_pos(uint8_t x, uint8_t y)
{
    uint16_t pos = ((uint16_t)y * 40) + x;
    tms9918_set_write_addr(TMS_TX_NAME_TABLE + pos);
}

void tms9918_write_text(char *text)
{
    while (*text != 0)
        tms9918_write_data(*text++);
}

void tms9918_reset(void)
{
    g_tms_colors = (TMS_TRANSPARENT << 4) | TMS_BLACK;

    // Disable the display
    tms9918_write_register(TMS_CTRL0_REG, TMS_R0_EXT_VDP_DISABLE);
    tms9918_write_register(TMS_CTRL1_REG, TMS_R1_RAM_16K);

    // Clear the entire ram
    tms9918_fill_data(0, 0, 0x4000);
}

void tms9918_init_text_mode()
{
    tms9918_reset();
    tms9918_write((uint8_t *)&tms_font, TMS_TX_PATTERN_TABLE, 2048);

    tms9918_write_register(TMS_CTRL0_REG, TMS_R0_EXT_VDP_DISABLE | TMS_R0_MODE_TEXT);
    tms9918_write_register(TMS_CTRL1_REG, TMS_R1_RAM_16K | TMS_R1_DISP_ACTIVE | TMS_R1_MODE_TEXT );
    tms9918_write_register(TMS_NAME_REG, TMS_TX_NAME_TABLE >> 10);
    tms9918_write_register(TMS_COLOR_TAB_REG, 0);
    tms9918_write_register(TMS_PATTERN_REG, TMS_TX_PATTERN_TABLE >> 11);
    tms9918_write_register(TMS_SPRITE_ATTR_REG, 0);
    tms9918_write_register(TMS_SPRITE_PATTERN_REG, 0);
    tms9918_write_register(TMS_COLOR_REG, g_tms_colors);
}

bool tms9918_initialise_display(display_t *display)
{
    if (g_tms9918_base == NULL)
        return false;

    tms9918_init_text_mode();
    tms9918_background(TMS_BLACK);
    tms9918_text_color(TMS_WHITE);

    return true;
}

void tms9918_clear_display(display_t *display)
{
    int width = display->display_width;
    int height = display->display_height;

    tms9918_fill_data(0, TMS_TX_NAME_TABLE, width * height);
}

void tms9918_copy_buffer(display_t *display)
{
    int width = display->display_width;
    int height = display->display_height;

    uint8_t *buffer = display->display_buffer;
    if (buffer == NULL)
    {
        printf("Display buffer not initialised");
        return;
    }

    tms9918_write(buffer, TMS_TX_NAME_TABLE, width * height);
}

void tms9918_direct_write_text(display_t *display, uint16_t x, uint16_t y, uint8_t *text, uint16_t len)
{
    tms9918_text_pos(x, y);
    tms9918_write_text((char *)text);
}

void tms9918_direct_write_char(display_t *display, uint16_t x, uint16_t y, uint8_t ch)
{
    tms9918_text_pos(x, y);
    tms9918_write_data(ch);
}

void tms9918_init_driver(display_t *display)
{
    // Set the base register address
    g_tms9918_base = (uint8_t *)display->base_address;

    // Set the display parameters
    display->display_width = 40;
    display->display_height = 24;
    display->cursor_on = false;
    display->cursor_state = false;
    display->cursor_on_char = '_';
    display->cursor_off_char = ' ';
    display->drv_initialise_display = tms9918_initialise_display;
    display->drv_clear_display = tms9918_clear_display;
    display->drv_copy_buffer = tms9918_copy_buffer;
    display->drv_write_text = tms9918_direct_write_text;
    display->drv_write_char = tms9918_direct_write_char;
}
