#ifndef DDRAIG_OS_CONIO_H
#define DDRAIG_OS_CONIO_H

#include "ddraig.h"

bool init_keyboard();

uint8_t keyboard_get_key();
bool keyboard_has_key();
uint8_t serial_get_key();

uint8_t get_key();
bool has_key();

void _putchar(char c);

#endif
