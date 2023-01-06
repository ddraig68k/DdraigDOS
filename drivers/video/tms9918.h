#ifndef DDRAIG_OS_TMS9918_DRIVER_H
#define DDRAIG_OS_TMS9918_DRIVER_H

#include "ddraig.h"
#include "drivers/display.h"

#define TMS_REG0                    0x80
#define TMS_REG1                    0x81
#define TMS_REG2                    0x82
#define TMS_REG3                    0x83
#define TMS_REG4                    0x84
#define TMS_REG5                    0x85
#define TMS_REG6                    0x86
#define TMS_REG7                    0x87

#define TMS_CTRL0_REG               TMS_REG0
#define TMS_CTRL1_REG               TMS_REG1
#define TMS_NAME_REG                TMS_REG2
#define TMS_COLOR_TAB_REG           TMS_REG3
#define TMS_PATTERN_REG             TMS_REG4
#define TMS_SPRITE_ATTR_REG         TMS_REG5
#define TMS_SPRITE_PATTERN_REG      TMS_REG6
#define TMS_COLOR_REG               TMS_REG7

#define TMS_WRITE_BIT               0x40

// VRAM addresses
#define TMS_TX_PATTERN_TABLE        0x0000
#define TMS_TX_NAME_TABLE           0x0800

#define TMS_G1_SPRITE_PATTERN       0x0000
#define TMS_G1_PATTERN              0x0800
#define TMS_G1_SPRITE_ATTR          0x1000
#define TMS_G1_NAME_TABLE           0x1400
#define TMS_G1_COLOR_TABLE          0x2000

#define TMS_G2_PATTERN              0x0000
#define TMS_G2_SPRITE_PATTERN       0x1800
#define TMS_G2_COLOR_TABLE          0x2000
#define TMS_G2_NAME_TABLE           0x3800
#define TMS_G2_SPRITE_ATTR          0x3B00

#define TMS_MC_SPRITE_PATTERN       0x0000
#define TMS_MC_PATTERN              0x0800
#define TMS_MC_SPRITE_ATTR          0x1000
#define TMS_MC_NAME_TABLE           0x1400
#define TMS_MC_COLOR_TABLE          0x1700

// Register values
#define TMS_R0_MODE_GRAPHICS_I      0x00
#define TMS_R0_MODE_GRAPHICS_II     0x02
#define TMS_R0_MODE_MULTICOLOR      0x00
#define TMS_R0_MODE_TEXT            0x00
#define TMS_R0_EXT_VDP_ENABLE       0x01
#define TMS_R0_EXT_VDP_DISABLE      0x00

#define TMS_R1_RAM_16K              0x80
#define TMS_R1_RAM_4K               0x00
#define TMS_R1_DISP_BLANK           0x00
#define TMS_R1_DISP_ACTIVE          0x40
#define TMS_R1_INT_ENABLE           0x20
#define TMS_R1_INT_DISABLE          0x00
#define TMS_R1_MODE_GRAPHICS_I      0x00
#define TMS_R1_MODE_GRAPHICS_II     0x00
#define TMS_R1_MODE_MULTICOLOR      0x08
#define TMS_R1_MODE_TEXT            0x10
#define TMS_R1_SPRITE_8             0x00
#define TMS_R1_SPRITE_16            0x02
#define TMS_R1_SPRITE_MAG1          0x00
#define TMS_R1_SPRITE_MAG2          0x01

// Color palette
enum tmscolors {
    TMS_TRANSPARENT,
    TMS_BLACK,
    TMS_MEDGREEN,
    TMS_LIGHTGREEN,
    TMS_DARKBLUE,
    TMS_LIGHTBLUE,
    TMS_DARKRED,
    TMS_CYAN,
    TMS_MEDRED,
    TMS_LIGHTRED,
    TMS_DARKYELLOW,
    TMS_LIGHTYELLOW,
    TMS_DARKGREEN,
    TMS_MAGENTA,
    TMS_GRAY,
    TMS_WHITE
};

void tms9918_init_driver(display_t *display);

#endif
