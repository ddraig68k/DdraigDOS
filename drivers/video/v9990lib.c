#include "v9990lib.h"
#include <stdio.h>

uint32_t g_v9990_base_address = 0;
uint8_t g_vd9990_scroll_mode = 0;

typedef struct {
    uint8_t scr0;
    uint8_t scr1;
    uint8_t ctrl;
} v9900_mode_t;

static const v9900_mode_t g_v9990_mode_table[] =
{
    // Pattern mode 1      (P1)
    { V9K_SCR0_P1 | V9K_SCR0_DTCLK4, 0, V9K_SYS_CTRL_XTAL },
    // Pattern mode 2      (P2)
    { V9K_SCR0_P2 | V9K_SCR0_DTCLK4, 0, V9K_SYS_CTRL_XTAL },
    // Bitmap 256 * 212    (B1)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK4, 0, V9K_SYS_CTRL_XTAL },
    // Bitmap 384 * 240    (B2)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK2, 0, V9K_SYS_CTRL_MCKIN },
    // Bitmap 512 * 212    (B3)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK2, 0, V9K_SYS_CTRL_XTAL },
    // Bitmap 768 * 212    (B4)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK, 0, V9K_SYS_CTRL_MCKIN },
    // Bitmap 640 * 400    (B5)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK, V9K_SCR1_HSCN, V9K_SYS_CTRL_XTAL },
    // Bitmap 640 * 480    (B6)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK, V9K_SCR1_HSCN | V9K_SCR1_C25M, V9K_SYS_CTRL_XTAL },
    // Bitmap 1024 * 212   (B7) (undocumented mode)
    { V9K_SCR0_BITMAP | V9K_SCR0_DTCLK, 0, V9K_SYS_CTRL_XTAL }
};

static inline uint8_t v9990_ReadReg(uint8_t reg)
{
    V9990_WRITE(V9990_REG_SELECT, reg);
    return V9990_READ(V9990_REG_DATA);
}

// Set the base address and reset display
void v9990_Init(uint32_t addr)
{
    g_v9990_base_address = addr;
    v9990_Reset();
}

// Reset the V9990 display to default
void v9990_Reset(void)
{
    V9990_WRITE_REG(V9990_SYS_CTRL, V9K_SYS_CTRL_SRS);    // Set reset state
    V9990_WRITE_REG(V9990_SYS_CTRL, 0);    // Clear reset state

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_STOP);
    V9990_WRITE_REG(V9K_CTRL, V9K_CTRL_DIS_SPD | V9K_CTRL_VRAM512);

    V9990_WRITE_REG(V9K_ARG, 0);
    V9990_WRITE_REG(V9K_LOP, V9K_LOP_WCSC);

    // Clear current palette
    V9990_WRITE_REG(V9K_PALETTE_PTR, 0);
    for (int i = 0; i < 192; i++)   // 64 * 3
        V9990_WRITE(V9990_PALETTE, 0);
}

// Enable the V9990 display
void v9990_DisplayEnable(void)
{
    uint8_t ctrl = v9990_ReadReg(V9K_CTRL | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, ctrl |  V9K_CTRL_DISP);
}

// Disable the V9990 display
void v9990_DisplayDisable(void)
{
    uint8_t ctrl = v9990_ReadReg(V9K_CTRL | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, ctrl & ~V9K_CTRL_DISP);
}

// Set the screen mode for the V9990
void v9990_SetScreenMode(uint8_t mode, uint8_t bpp, uint8_t width, uint8_t interlace, uint8_t vidmode, uint8_t pal_ctrl)
{
    if (g_v9990_base_address == 0)
    {
        printf("base address must be set\n\r");
        return;
    }

    // Get the default settings for the mode
    const v9900_mode_t *regs = &g_v9990_mode_table[mode];

    uint8_t scr0 = regs->scr0 | bpp | width;    // Set the BPP and Width
    uint8_t scr1 = regs->scr1;

    if (vidmode == V9990_PAL)                   // PAL or NTSC?
        scr1 |= V9K_SCR1_PAL;

    if (interlace)                              // Interlaced or not?
        scr1 |= V9K_SCR1_EO | V9K_SCR1_IL;

    V9990_WRITE_REG(V9K_SCREEN_MODE0, scr0);
    V9990_WRITE(V9990_REG_DATA, scr1);
    V9990_WRITE(V9990_SYS_CTRL, regs->ctrl);

    V9990_WRITE_REG(V9K_PALETTE_CTRL, pal_ctrl);
}

void v9990_WritePalette(V9990_COLOR *data, uint8_t palnum, uint16_t count)
{
    V9990_WRITE_REG(V9K_PALETTE_PTR, palnum << 2);
    uint8_t *data_ptr = (uint8_t *)data;
    for (uint16_t col = 0; col < (count * 3); col++)
        V9990_WRITE(V9990_PALETTE, *data_ptr++);

}

void v9990_ReadPalette(V9990_COLOR *data, uint8_t palnum, uint16_t count)
{
    V9990_WRITE_REG(V9K_PALETTE_PTR, palnum);
    uint8_t *data_ptr = (uint8_t *)data;
    for (uint16_t col = 0; col < (count * 3); col++)
        *data_ptr++ = V9990_READ(V9990_PALETTE);
}

// Sets the address for the sprite patterns
// Sets the bits A17,A16,A15,0 of VRAM address of the sprite pattern table start
void v9990_InitSpritePatten(uint8_t base)
{
    V9990_WRITE_REG(V9K_PAT_GEN_TABLE, base);
}

void v9990_SpritesEnable()
{
    uint8_t ctrl = v9990_ReadReg(V9K_CTRL | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, ctrl & ~V9K_CTRL_DIS_SPD);

}

void v9990_SpritesDisable()
{
    uint8_t ctrl = v9990_ReadReg(V9K_CTRL | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, ctrl | V9K_CTRL_DIS_SPD);

}

void v9990_SetCmdWriteMask(uint16_t mask)
{
    V9990_WRITE_REG(V9K_WRITE_MASK, mask & 0xFF);
    V9990_WRITE(V9990_REG_DATA, mask >> 8);
}

void v9990_SetBackDropColor(uint8_t color)
{
    V9990_WRITE_REG(V9K_BACK_DROP_COLOR, color & 0x3F);
}

void v9990_SetCmdColor(uint16_t color)
{
    V9990_WRITE_REG(V9K_FC, (uint8_t)color);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(color >> 8));
}

void v9990_SetCmdBackColor(uint16_t color)
{
    V9990_WRITE_REG(V9K_BC, (uint8_t)color);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(color >> 8));
}

void v9990_CmdWait()
{
    while ((V9990_READ(V9990_STATUS) & V9K_STATUS_CE) != 0);
}

void v9990_WaitVsync()
{
    while ((V9990_READ(V9990_STATUS) & V9K_STATUS_VR) == 0);
}

void v9990_WaitVsyncClear()
{
    while ((V9990_READ(V9990_STATUS) & V9K_STATUS_VR) != 0);
}

void v9990_DrawBox(V9990_BOX *box, uint16_t color)
{
    v9990_DrawLine((int16_t)(box->left + box->width), (int16_t)box->top, 0, (int16_t)box->height, color);    // RIGHT - top right -> bottom right
    v9990_DrawLine((int16_t)box->left, (int16_t)box->top, (int16_t)box->width, 0, color);                  // TOP - top left -> top right
    v9990_DrawLine((int16_t)box->left, (int16_t)box->top, 0, (int16_t)box->height, color);                 // LEFT - top left -> bottom left
    v9990_DrawLine((int16_t)box->left, (int16_t)(box->top + box->height), (int16_t)box->width, 0, color);    // BOTTOM - bottom left -> bottom right
}

void v9990_DrawFilledBox(V9990_BOX *box, uint16_t color)
{
    V9990_WRITE_REG(V9K_DS_X, (uint8_t)box->left);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->left >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height >> 8));

    V9990_WRITE_REG(V9K_FC, (uint8_t)color);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(color >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_LMMV);
}

void v9990_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
    int32_t mj, mi;
    uint8_t maj = 0;

    if (x2 < 0)
    {
        maj += 4;
        x2 = -1 * x2;
    }

    if (y2 < 0)
    {
        maj += 8;
        y2 = -1 * y2;
    }

    if (x2>= y2)
    {
        mj = x2;
        mi = y2;
        maj += 0;
    }
    else
    {
        mj = y2;
        mi = x2;
        maj += 1;
    }

    v9990_CmdWait();
    V9990_WRITE_REG(V9K_DS_X, (uint8_t)x1);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(x1 >> 8) & 0x07);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)y1);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(y1 >> 8) & 0x0F);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)mj);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(mj >> 8) & 0x0F);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)mi);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(mi >> 8) & 0x0F);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)maj);

    V9990_WRITE_REG(V9K_FC, (uint8_t)color);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(color >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_LINE);
}

void v9990_SetPoint(uint16_t x, uint16_t y)
{
    V9990_WRITE_REG(V9K_DS_X, (uint8_t)x);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(x >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)y);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(y >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_PSET);
}

void v9990_SetAdjust(uint8_t x, uint8_t y)
{
    V9990_WRITE_REG(V9K_DISPLAY_ADJUST, ((x & 0x0f) + ((y << 4) & 0xf0)));
}

void v9990_SetVramWrite(uint32_t addr)
{
    V9990_WRITE_REG(V9K_WRITE_ADDR, (uint8_t)addr);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(addr >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(addr >> 16));
}

void v9990_SetVramRead(uint32_t addr)
{
    V9990_WRITE_REG(V9K_READ_ADDR, (uint8_t)addr);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(addr >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(addr >> 16));
}

void v9990_SetupCopyRamToXY(V9990_BOX *box)
{
    v9990_CmdWait();
    V9990_WRITE_REG(V9K_DS_X, (uint8_t)box->left);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->left >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_LMMC);
}

void v9990_CopyRamToXY(uint8_t *buffer, uint32_t count)
{
    while (count--)
        V9990_WRITE(V9990_CMD_DATA, *buffer++);
}

void v9990_SetupCopyXYToRam(V9990_BOX *box)
{
    v9990_CmdWait();
    V9990_WRITE_REG(V9K_DS_X, (uint8_t)box->left);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->left >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_LMCM);
}

void v9990_CopyXYToRam(uint8_t *buffer, uint32_t count)
{
    while (count--)
        *buffer++ = V9990_READ(V9990_CMD_DATA);
}

void v9990_CopyRamToVram(uint8_t *buffer, uint32_t count)
{
    while (count--)
        V9990_WRITE(V9990_VRAM_DATA, *buffer++);
}

void v9990_CopyXYToXY(V9990_COPY_XY_XY *xybox)
{
    v9990_CmdWait();
    V9990_WRITE_REG(V9K_SC_X,   (uint8_t)(xybox->sourceX));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->sourceX >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->sourceY));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->sourceY >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->destX));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->destX >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->destY));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->destY >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->width));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->width >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->height));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(xybox->height >> 8));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_LMMM);
}

void v9990_CopyVRamToVRam(uint32_t source, uint32_t dest, uint32_t size)
{
    v9990_CmdWait();
    V9990_WRITE_REG(V9K_SC_X, (uint8_t)source);
    V9990_WRITE_REG(V9K_SC_Y, (uint8_t)(source >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(source >> 16));

    V9990_WRITE(V9990_REG_DATA, (uint8_t)(dest));
    V9990_WRITE_REG(V9K_DS_Y, (uint8_t)(dest >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(dest >> 16));

    V9990_WRITE(V9990_REG_DATA, (uint8_t)(size));
    V9990_WRITE_REG(V9K_NY, (uint8_t)(size >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(size >> 16));

    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_BMLL);
}

void v9990_CopyVRamCharToXY(V9990_BOX *box, uint32_t vram)
{
    v9990_CmdWait();
    V9990_WRITE_REG(V9K_SC_X, (uint8_t)vram);
    V9990_WRITE_REG(V9K_SC_Y, (uint8_t)(vram >> 8));
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(vram >> 16) & 0x07);

    V9990_WRITE(V9990_REG_DATA, (uint8_t)box->left);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->left >> 8));

    V9990_WRITE(V9990_REG_DATA, (uint8_t)box->top);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->top >> 8));

    V9990_WRITE(V9990_REG_DATA, (uint8_t)box->width);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->width >> 8));

    V9990_WRITE(V9990_REG_DATA, (uint8_t)box->height);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(box->height>>8));
    V9990_WRITE_REG(V9K_OPCODE, V9K_OPCODE_CMMM);
}

void v9990_EnableLayerA()
{
    uint8_t reg = v9990_ReadReg(V9K_SCROLL_HIGH_Y | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, reg & 0x7F);
}

void v9990_EnableLayerB()
{
    uint8_t reg = v9990_ReadReg(V9K_SCROLL_HIGH_Y | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, reg & 0xBF);
}

void v9990_DisableLayerA()
{
    uint8_t reg = v9990_ReadReg(V9K_SCROLL_HIGH_Y | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, reg | 0x80);
}

void v9990_DisableLayerB()
{
    uint8_t reg = v9990_ReadReg(V9K_SCROLL_HIGH_Y | V9K_DIS_INC_READ);
    V9990_WRITE(V9990_REG_DATA, reg | 0x40);
}

void v9990_SetPattern(uint16_t pattern, uint16_t location, uint8_t layer)
{
    uint32_t address;
    if (layer)
        address = 0x7E000;          // Base address for layer 1
    else
        address = 0x7C000;          // Base address for layer 2

    address += (location << 1);     // Set location word address
    v9990_SetVramWrite(address);
    V9990_WRITE(V9990_VRAM_DATA, (uint8_t)pattern);
    V9990_WRITE(V9990_VRAM_DATA, (uint8_t)(pattern >> 8));
}

// Prints a number of tiles (with tile number indicated as a byte)
// Intended for printing small texts, scores...

void v9990_PrintTilesA(uint16_t *data, uint16_t location, uint16_t count)
{
    uint32_t address = 0x7C000 + (location << 1);     // Set location word address
    v9990_SetVramWrite(address);
    while (count--)
    {
        V9990_WRITE(V9990_VRAM_DATA, *data);
        V9990_WRITE(V9990_VRAM_DATA, *data >> 8);
        data++;
    }
}

void v9990_PrintTilesB(uint16_t *data, uint16_t location, uint16_t count)
{
    uint32_t address = 0x7E000 + (location << 1);     // Set location word address
    v9990_SetVramWrite(address);
    while (count--)
    {
        V9990_WRITE(V9990_VRAM_DATA, *data);
        V9990_WRITE(V9990_VRAM_DATA, *data >> 8);
        data++;
    }
}

void v9990_SetIntLine(uint16_t line)
{
    V9990_WRITE_REG(V9K_INT_V_LINE_LO, (uint8_t)line);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)(line >> 8));
}

// 0 = Roll by image space size
// 1 = Roll by 256 lines
// 2 = Roll by 512 lines
void v9990_SetScrollMode(uint8_t mode)
{
    g_vd9990_scroll_mode = mode << 6;
}

void v9990_SetScrollX(uint16_t count)
{
    V9990_WRITE_REG(V9K_SCROLL_LOW_X, (uint8_t)count & 0x07);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)( count >> 3));
}

void v9990_SetScrollY(uint16_t count)
{
    V9990_WRITE_REG(V9K_SCROLL_LOW_Y, (uint8_t)count);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)((count >> 8) & 0x1F) | g_vd9990_scroll_mode);
}

void v9990_SetScrollXB(uint16_t count)
{
    V9990_WRITE_REG(V9K_SCROLL_LOW_X_B, (uint8_t)count & 0x07);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)( count >> 3));
}

void v9990_SetScrollYB(uint16_t count)
{
    V9990_WRITE_REG(V9K_SCROLL_LOW_Y_B, (uint8_t)count);
    V9990_WRITE(V9990_REG_DATA, (uint8_t)((count >> 8) & 0x1F) | g_vd9990_scroll_mode);
}
