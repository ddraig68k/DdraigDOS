#ifndef DDRAIGOS_NE2K_CONST_H
#define DDRAIGOS_NE2K_CONST_H 

//
// Page 0 register offsets
//

#define NE_P0_CR        (0x00<<1)      // Command Register
#define NE_P0_CLDA0     (0x01<<1)      // Current Local DMA Addr low (read)
#define NE_P0_PSTART    (0x01<<1)      // Page Start register (write)
#define NE_P0_CLDA1     (0x02<<1)      // Current Local DMA Addr high (read)
#define NE_P0_PSTOP     (0x02<<1)      // Page Stop register (write)
#define NE_P0_BNRY      (0x03<<1)      // Boundary Pointer
#define NE_P0_TSR       (0x04<<1)      // Transmit Status Register (read)
#define NE_P0_TPSR      (0x04<<1)      // Transmit Page Start (write)
#define NE_P0_NCR       (0x05<<1)      // Number of Collisions Reg (read)
#define NE_P0_TBCR0     (0x05<<1)      // Transmit Byte count, low (write)
#define NE_P0_FIFO      (0x06<<1)      // FIFO register (read)
#define NE_P0_TBCR1     (0x06<<1)      // Transmit Byte count, high (write)
#define NE_P0_ISR       (0x07<<1)      // Interrupt Status Register
#define NE_P0_CRDA0     (0x08<<1)      // Current Remote DMA Addr low (read)
#define NE_P0_RSAR0     (0x08<<1)      // Remote Start Address low (write)
#define NE_P0_CRDA1     (0x09<<1)      // Current Remote DMA Addr high (read)
#define NE_P0_RSAR1     (0x09<<1)      // Remote Start Address high (write)
#define NE_P0_RBCR0     (0x0A<<1)      // Remote Byte Count low (write)
#define NE_P0_RBCR1     (0x0B<<1)      // Remote Byte Count high (write)
#define NE_P0_RSR       (0x0C<<1)      // Receive Status (read)
#define NE_P0_RCR       (0x0C<<1)      // Receive Configuration Reg (write)
#define NE_P0_CNTR0     (0x0D<<1)      // Frame alignment error counter (read)
#define NE_P0_TCR       (0x0D<<1)      // Transmit Configuration Reg (write)
#define NE_P0_CNTR1     (0x0E<<1)      // CRC error counter (read)
#define NE_P0_DCR       (0x0E<<1)      // Data Configuration Reg (write)
#define NE_P0_CNTR2     (0x0F<<1)      // Missed packet counter (read)
#define NE_P0_IMR       (0x0F<<1)      // Interrupt Mask Register (write)

//
// Page 1 register offsets
//

#define NE_P1_CR        (0x00<<1)           // Command Register
#define NE_P1_PAR0      (0x01<<1)           // Physical Address Register 0
#define NE_P1_PAR1      (0x02<<1)           // Physical Address Register 1
#define NE_P1_PAR2      (0x03<<1)           // Physical Address Register 2
#define NE_P1_PAR3      (0x04<<1)           // Physical Address Register 3
#define NE_P1_PAR4      (0x05<<1)           // Physical Address Register 4
#define NE_P1_PAR5      (0x06<<1)           // Physical Address Register 5
#define NE_P1_CURR      (0x07<<1)           // Current RX ring-buffer page
#define NE_P1_MAR0      (0x08<<1)           // Multicast Address Register 0
#define NE_P1_MAR1      (0x09<<1)           // Multicast Address Register 1
#define NE_P1_MAR2      (0x0A<<1)           // Multicast Address Register 2
#define NE_P1_MAR3      (0x0B<<1)           // Multicast Address Register 3
#define NE_P1_MAR4      (0x0C<<1)           // Multicast Address Register 4
#define NE_P1_MAR5      (0x0D<<1)           // Multicast Address Register 5
#define NE_P1_MAR6      (0x0E<<1)           // Multicast Address Register 6
#define NE_P1_MAR7      (0x0F<<1)           // Multicast Address Register 7

//
// Page 2 register offsets
//

#define NE_P2_CR        (0x00<<1)           // Command Register
#define NE_P2_PSTART    (0x01<<1)           // Page Start (read)
#define NE_P2_CLDA0     (0x01<<1)           // Current Local DMA Addr 0 (write)
#define NE_P2_PSTOP     (0x02<<1)           // Page Stop (read)
#define NE_P2_CLDA1     (0x02<<1)           // Current Local DMA Addr 1 (write)
#define NE_P2_RNPP      (0x03<<1)           // Remote Next Packet Pointer
#define NE_P2_TPSR      (0x04<<1)           // Transmit Page Start (read)
#define NE_P2_LNPP      (0x05<<1)           // Local Next Packet Pointer
#define NE_P2_ACU       (0x06<<1)           // Address Counter Upper
#define NE_P2_ACL       (0x07<<1)           // Address Counter Lower
#define NE_P2_RCR       (0x0C<<1)           // Receive Configuration Register (read)
#define NE_P2_TCR       (0x0D<<1)           // Transmit Configuration Register (read)
#define NE_P2_DCR       (0x0E<<1)           // Data Configuration Register (read)
#define NE_P2_IMR       (0x0F<<1)           // Interrupt Mask Register (read)


//
// Page 3 register offsets   
//

#define NE_P3_9346CR    (0x01<<1)
#define NE_P3_CONFIG0   (0x03<<1)
#define NE_P3_CONFIG1   (0x04<<1)
#define NE_P3_CONFIG2   (0x05<<1)
#define NE_P3_CONFIG3   (0x06<<1)

#define NE_EEM1 		(1 << 7)
#define NE_EEM0 		(1 << 6)

//
// Command Register (CR)
//

#define NE_CR_STP       0x01           // Stop
#define NE_CR_STA       0x02           // Start
#define NE_CR_TXP       0x04           // Transmit Packet
#define NE_CR_RD0       0x08           // Remote DMA Command 0
#define NE_CR_RD1       0x10           // Remote DMA Command 1
#define NE_CR_RD2       0x20           // Remote DMA Command 2
#define NE_CR_PS0       0x40           // Page Select 0
#define NE_CR_PS1       0x80           // Page Select 1

#define NE_CR_PAGE_0    0x00           // Select Page 0
#define NE_CR_PAGE_1    0x40           // Select Page 1
#define NE_CR_PAGE_2    0x80           // Select Page 2
#define NE_CR_PAGE_3    0xC0           // Select Page 3

//
// Interrupt Status Register (ISR)
//

#define NE_ISR_PRX      0x01           // Packet Received
#define NE_ISR_PTX      0x02           // Packet Transmitted
#define NE_ISR_RXE      0x04           // Receive Error
#define NE_ISR_TXE      0x08           // Transmission Error
#define NE_ISR_OVW      0x10           // Overwrite
#define NE_ISR_CNT      0x20           // Counter Overflow
#define NE_ISR_RDC      0x40           // Remote Data Complete
#define NE_ISR_RST      0x80           // Reset status

//
// Interrupt Mask Register (IMR)

#define NE_IMR_PRXE     0x01           // Packet Received Interrupt Enable
#define NE_IMR_PTXE     0x02           // Packet Transmit Interrupt Enable
#define NE_IMR_RXEE     0x04           // Receive Error Interrupt Enable
#define NE_IMR_TXEE     0x08           // Transmit Error Interrupt Enable
#define NE_IMR_OVWE     0x10           // Overwrite Error Interrupt Enable
#define NE_IMR_CNTE     0x20           // Counter Overflow Interrupt Enable
#define NE_IMR_RDCE     0x40           // Remote DMA Complete Interrupt Enable

//
// Data Configuration Register (DCR)

#define NE_DCR_WTS      0x01           // Word Transfer Select
#define NE_DCR_BOS      0x02           // Byte Order Select
#define NE_DCR_LAS      0x04           // Long Address Select
#define NE_DCR_LS       0x08           // Loopback Select
#define NE_DCR_AR       0x10           // Auto-initialize Remote
#define NE_DCR_FT0      0x20           // FIFO Threshold Select 0
#define NE_DCR_FT1      0x40           // FIFO Threshold Select 1

//
// Transmit Configuration Register (TCR)

#define NE_TCR_CRC      0x01           // Inhibit CRC
#define NE_TCR_LB0      0x02           // Loopback Control 0
#define NE_TCR_LB1      0x04           // Loopback Control 1
#define NE_TCR_ATD      0x08           // Auto Transmit Disable
#define NE_TCR_OFST     0x10           // Collision Offset Enable

//
// Transmit Status Register (TSR)

#define NE_TSR_PTX      0x01           // Packet Transmitted
#define NE_TSR_COL      0x04           // Transmit Collided
#define NE_TSR_ABT      0x08           // Transmit Aborted
#define NE_TSR_CRS      0x10           // Carrier Sense Lost
#define NE_TSR_FU       0x20           // FIFO Underrun
#define NE_TSR_CDH      0x40           // CD Heartbeat
#define NE_TSR_OWC      0x80           // Out of Window Collision

//
// Receiver Configuration Register (RCR)
//

#define NE_RCR_SEP      0x01           // Save Errored Packets
#define NE_RCR_AR       0x02           // Accept Runt packet
#define NE_RCR_AB       0x04           // Accept Broadcast
#define NE_RCR_AM       0x08           // Accept Multicast
#define NE_RCR_PRO      0x10           // Promiscuous Physical
#define NE_RCR_MON      0x20           // Monitor Mode

//
// Receiver Status Register (RSR)


#define NE_RSR_PRX      0x01           // Packet Received Intact
#define NE_RSR_CRC      0x02           // CRC Error
#define NE_RSR_FAE      0x04           // Frame Alignment Error
#define NE_RSR_FO       0x08           // FIFO Overrun
#define NE_RSR_MPA      0x10           // Missed Packet
#define NE_RSR_PHY      0x20           // Physical Address
#define NE_RSR_DIS      0x40           // Receiver Disabled
#define NE_RSR_DFR      0x80           // Deferring

#define NE_NOVELL_DATA  (0x10<<1)

#define NE_PAGE_SIZE            256    // Size of RAM pages in bytes
#define NE_TXBUF_SIZE           6      // Size of TX buffer in pages
#define NE_TX_BUFERS            2      // Number of transmit buffers

#define NE_TIMEOUT              10000
#define NE_TXTIMEOUT            30000

#define ETHER_ADDR_LEN          6

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#endif
