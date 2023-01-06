#include "v9958.h"
#include "drivers/display.h"
#include "font8x8.h"

#include <printf.h>

uint32_t g_v9958_base = 0;

#define	PORT_RWDATA      (*((volatile uint8_t *) (g_v9958_base)))
#define	PORT_WREG_SETUP  (*((volatile uint8_t *) (g_v9958_base + 0x02)))
#define	PORT_WPALETTE    (*((volatile uint8_t *) (g_v9958_base + 0x04)))
#define	PORT_WREG_DATA   (*((volatile uint8_t *) (g_v9958_base + 0x06)))

static inline void ll_v9958_init_textmode()
{
	__asm__ volatile
    (
        "move.l	%0, %%a0\n\t"
        "jsr v9958_ll_init_textmode\n\t"
        : /* no outputs */
        : "g" (g_v9958_base)
        : "%a0" /* clobbered registers */
	);
}

void ll_v9958_clear_display()
{
	__asm__ __volatile__
    (
        "move.l	%0, %%a0\n\t"
        "jsr v9958_ll_clear_display\n\t"
        : /* no outputs */
        : "g" (g_v9958_base)
        : "%a0" /* clobbered registers */
	);

}

void ll_v9958_set_write_address(uint32_t address)
{
    __asm__ __volatile__
    (
        "move.l	%0, %%a0\n\t"
        "move.l	%1, %%d0\n\t"
        "jsr v9958_ll_set_write_address\n\t"
        : /* no outputs */
        : "g" (g_v9958_base), "g" (address)
        : "%d0", "%a0" /* clobbered registers */
	);
}

void ll_v9958_write_data(uint8_t *buffer, uint32_t size)
{
    __asm__ __volatile__
    (
        "move.l	%0, %%a0\n\t"
        "move.l	%1, %%a1\n\t"
        "move.l	%2, %%d0\n\t"
        "jsr v9958_ll_write_data\n\t"
        : /* no outputs */
        : "g" (g_v9958_base), "g" (buffer), "g" (size + 1)
        : "%d0", "%a0", "%a1"/* clobbered registers */
	);
}


void v9958_delay()
{
    volatile uint8_t d = 2;
    while (d--);
}

void v9958_write_register(uint8_t reg, uint8_t data)
{
    // Write 1: bits 16-14
    // Write 2: bits 7-0
    // Write 3: bits 13-8, $40 is write-enable
    PORT_WREG_SETUP = data;
    v9958_delay();
    PORT_WREG_SETUP = reg | 0x80;
    v9958_delay();
}

void v9958_write_address(uint32_t addr)
{
    v9958_write_register(0x0E,(uint8_t)((addr >> 14) & 0x07));
    PORT_WREG_SETUP = (uint8_t)(addr & 0x00FF);
    v9958_delay();
    PORT_WREG_SETUP = (uint8_t)((addr >> 8) & 0x003F) | 0x40;   // VRAM A8-A13 and write enable
    v9958_delay();
    v9958_delay();
    v9958_delay();
}


void v9958_write_data(uint32_t address, uint8_t *data, uint16_t size)
{
/*
    v9958_write_address(address);
    for (int i = 0; i < size; i++)
    {
        PORT_RWDATA = *data++;
        v9958_delay();
        v9958_delay();
        v9958_delay();
    }
*/
    ll_v9958_set_write_address(address);
    ll_v9958_write_data(data, size);
}

void __attribute__((interrupt)) v9958_vblank_int(void)
{
    v9958_write_register(0x0F, 0x00);       //  Write 0 (SR#0)...
    volatile __attribute__((unused)) uint8_t data = PORT_WREG_SETUP; //And read status into it (clear interrupt flag)
}


bool v9958_initialise_display(display_t *display)
{
    if (g_v9958_base == 0)
        return false;

    ISR_VECT_EXP1(v9958_vblank_int);
    ll_v9958_init_textmode();

    return true;
}

void v9958_clear_display(display_t *display)
{
    ll_v9958_set_write_address(0);
    ll_v9958_write_data(display->display_buffer, display->display_width * display->display_height);

}

void v9958_copy_buffer(display_t *display)
{
    int width = display->display_width;
    int height = display->display_height;

    uint8_t *buffer = display->display_buffer;
    if (buffer == NULL)
    {
        printf("Display buffer not initialised");
        return;
    }

    ll_v9958_set_write_address(0);
    ll_v9958_write_data(buffer, width * height);
}

void v9958_write_text(display_t *display, uint16_t x, uint16_t y, uint8_t *text, uint16_t len)
{
}

void v9958_write_char(display_t *display, uint16_t x, uint16_t y, uint8_t ch)
{
    uint16_t address = (y * display->display_width) + x;
    ll_v9958_set_write_address(address);
    PORT_RWDATA = ch;
}

void v9958_init_driver(display_t *display)
{
    // Set the base register address
    g_v9958_base = display->base_address;

    // Set the display parameters
    display->display_width = 80;
    display->display_height = 24;
    display->cursor_on = false;
    display->cursor_state = false;
    display->cursor_on_char = '_';
    display->cursor_off_char = ' ';
    display->drv_initialise_display = v9958_initialise_display;
    display->drv_clear_display = v9958_clear_display;
    display->drv_copy_buffer = v9958_copy_buffer;
    display->drv_write_text = v9958_write_text;
    display->drv_write_char = v9958_write_char;
}
