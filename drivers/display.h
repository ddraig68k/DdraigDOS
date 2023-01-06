#ifndef DDRAIG_OS_DISPLAY_H
#define DDRAIG_OS_DISPLAY_H

#include "ddraig.h"
#include "expansion.h"

struct display_device;

typedef struct display_device
{
    uint16_t display_width;
    uint16_t display_height;
    uint16_t position_x;
    uint16_t position_y;
    uint16_t current_attribute;

    uint8_t *display_buffer;
    uint8_t *attrib_buffer;
    uint32_t base_address;

    bool (*drv_initialise_display)(struct display_device *display);
    void (*drv_clear_display)(struct display_device *display);
    void (*drv_copy_buffer)(struct display_device *display);
    void (*drv_write_text)(struct display_device *display, uint16_t x, uint16_t y, uint8_t *text, uint16_t len);
    void (*drv_write_char)(struct display_device *display, uint16_t x, uint16_t y, uint8_t ch);
    void (*drv_interrupt_enable)(struct display_device *display);
    void (*drv_interrupt_disable)(struct display_device *display);

    void (*disable_display)(void);
    void (*enable_display)(void);

    uint8_t display_type;
    bool cursor_on;
    bool cursor_state;
    bool buffer_changed;
    uint8_t cursor_on_char;
    uint8_t cursor_off_char;
} display_t;

extern display_t g_display;
extern bool g_has_display;

bool init_display(expansion_dev_t *slot);

display_t *get_display_info();

void reset_display();
void os_disable_display();
void os_enable_display();
void clear_screen();
void toggle_cursor();

void set_attribute(uint8_t attribute);
void set_attribute_at(uint16_t x, uint16_t y, uint8_t attribute);

void set_position(uint16_t x, uint16_t y);
void get_position(uint16_t *x, uint16_t *y);

void write_text(char *text);
void write_text_at(uint16_t x, uint16_t y, char *text);
void write_char(char ch);
void write_char_at(uint16_t x, uint16_t y, char ch);

static inline bool has_display()
{
    return g_has_display;
}

static inline bool has_display_changed()
{
    return g_display.buffer_changed;
}

void display_refresh();
void enable_cursor();
void disable_cursor();


#endif
