#include "rtl8019as.h"

#include <printf.h>
#include "drivers/network.h"

struct ne2k_struct ne2k;

#define RTLWRITE_BYTE(x, y)	        (*((uint8_t *) (x)) = (y))
#define RTLREAD_BYTE(x)	            (*((volatile uint8_t *) (x)))

uint8_t g_rtl8019_mac_addr[6] = { 0xD1, 0x8F, 0x9D, 0xCA, 0x2F, 0xD1 };

void rtl8019_dump_headers()
{
	struct recv_ring_desc header;
	for (uint16_t i = 0x40; i < 0x60; i++)
	{
		 // Read receive ring descriptor
		ne2k_readmem(i * NE_PAGE_SIZE, &header, sizeof(struct recv_ring_desc));

		printf("Page 0x%02x - RSR: 0x%02x, Next Pkt: 0x%02x, Length: 0x%04x\n", i, header.rsr, header.next_pkt, header.count);
	}
	
}

void rtl8019_device_init(network_device_t *device)
{
	uint8_t i = 0; 

 	ne2k.addrbase = device->base_address;
	for (i = 0; i < 6; i++)
		ne2k.mac[i] = g_rtl8019_mac_addr[i];

	ne2k.rx_page_start = 0x40; // first page at 16k

	// 12 pages (2x 1536 bytes) at the end of the SRAM as a transmit buffer
	ne2k.rx_page_stop = 0x60 - (NE_TXBUF_SIZE * NE_TX_BUFERS); // last page at 0x60 (not 0x80 (!), because we're in 8bit mode, see RTL8019AS datasheet, p.15)
	ne2k.next_pkt = ne2k.rx_page_start + 1;

	ne2k.rx_ring_start = ne2k.rx_page_start * NE_PAGE_SIZE;
	ne2k.rx_ring_end = ne2k.rx_page_stop * NE_PAGE_SIZE;

	printf("[NE2k] Resetting card...\n");

	RTLWRITE_BYTE(ne2k.addrbase + (0x1F<<1), RTLREAD_BYTE(ne2k.addrbase + (0x1F<<1)));  // write the value of RESET into the RESET register
	while ((RTLREAD_BYTE(ne2k.addrbase + (0x07<<1)) & 0x80) == 0);	  // wait for the RESET to complete
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_ISR, 0xFF);					 // mask interrupts

	printf("[NE2k] Card reset successfully.\n");

	// Set page 0 registers, abort remote DMA, stop NIC
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);	

	// Set FIFO threshold to 8, no auto-init remote DMA, byte order=80x86, byte-wide DMA transfers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_DCR, NE_DCR_FT1 | NE_DCR_LS);

	// Set page 3 registers (RTL8019 specific)
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_PAGE_3 | NE_CR_RD2 | NE_CR_STP);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P3_9346CR, (uint8_t) (NE_EEM0 | NE_EEM1));
	RTLWRITE_BYTE(ne2k.addrbase + NE_P3_CONFIG1, 0x00);  // io=0x300
	RTLWRITE_BYTE(ne2k.addrbase + NE_P3_CONFIG2, 0x00);  // io=0x300
	RTLWRITE_BYTE(ne2k.addrbase + NE_P3_CONFIG3, 0x50); // fdx, leds on

	// Set page 0 registers, abort remote DMA, stop NIC
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

	// Clear remote byte count registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR0, 0);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR1, 0);

	// Initialize receiver (ring-buffer) page stop and boundry
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_PSTART, ne2k.rx_page_start);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_PSTOP, ne2k.rx_page_stop);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_BNRY, ne2k.rx_page_start);

	// Enable the following interrupts: receive/transmit complete, receive/transmit error, 
	// receiver overwrite and remote dma complete.
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_IMR, NE_IMR_PRXE | NE_IMR_PTXE | NE_IMR_RXEE | NE_IMR_TXEE | NE_IMR_OVWE | NE_IMR_RDCE);

	// Set page 1 registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STP);

	// Copy out our station address
	for (i = 0; i < ETHER_ADDR_LEN; i++)
		RTLWRITE_BYTE(ne2k.addrbase + NE_P1_PAR0 + i, ne2k.mac[i]);

	// Set current page pointer 
	RTLWRITE_BYTE(ne2k.addrbase + NE_P1_CURR, ne2k.next_pkt);

	// Initialize multicast address hashing registers to not accept multicasts
	for (i = 0; i < 8; i++)
		RTLWRITE_BYTE(ne2k.addrbase + NE_P1_MAR0 + i, 0);

	// Set page 0 registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STP);

	// Accept broadcast packets
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RCR, NE_RCR_AB);

	// Take NIC out of loopback
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TCR, 0);

	// Clear any pending interrupts
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_ISR, 0xFF);

	// Start NIC
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

	printf("[NE2k] init done!\n");
}

int ne2k_transmit(uint8_t *packet, uint16_t length)
{
	unsigned short dst;
	uint16_t i; 

	while (RTLREAD_BYTE(ne2k.addrbase + NE_P0_CR) & NE_CR_TXP)
	{
		// packet is still being sent. waiting...
	}

	// Set page 0 registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

	// Reset remote DMA complete flag
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_ISR, NE_ISR_RDC);

	// Set up DMA byte count
	if (length > 64)
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR0, (unsigned char) (length & 0xff));
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR1, (unsigned char) (length >> 8));
	}
	else
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR0, (unsigned char) 64);
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR1, (unsigned char) 0);
	}
	
	// Set up destination address in NIC memory
	dst = ne2k.rx_page_stop; // for now we only use one tx buffer
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR0, (dst * NE_PAGE_SIZE) & 0xff);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR1, (dst * NE_PAGE_SIZE) >> 8);

	// Set remote DMA write
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD1/* | NE_CR_STA*/);

	for (i = 0; i < length; ++i)
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_NOVELL_DATA, packet[i]);
	}
	while (i++ < 64)
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_NOVELL_DATA, 0x00);
	}

	// Set TX buffer start page
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TPSR, dst);

	// Set TX length (packets smaller than 64 bytes must be padded)
	if (length > 64)
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TBCR0, length);
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TBCR1, (length >> 8));
	}
	else
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TBCR0, 64);
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_TBCR1, 0);
	}

	// Set page 0 registers, transmit packet, and start
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_TXP | NE_CR_STA);

	printf("[NE2k] Transmitted packet with length %d\n", length);
	/*print_memory(uip_buf, length);
	myprintf("\n\n");*/
   // z80_delay_ms(100);

	return 0;
}

void ne2k_readmem(uint16_t src, void *dst, uint16_t len)
{   
	uint16_t i;

	// Abort any remote DMA already in progress
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

	// Setup DMA byte count
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR0, (uint8_t) (len & 0xff));
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR1, (uint8_t) (len >> 8));

	// Setup NIC memory source address
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR0, (uint8_t) (src & 0xff));
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR1, (uint8_t) (src >> 8));

	// Select remote DMA read
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD0 | NE_CR_STA);

	// Read NIC memory
	for (i = 0; i < len; i++)
	{
		((uint8_t*)dst)[i] = RTLREAD_BYTE(ne2k.addrbase + NE_NOVELL_DATA);
	}
}

int ne2k_writemem(uint16_t dst, uint8_t *src, uint16_t length)
{
	uint16_t i;

	// Set page 0 registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD2 | NE_CR_STA);

	// Reset remote DMA complete flag
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_ISR, RTLREAD_BYTE(ne2k.addrbase + NE_P0_ISR) & ~NE_ISR_RDC);

	// Set up destination address in NIC memory
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR0, dst & 0xff);
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RSAR1, dst >> 8);
	
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR0, (unsigned char) (length & 0xff));
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_RBCR1, (unsigned char) (length >> 8));

	// Set remote DMA write
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_RD1 | NE_CR_STA);

	for (i = 0; i < length; ++i)
	{
		RTLWRITE_BYTE(ne2k.addrbase + NE_NOVELL_DATA, src[i]);
	}

	return 0;
}

void ne2k_get_packet(uint16_t src, char *dst, uint16_t len)
{
	if (src + len > ne2k.rx_ring_end)
	{
		uint16_t split = ne2k.rx_ring_end - src;

		ne2k_readmem(src, dst, split);
		len -= split;
		src = ne2k.rx_ring_start;
		dst += split;
	}

	ne2k_readmem(src, dst, len);
}

uint16_t ne2k_receive()
{
#if 0
	struct recv_ring_desc packet_hdr;
	unsigned short packet_ptr;
	unsigned short len;
	unsigned char bndry;

	// Set page 1 registers
	RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STA);

	if (ne2k.next_pkt != RTLREAD_BYTE(ne2k.addrbase + NE_P1_CURR))
	{
		// Get pointer to buffer header structure
		packet_ptr = ne2k.next_pkt * NE_PAGE_SIZE;

		// Read receive ring descriptor
		ne2k_readmem(packet_ptr, &packet_hdr, sizeof(struct recv_ring_desc));

		// This was once caused in 8bit mode with a page stop behind 0x60 (which isn't allowed according to the RTL8019 datasheet.)
		// It shouldn't and probably will not happen in any normal operation. 
		if (!(packet_hdr.rsr & 0x01))
		{
//			myprintf("[NE2k] Packet read with invalid RSR, Page: 0x%02x, RSR: 0x%02x, Next Pkt: 0x%02x, Length: 0x%04x\n", ne2k.next_pkt, packet_hdr.rsr, packet_hdr.next_pkt, packet_hdr.count);
			return 0;
		}

		len = packet_hdr.count - sizeof(struct recv_ring_desc);
		//myprintf("[NE2k] received packet, %u bytes\r\n", len);
		if (len > UIP_BUFSIZE)
		{
			myprintf("[NE2k] packet too large. dumping all packet headers!\n");
			ne2k_dump_headers();
			return 0;
		}
			
		// Fetch packet payload
		packet_ptr += sizeof(struct recv_ring_desc);
		ne2k_get_packet(packet_ptr, uip_buf, len);

		// Set the read pointer to the page number give in the received header
		ne2k.next_pkt = packet_hdr.next_pkt;

		// Set page 0 registers
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_PAGE_0 | NE_CR_RD2 | NE_CR_STA);

		// Update boundry pointer
		bndry = ne2k.next_pkt - 1;
		if (bndry < ne2k.rx_page_start) bndry = ne2k.rx_page_stop - 1;
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_BNRY, bndry);

		//myprintf("[NE2k] 2: start: %02x stop: %02x next: %02x bndry: %02x\r\n", ne2k.rx_page_start, ne2k.rx_page_stop, ne2k.next_pkt, bndry);

		// Set page 1 registers
		RTLWRITE_BYTE(ne2k.addrbase + NE_P0_CR, NE_CR_PAGE_1 | NE_CR_RD2 | NE_CR_STA);

		return len;
	}
#endif
	return 0;
}

#pragma pack(1)
struct frame
{
	uint8_t dst_mac[6];                 // MAC address
	uint8_t src_mac[6];                 // MAC address
	uint16_t type;
	uint8_t data[1500];
};

void transmit_wrapper(void)
{
	struct frame my_frame;
	
	my_frame.dst_mac[0] = 0xfC;
	my_frame.dst_mac[1] = 0xAA;
	my_frame.dst_mac[2] = 0x14;
	my_frame.dst_mac[3] = 0x0A;
	my_frame.dst_mac[4] = 0x96;
	my_frame.dst_mac[5] = 0x9E;

	for (int i = 0; i < 6; i++)
		my_frame.src_mac[i] = g_rtl8019_mac_addr[i];

	my_frame.type = 0x0888;

	for (uint16_t c = 0; c < 1500; c++)
		my_frame.data[c] = (uint8_t)c;

	ne2k_transmit((uint8_t *) &my_frame, sizeof(struct frame));
}

void readmem_wrapper(void)
{
	ne2k_readmem(ne2k.rx_page_stop * NE_PAGE_SIZE, (uint8_t *) 0x20000, sizeof(struct frame));
}
