#ifndef V9990DDRAIG_OS_V9958LIB_H
#define V9990DDRAIG_OS_V9958LIB_H

#include "ddraig.h"
#include "v9990_definitions.h"

// global variables
extern uint32_t g_v9990_base_address;
extern uint8_t g_vd9990_scroll_mode;

#define V9990_VRAM_DATA         0x00
#define V9990_PALETTE   	    0x02
#define V9990_CMD_DATA	        0x04
#define V9990_REG_DATA          0x06
#define V9990_REG_SELECT        0x08
#define V9990_STATUS	        0x0a
#define V9990_INT_FLAG          0x0c
#define V9990_SYS_CTRL	        0x0e

#define V9990_READ(x)	        (*((volatile uint8_t *) (g_v9990_base_address + x) ))
#define V9990_WRITE(x, y)	    (*((volatile uint8_t *) (g_v9990_base_address + x) ) = (y) )
#define V9990_WRITE_REG(x, y)   ( V9990_WRITE(V9990_REG_SELECT, x) ); \
                                ( V9990_WRITE(V9990_REG_DATA, y) )


// Mode select defines for SetScreenMode
#define V9990_MODE_P1           0x00    // Pattern mode 0 256 212
#define V9990_MODE_P2           0x01    // Pattern mode 1 512 212
#define V9990_MODE_B1           0x02    // Bitmap mode 1 256 212
#define V9990_MODE_B2           0x03    // Bitmap mode 2 384 240
#define V9990_MODE_B3           0x04    // Bitmap mode 3 512 212
#define V9990_MODE_B4           0x05    // Bitmap mode 4 768 240
#define V9990_MODE_B5           0x06    // Bitmap mode 5 640 400 (VGA)
#define V9990_MODE_B6           0x07    // Bitmap mode 6 640 480 (VGA)
#define V9990_MODE_B7           0x08    // Bitmap mode 7 1024 212 (Undocumented v9990 mode)

#define V9990_PAL               0x01    // PAL Mode
#define V9990_NTSC              0x00    // NTSC mode
#define V9990_INTERLACE         0x01    // Interlace mode
#define V9990_NOINTERLACE       0x00    // Non interlace ode

#define V9990_BPP2              V9K_SCR0_2BIT
#define V9990_BPP4              V9K_SCR0_4BIT
#define V9990_BPP8              V9K_SCR0_4BIT
#define V9990_BPP16             V9K_SCR0_16BIT

#define V9990_WIDTH256          V9K_SCR0_XIM256
#define V9990_WIDTH512          V9K_SCR0_XIM512
#define V9990_WIDTH1024         V9K_SCR0_XIM1024
#define V9990_WIDTH2048         V9K_SCR0_XIM2048



// v9990 structures
typedef struct V9990_BOX
{
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t height;
} V9990_BOX;

typedef struct V9990_COPY_XY_XY
{
    uint16_t sourceX;
    uint16_t sourceY;
    uint16_t destX;
    uint16_t destY;
    uint16_t width;
    uint16_t height;
} V9990_COPY_XY_XY;

typedef struct V9990_COLOR
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} V9990_COLOR;


void v9990_Init(uint32_t addr);

void v9990_Reset(void);
void v9990_DisplayEnable(void);
void v9990_DisplayDisable(void);

void v9990_SetScreenMode(uint8_t mode, uint8_t bpp, uint8_t width, uint8_t interlace, uint8_t vidmode, uint8_t pal_ctrl);

void v9990_WritePalette(V9990_COLOR *data, uint8_t palnum, uint16_t count);
void v9990_ReadPalette(V9990_COLOR *data, uint8_t palum, uint16_t count);

void v9990_InitSpritePatten(uint8_t base);
void v9990_SpritesEnable();
void v9990_SpritesDisable();

void v9990_SetCmdWriteMask(uint16_t mask);
void v9990_SetBackDropColor(uint8_t color);
void v9990_SetCmdColor(uint16_t color);
void v9990_SetCmdBackColor(uint16_t color);

void v9990_CmdWait();
void v9990_WaitVsync();
void v9990_WaitVsyncClear();


void v9990_DrawBox(V9990_BOX *box, uint16_t color);
void v9990_DrawFilledBox(V9990_BOX *box, uint16_t color);
void v9990_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void v9990_SetPoint(uint16_t x, uint16_t y);

void v9990_SetAdjust(uint8_t x, uint8_t y);

void v9990_SetVramWrite(uint32_t addr);
void v9990_SetVramRead(uint32_t addr);

void v9990_SetupCopyRamToXY(V9990_BOX *box);
void v9990_CopyRamToXY(uint8_t *buffer, uint32_t count);

void v9990_SetupCopyXYToRam(V9990_BOX *box);
void v9990_CopyXYToRam(uint8_t *buffer, uint32_t count);

void v9990_CopyRamToVram(uint8_t *buffer, uint32_t count);

void v9990_CopyXYToXY(V9990_COPY_XY_XY *data);
void v9990_CopyVRamToVRam(uint32_t source, uint32_t dest, uint32_t size);
void v9990_CopyVRamCharToXY(V9990_BOX *box, uint32_t vram);

void v9990_EnableLayerA();
void v9990_EnableLayerB();
void v9990_DisableLayerA();
void v9990_DisableLayerB();

void v9990_SetPattern(uint16_t pattern, uint16_t location, uint8_t layer);
void v9990_PrintTilesA(uint16_t *data, uint16_t location, uint16_t count);
void v9990_PrintTilesB(uint16_t *data, uint16_t location, uint16_t count);

void v9990_SetIntLine(uint16_t line);

void v9990_Locate(uint16_t x, uint16_t y);

void v9990_SetScrollMode(uint8_t mode);
void v9990_SetScrollX(uint16_t count);
void v9990_SetScrollY(uint16_t count);
void v9990_SetScrollXB(uint16_t count);
void v9990_SetScrollYB(uint16_t count);

#endif
