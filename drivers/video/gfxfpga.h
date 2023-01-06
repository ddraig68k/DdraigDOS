#ifndef DDRAIG_OS_GFXVGA_DRIVER_H
#define DDRAIG_OS_GFXVGA_DRIVER_H

#include "ddraig.h"
#include "drivers/display.h"

#define	REG_STATUS          0x00
#define REG_CONTROL         0x02
#define REG_INTERRUPT       0x04
#define	REG_COMMAND         0x06
#define REG_ADDR_DATA       0x08
#define	REG_PARAM_COLOR     0x0A
#define	REG_PARAM_X0        0x0C
#define	REG_PARAM_Y0        0x0E
#define	REG_PARAM_X1        0x10
#define	REG_PARAM_Y1        0x12

#define	REG_PARAM_DATA0     0x0A
#define	REG_PARAM_DATA1     0x0C
#define	REG_PARAM_DATA2     0x0E
#define	REG_PARAM_DATA3     0x10
#define	REG_PARAM_DATA4     0x12
#define	REG_PARAM_DATA5     0x14
#define	REG_PARAM_DATA6     0x16
#define	REG_PARAM_DATA7     0x18

#define CMD_CLEAR_SCREEN    0x0001
#define CMD_FILL_RECT       0x0002
#define CMD_DRAW_PIXEL      0x0003
#define CMD_DRAW_LINE       0x0004
#define CMD_SET_CHARACTER   0x0010
#define CMD_SET_TEXTCOLOR   0x0011
#define CMD_SET_TEXTAREA    0x0012
#define CMD_MEMORY_ACCESS   0x0020

void gfxfpga_init_driver(display_t *display);

#endif
