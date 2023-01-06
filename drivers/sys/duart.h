#ifndef DDRAIG_OS_DUART_DRIVER_H
#define DDRAIG_OS_DUART_DRIVER_H

#include "ddraig.h"

#define	DUART_MR1A      *((volatile uint8_t *) (DUART_BASE + 0x00))     // Mode Register A
#define DUART_MR2A      *((volatile uint8_t *) (DUART_BASE + 0x00))     // Mode Register A
#define DUART_SRA       *((volatile uint8_t *) (DUART_BASE + 0x02))     // Status Register A
#define DUART_CSRA      *((volatile uint8_t *) (DUART_BASE + 0x02))     // Clock-Select Register A
#define DUART_CRA       *((volatile uint8_t *) (DUART_BASE + 0x04))     // Command Register A
#define DUART_RBA       *((volatile uint8_t *) (DUART_BASE + 0x06))     // Receive Buffer A
#define DUART_TBA       *((volatile uint8_t *) (DUART_BASE + 0x06))     // Transmit Buffer A
#define DUART_IPCR      *((volatile uint8_t *) (DUART_BASE + 0x08))     // Input Port Change Register
#define DUART_ACR       *((volatile uint8_t *) (DUART_BASE + 0x08))     // Auxiliary Control Register
#define DUART_ISR       *((volatile uint8_t *) (DUART_BASE + 0x0A))     // Interrupt Status Register
#define DUART_IMR       *((volatile uint8_t *) (DUART_BASE + 0x0A))     // Interrupt Mask Register
#define DUART_CUR       *((volatile uint8_t *) (DUART_BASE + 0x0C))     // Counter Mode: current MSB
#define DUART_CTUR      *((volatile uint8_t *) (DUART_BASE + 0x0C))     // Counter/Timer upper reg
#define DUART_CLR       *((volatile uint8_t *) (DUART_BASE + 0x0E))     // Counter Mode: current LSB
#define DUART_CTLR      *((volatile uint8_t *) (DUART_BASE + 0x0E))     // Counter/Timer lower reg
#define DUART_MR1B      *((volatile uint8_t *) (DUART_BASE + 0x10))     // Mode Register B
#define DUART_MR2B      *((volatile uint8_t *) (DUART_BASE + 0x10))     // Mode Register B
#define DUART_SRB       *((volatile uint8_t *) (DUART_BASE + 0x12))     // Status Register B
#define DUART_CSRB      *((volatile uint8_t *) (DUART_BASE + 0x12))     // Clock-Select Register B
#define DUART_CRB       *((volatile uint8_t *) (DUART_BASE + 0x14))     // Command Register B
#define DUART_RBB       *((volatile uint8_t *) (DUART_BASE + 0x16))     // Receive Buffer B
#define DUART_TBB       *((volatile uint8_t *) (DUART_BASE + 0x16))     // Transmit Buffer A
#define DUART_IVR       *((volatile uint8_t *) (DUART_BASE + 0x18))     // Interrupt Vector Register
#define DUART_IP        *((volatile uint8_t *) (DUART_BASE + 0x1A))     // Input Port
#define DUART_OPCR      *((volatile uint8_t *) (DUART_BASE + 0x1A))     // Output Port Configuration Reg.
#define DUART_STRTCC    *((volatile uint8_t *) (DUART_BASE + 0x1C))     // Start-Counter command
#define DUART_OPRSET    *((volatile uint8_t *) (DUART_BASE + 0x1C))     // Output Port Reg,SET bits
#define DUART_STOPCC    *((volatile uint8_t *) (DUART_BASE + 0x1E))     // Stop-Counter command
#define DUART_OPRRST    *((volatile uint8_t *) (DUART_BASE + 0x1E))     // Output Port Reg,ReSeT bits

// Status Register Bits (SRA/SRB)
#define SR_RECEIVED_BREAK       0x80
#define SR_FRAMING_ERROR	    0x40
#define SR_PARITY_ERROR		    0x20
#define SR_OVERRUN_ERROR	    0x10
#define SR_TX_EMPTY			    0x08
#define SR_TX_READY			    0x04
#define SR_RX_FULL			    0x02
#define SR_RX_READY			    0x01


// Interrupt Status/Mask Bits (ISR/IVR)
#define ISR_INPUT_CHANGE		0x80
#define ISR_CH_B_BREAK_CHANGE	0x40
#define ISR_CH_B_RX_READY_FULL	0x20
#define ISR_CH_B_TX_READY		0x10
#define ISR_TIMER_CHANGE		0x08
#define ISR_CH_A_BREAK_CHANGE	0x04
#define ISR_CH_A_RX_READY_FULL	0x02
#define ISR_CH_A_TX_READY		0x01

void mc68681_init();

uint8_t serial_has_char(void);
uint8_t serial_get(void);
void _putchar_serial(char c);

//int8_t serialb_has_char(void);
//int8_t serialb_get(void);
//void serialb_put(char c);

uint8_t read_serial_inputs(void);

#endif
