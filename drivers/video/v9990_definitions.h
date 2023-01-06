#ifndef V9990DDRAIG_OS_V9990_DEFINITIONS_H
#define V9990DDRAIG_OS_V9990_DEFINITIONS_H

#include <stdint.h>

#define V9K_SYS_CTRL_SRS	        2   // Power on reset state
#define V9K_SYS_CTRL_MCKIN	        1	// Select MCKIN terminal
#define V9K_SYS_CTRL_XTAL	        0	// Select XTAL

// V9990 Register defines
#define V9K_WRITE_ADDR		        0	// W
#define V9K_READ_ADDR		        3	// W
#define V9K_SCREEN_MODE0	        6	// R/W
#define V9K_SCREEN_MODE1	        7	// R/W
#define V9K_CTRL		            8	// R/W
#define V9K_INT_ENABLE              9     // R/W
#define V9K_INT_V_LINE_LO	        10	// R/W
#define V9K_INT_V_LINE_HI	        11	// R/W
#define V9K_INT_H_LINE		        12	// R/W
#define V9K_PALETTE_CTRL	        13	// W
#define V9K_PALETTE_PTR		        14	// W
#define V9K_BACK_DROP_COLOR 	    15  // R/W
#define V9K_DISPLAY_ADJUST	        16	// R/W
#define V9K_SCROLL_LOW_Y	        17  // R/W
#define V9K_SCROLL_HIGH_Y	        18  // R/W
#define V9K_SCROLL_LOW_X	        19  // R/W
#define V9K_SCROLL_HIGH_X	        20  // R/W
#define V9K_SCROLL_LOW_Y_B	        21  // R/W
#define V9K_SCROLL_HIGH_Y_B	        22  // R/W
#define V9K_SCROLL_LOW_X_B	        23  // R/W
#define V9K_SCROLL_HIGH_X_B	        24  // R/W
#define V9K_PAT_GEN_TABLE   	    25  // R/W
#define V9K_LCD_CTRL        	    26  // R/W
#define V9K_PRIORITY_CTRL  	        27  // R/W
#define V9K_SPR_PAL_CTRL	        28	// W
#define V9K_SC_X		            32	// W
#define V9K_SC_Y		            34	// W
#define V9K_DS_X		            36	// W
#define V9K_DS_Y		            38	// W
#define V9K_NX			            40	// W
#define V9K_NY			            42	// W
#define V9K_ARG			            44	// W
#define V9K_LOP			            45	// W Logical Operation WC (Write Color) is obtained through logical operation with SC (Source Color) and DC (Destination Color)
#define V9K_WRITE_MASK		        46	// W
#define V9K_FC			            48	// W
#define V9K_BC			            50	// W
#define V9K_OPCODE		            52	// W
#define V9K_BDC_LOW_X		        53	// R
#define V9K_BDC_HIGH_X		        54	// R

// Register Select options
#define V9K_DIS_INC_READ	        0x40
#define V9K_DIS_INC_WRITE	        0x80

// Bit defines SCREEN_MODE0 (Register 6)
#define V9K_SCR0_STANDBY	        0xC0	// Standby mode
#define V9K_SCR0_BITMAP		    0x80	// Select Bit map mode
#define V9K_SCR0_P2		        0x40	// Select P1 mode
#define V9K_SCR0_P1		        0x00	// Select P1 mode
#define V9K_SCR0_DTCLK		    0x20	// Master Dot clock not divided
#define V9K_SCR0_DTCLK2		    0x10	// Master Dot clock divided by 2
#define V9K_SCR0_DTCLK4		    0x00	// Master Dot clock divided by 4
#define V9K_SCR0_XIM2048	        0x0C	// Image size = 2048
#define V9K_SCR0_XIM1024	        0x08	// Image size = 1024
#define V9K_SCR0_XIM512		    0x04	// Image size = 512
#define V9K_SCR0_XIM256		    0x00	// Image size = 256
#define V9K_SCR0_16BIT		    0x03	// 16 bits/dot
#define V9K_SCR0_8BIT		        0x02	// 8 bits/dot
#define V9K_SCR0_4BIT		        0x01	// 4 bits/dot
#define V9K_SCR0_2BIT		        0x00	// 2 bits/dot

// Bit defines SCREEN_MODE1 (Register 7)
#define V9K_SCR1_C25M		        0x40	// Select 640*480 mode
#define V9K_SCR1_SM1		        0x20	// Selection of 263 lines during non interlace , else 262
#define V9K_SCR1_SM		        0x10	// Selection of horizontal frequency 1H=fsc/227.5
#define V9K_SCR1_PAL		        0x08	// Select PAL
#define V9K_SCR1_NTSC		        0x00	// Select NTSC
#define V9K_SCR1_EO		        0x04	// Select of vertical resoltion of twice the non-interlace resolution
#define V9K_SCR1_IL		        0x02	// Select Interlace
#define V9K_SCR1_HSCN		        0x01	// Select High scan mode

// Bit defines CTRL (Register 8)
#define V9K_CTRL_DISP		        0x80	// Display VRAM
#define V9K_CTRL_DIS_SPD	        0x40	// Disable display sprite (cursor)
#define V9K_CTRL_YSE		        0x20	// /YS Enable
#define V9K_CTRL_VWTE		        0x10	// VRAM Serial data bus control during digitization
#define V9K_CTRL_VWM		        0x08	// VRAM write control during digitization
#define V9K_CTRL_DMAE		        0x04	// Enable DMAQ output
#define V9K_CTRL_VRAM512	        0x02	// VRAM=512KB
#define V9K_CTRL_VRAM256	        0x01	// VRAM=256KB
#define V9K_CTRL_VRAM128	        0x00	// VRAM=128KB

// Bit defines INT_ENABLE (Register 9)
#define V9K_INT_IECE	            0x04    // Command end interrupt enable control
#define V9K_INT_IEH	            0x02    // Display position interrupt enable
#define V9K_INT_IEV	            0x01    // Int. enable during vertical retrace line interval

// Bit Defines PALETTE_CTRL (Register 13)
#define V9K_PAL_CTRL_YUV	        0xC0    // YUV mode
#define V9K_PAL_CTRL_YJK	        0x80	// YJK mode
#define V9K_PAL_CTRL_256	        0x44	// 256 color mode
#define V9K_PAL_CTRL_YAE	        0x20	// Enable YUV/YJK RGB mixing mode
#define V9K_PAL_CTRL_PAL          0x00    // Pallete mode

// Bit defines LOP (Register 45)
//  b4 TP	: When this bit is "1", the data with SC as all "0"
//            (by pixel for the X/Y-coordinates and by byte for the linear address) is not transferred.
//  b0 L00	: WC value for the bit with SC=0 and DC=0
//  b1 L01	: WC value for the bit with SC=0 and DC=1
//  b2 L10	: WC value for the bit with SC=1 and DC=0
//  b3 L11	: WC value for the bit with SC=1 and DC=1
#define V9K_LOP_TP		        0x10
#define V9K_LOP_WCSC		        0x0C
#define V9K_LOP_WCNOTSC		    0x03
#define V9K_LOP_WCANDSC		    0x08
#define V9K_LOP_WCORSC		    0x0E
#define V9K_LOP_WCEORSC		    0x06

// Bit defines ARG
#define V9K_ARG_MAJ		        0x01
#define V9K_ARG_NEG		        0x02
#define V9K_ARG_DIX		        0x04
#define V9K_ARG_DIY		        0x08

// Blitter Commands OPCODE (Register 52)
#define V9K_OPCODE_STOP		    0x00    // Command being excuted is stopped
#define V9K_OPCODE_LMMC		    0x10    // Data is transferred from CPU to VRAM rectangle area
#define V9K_OPCODE_LMMV		    0x20    // VRAM rectangle area is painted out
#define V9K_OPCODE_LMCM		    0x30    // VRAM rectangle area is transferred to CPU
#define V9K_OPCODE_LMMM		    0x40    // Rectangle area os transferred from VRAM to VRAM
#define V9K_OPCODE_CMMC		    0x50    // CPU character data is color-developed and transferred to VRAM rectangle area
#define V9K_OPCODE_CMMK		    0x60    // Kanji ROM data is is color-developed and transferred to VRAM rectangle area
#define V9K_OPCODE_CMMM		    0x70    // VRAM character data is color-developed and transferred to VRAM rectangle area
#define V9K_OPCODE_BMXL		    0x80    // Data on VRAM linear address is transferred to VRAM rectangle area
#define V9K_OPCODE_BMLX		    0x90    // VRAM rectangle area is transferred to VRAM linear address
#define V9K_OPCODE_BMLL		    0xA0    // Data on VRAM linear address is transferred to VRAM linear address
#define V9K_OPCODE_LINE		    0xB0    // Straight line is drawer on X-Y co-ordinates
#define V9K_OPCODE_SRCH		    0xC0    // Border color co-ordinates on X-Y are detected
#define V9K_OPCODE_POINT	        0xD0    // Color code on specified point on X-Y is read out
#define V9K_OPCODE_PSET		    0xE0    // Drawing is executed at drawing point on X-Y co-ordinates
#define V9K_OPCODE_ADVN		    0xF0    // Drawing point on X-Y co-ordinates is shifted

// Bit defines STATUS
#define V9K_STATUS_TR             0x80    // Command data transfer ready
#define V9K_STATUS_VR             0x40    // Vertical non display period
#define V9K_STATUS_HR             0x20    // Horizontal non display period
#define V9K_STATUS_BD             0x10    // Border color detect
#define V9K_STATUS_MSC            0x04    // Content of p#7 MCS
#define V9K_STATUS_EO             0x02    // In the second field period of interlace
#define V9K_STATUS_CE             0x01    // Command being executed

#define V9K_RED                   0x020
#define V9K_GREEN                 0x400
#define V9K_BLUE                  0x001

#endif
