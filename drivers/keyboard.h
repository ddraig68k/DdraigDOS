#ifndef DDRAIGOS_KEYBOARD_H
#define DDRAIGOS_KEYBOARD_H

#include "ddraig.h"
#include "drivers/sys/vt8242.h"

void enable_keyboard_interrupt();
void disable_keyboard_interrupt();

void process_keyboard();
void keyboard_flush();

static inline void clear_keyboard_config()
{
    vt8242_set_config_byte(0);
}


#endif
