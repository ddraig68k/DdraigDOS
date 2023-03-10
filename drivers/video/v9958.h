#ifndef DDRAIG_OS_V9958_DRIVER_H
#define DDRAIG_OS_V9958_DRIVER_H

#include "ddraig.h"
#include "drivers/display.h"

enum vdp_screen_mode
{
    V9958_SCREEN_MODE_GRAPHIC_1   = 0, // 00000b (00) SCREEN 1
    V9958_SCREEN_MODE_TEXT_1      = 1, // 00001b (01) SCREEN 0: WIDTH 40
    V9958_SCREEN_MODE_MULTI_COLOR = 2, // 00010b (02) SCREEN 3
    V9958_SCREEN_MODE_GRAPHIC_2   = 3, // 00100b (04) SCREEN 2
    V9958_SCREEN_MODE_GRAPHIC_3   = 4, // 01000b (08) SCREEN 4
    V9958_SCREEN_MODE_TEXT_2      = 5, // 01001b (09) SCREEN 0: WIDTH 80
    V9958_SCREEN_MODE_GRAPHIC_4   = 6, // 01100b (0C) SCREEN 5
    V9958_SCREEN_MODE_GRAPHIC_5   = 7, // 10000b (10) SCREEN 6
    V9958_SCREEN_MODE_GRAPHIC_6   = 8, // 10100b (14) SCREEN 7
    V9958_SCREEN_MODE_GRAPHIC_7   = 9  // 11100b (1C) SCREEN 8
};

enum vdp_screen_lines
{
    V9958_SCREEN_LINES_192 = 0x00,
    V9958_SCREEN_LINES_212 = 0x80
};

enum vdp_sprite_size
{
    V9958_SPRITE_SIZE_8x8 = 0,
    V9958_SPRITE_SIZE_8x8_MAGNIFIED = 1,
    V9958_SPRITE_SIZE_16x16 = 2,
    V9958_SPRITE_SIZE_16x16_MAGNIFIED = 3
};

void v9958_init_driver(display_t *display);

#endif
