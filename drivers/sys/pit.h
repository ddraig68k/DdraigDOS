#ifndef DDRAIGOS_PIT_DRIVER_H
#define DDRAIGOS_PIT_DRIVER_H

#include "ddraig.h"
// PIT Definitions file

#define PIT_READ(x)      (*((volatile uint8_t *) PIT_BASE + (x)))
#define PIT_WRITE(x, y)  (*((uint8_t *) PIT_BASE + (x)) = (y))

// PIT Registers
#define PIT_PGCR            0x00     // Port General Control Register
#define PIT_PSRR 	        0x02     // Port Service Request Register
#define PIT_PADDR	        0x04     // Port A Data Direction Register
#define PIT_PBDDR	        0x06     // Port B Data Direction Register
#define PIT_PCDDR	        0x08     // Port C Data Direction Register
#define PIT_PIVR	        0x0A     // Port Interrupt Vector Register
#define PIT_PACR	        0x0C     // Port A Control Register
#define PIT_PBCR	        0x0E     // Port B Control Register
#define PIT_PADR	        0x10     // Port A Data Register
#define PIT_PBDR	        0x12     // Port B Data Register
#define PIT_PAAR	        0x14     // Port A Alternative Register
#define PIT_PBAR	        0x16     // Port B Alternative Register
#define PIT_PCDR	        0x18     // Port C Data Register
#define PIT_PSR	            0x1A     // Port Status Register
#define PIT_TCR 	        0x20     // Timer Control Register
#define PIT_TIVR	        0x22     // Timer Interrupt Vector Register
#define PIT_CPRH	        0x26     // Counter Preload Register (High)
#define PIT_CPRM	        0x28     // Counter Preload Register (Mid)
#define PIT_CPRL	        0x2A     // Counter Preload Register (Low)
#define PIT_CNTRH	        0x2E     // Count Register (High)
#define PIT_CNTRM	        0x30     // Count Register (Mid)
#define PIT_CNTRL	        0x32     // Count Register (Low)
#define PIT_TSR             0x34     // Timer Status Register

// General Control Register
#define PIT_GCR_MODE0       0x00    // Mode 0 - Unidirectional 8-bit mode
#define PIT_GCR_MODE1       0x40    // Mode 1 - Unidirectional 16-bit mode
#define PIT_GCR_MODE2       0x80    // Mode 2 - Bidirectional 8-bit mode
#define PIT_GCR_MODE3       0xC0    // Mode 3 - Bidirectional 18-bit mode
#define PIT_GCR_H34_EN      0x20    // H34 Enable
#define PIT_GCR_H12_EN      0x10    // H12 Enable
#define PIT_GCR_SENSE_H4    0x08    // Handshake sense H4
#define PIT_GCR_SENSE_H3    0x04    // Handshake sense H3
#define PIT_GCR_SENSE_H2    0x02    // Handshake sense H2
#define PIT_GCR_SENSE_H1    0x01    // Handshake sense H1

// Port Service Request Register
#define PIT_PSRR_NODMA      0x00    // DMA is not used
#define PIT_PSRR_DMA_H1     0x40    // DMA transfers controlled by H1
#define PIT_PSRR_DMA_H3     0x60    // DMA transfers controlled by H3
#define PIT_PSRR_NOINT      0x00    // No interrupt support
#define PIT_PSRR_AVINT      0x08    // PC5/PIRQ, PC6/PIACK, supports autovectored interrupts
#define PIT_PSRR_INT        0x10    // PC5/PIRQ, PC6/PIACK
#define PIT_PSRR_VECINT     0x18    // PC5/PIRQ, PC6/PIACK, supports vectored interrupts
                                    // Priority High -> Low
#define PIT_PSRR_PRI07      0x07    // H4S, H3S, H2S, H1S
#define PIT_PSRR_PRI06      0x06    // H4S, H3S, H1S, H2S
#define PIT_PSRR_PRI05      0x05    // H3S, H4S, H2S, H1S
#define PIT_PSRR_PRI04      0x04    // H3S, H4S, H1S, H2S
#define PIT_PSRR_PRI03      0x03    // H2S, H1S, H4S, H3S
#define PIT_PSRR_PRI02      0x02    // H1S, H2S, H4S, H3S
#define PIT_PSRR_PRI01      0x01    // H2S, H1S, H3S, H4S
#define PIT_PSRR_PRI00      0x00    // H1S, H2S, H3S, H4S

// Port status register
#define PIT_PSR_LEVEL_H4    0x80
#define PIT_PSR_LEVEL_H3    0x40
#define PIT_PSR_LEVEL_H2    0x20
#define PIT_PSR_LEVEL_H1    0x10
#define PIT_PSR_H4S         0x08
#define PIT_PSR_H3S         0x04
#define PIT_PSR_H2S         0x02
#define PIT_PSR_H1S         0x01

// Timer Control Register
#define PIT_TCR_ZDT         0x08    // Zero Detect Control
#define PIT_TCR_CC3         0x03    // PC2/TIN timer input, no prescaler
#define PIT_TCR_CC2         0x02    // PC2/TIN timer input, prescaler used
#define PIT_TCR_CC1         0x01    // PC2/TIN timer input, CLK and prescaler used
#define PIT_TCR_CC0         0x00    // PC2/TIN port C function, no prescaler


#endif

