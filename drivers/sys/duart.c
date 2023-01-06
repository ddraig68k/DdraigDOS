#include <string.h>

// NOTE: Currently not used, dropping characters when recieving,
// Needs debugging or possibly handling interrupt in assembler

#include "duart.h"
#include "libraries/ringbuffer.h"

#define DUART_BUFFER_SIZE 256

uint8_t g_rxa_buf_data[DUART_BUFFER_SIZE];
uint8_t g_txa_buf_data[DUART_BUFFER_SIZE];
uint8_t g_rxb_buf_data[DUART_BUFFER_SIZE];
uint8_t g_txb_buf_data[DUART_BUFFER_SIZE];

typedef struct
{
    uint8_t         imr;            // Interrupt mask register value                                    */
    uint8_t         dummy;          // Dummy byte for register reads
    ringbuffer_t    rxa_buf;
    ringbuffer_t    txa_buf;
    ringbuffer_t    rxb_buf;
    ringbuffer_t    txb_buf;
} mc68681_state_t;

mc68681_state_t g_duart_state;

void __attribute__((interrupt)) mc68681_interrupt(void)
{
    uint8_t isr = DUART_ISR;

    if (isr & ISR_CH_A_RX_READY_FULL)
    {
            uint8_t ch = DUART_RBA;
            ring_buf_put(&g_duart_state.rxa_buf, ch);
    }

    if (isr & ISR_CH_A_TX_READY)
    {
        if (!is_ring_buf_empty(&g_duart_state.txa_buf))
        {
            DUART_TBA = ring_buf_get(&g_duart_state.txa_buf);
        }
        else
        {
            // Disable TX interrupt
            g_duart_state.imr &= ~ISR_CH_A_TX_READY;
            DUART_IMR = g_duart_state.imr;
        }
    }

    if (isr & ISR_CH_B_RX_READY_FULL)
    {
            uint8_t ch = DUART_RBB;
            ring_buf_put(&g_duart_state.rxb_buf, ch);
    }

    if (isr & ISR_CH_B_TX_READY)
    {
        if (!is_ring_buf_empty(&g_duart_state.txb_buf))
        {
            DUART_TBB = ring_buf_get(&g_duart_state.txb_buf);
        }
        else
        {
            // Disable TX interrupt
            g_duart_state.imr &= ~ISR_CH_B_TX_READY;
            DUART_IMR = g_duart_state.imr;
        }
    }

    if (isr & ISR_TIMER_CHANGE)
    {
    }

    volatile __attribute__((unused)) uint8_t clear_int = DUART_STOPCC;
}

// initialise the  DUART
void mc68681_init()
{
    memset(&g_duart_state, 0, sizeof(mc68681_state_t));
    ring_buf_init(&g_duart_state.rxa_buf, rxa_buf_data, DUART_BUFFER_SIZE);
    ring_buf_init(&g_duart_state.txa_buf, txa_buf_data, DUART_BUFFER_SIZE);
    ring_buf_init(&g_duart_state.rxb_buf, rxb_buf_data, DUART_BUFFER_SIZE);
    ring_buf_init(&g_duart_state.txb_buf, txb_buf_data, DUART_BUFFER_SIZE);

    DUART_CRA = 0x20;       // reset receiver
    DUART_CRA = 0x10;       // reset MR pointer to MRx1
    DUART_CRB = 0x20;       // reset receiver
    DUART_CRB = 0x10;       // reset MR pointer to MRx1
    DUART_ACR = 0x60;       // select BAUD rate set 1

    DUART_MR1A = 0x93;      // set 8bits,no parity,character mode,RxRDY IRQ,RxRTS enabled
    DUART_MR2A = 0x27;      // $37 Normal Mode,enable TxRTS & TxCTS,one Stop bit
    DUART_CSRA = 0xCC;      // set TX & RX to 38400 BAUD
    DUART_CRA = 0x05;       // enable Receiver & Transmitter

    DUART_MR1B = 0x93;      // set 8bits,no parity,character mode,RxRDY IRQ,RxRTS enabled
    DUART_MR2B = 0x37;      // $37 Normal Mode,enable TxRTS & TxCTS,one Stop bit
    DUART_CSRB = 0xCC;      // set TX & RX to 38400 BAUD
    DUART_CRB = 0x05;       // enable Receiver & Transmitter

    DUART_OPRSET = 0x05;    // $01 assert rts

    // Timer to 1000ms
    // Calculated at Clock Freq (3686400) / Prescaler (16) / Rate (Htz)
    // Rate 10 Hz = 3686400 / 16/ 10 = 23040 (0x5a00)
    DUART_IMR = 0x08;       // mask in the timer overflow
    g_duart_state.imr = 0x08;
    DUART_CTUR = 0x5a;      // Load the timer counter value
    DUART_CTLR = 0x00;

    ISR_VECT_DUART(mc68681_interrupt);
}

uint8_t serial_has_char(void)
{
    return !is_ring_buf_empty(&g_duart_state.rxa_buf);
}

uint8_t serial_get(void)
{
    return ring_buf_get(&g_duart_state.rxa_buf);
}

void _putchar_serial(char c)
{
    if (c == '\n')
    {
        while (is_ring_buf_full(&g_duart_state.txa_buf))
        {}

        ring_buf_put(&g_duart_state.txa_buf, '\r');
        g_duart_state.imr |= ISR_CH_A_TX_READY;
        DUART_IMR = g_duart_state.imr;
    }

    while (is_ring_buf_full(&g_duart_state.txa_buf))
    {}

    ring_buf_put(&g_duart_state.txa_buf, '\r');
    g_duart_state.imr |= ISR_CH_A_TX_READY;
    DUART_IMR = g_duart_state.imr;
}

int8_t serialb_haschar(void)
{
    return !is_ring_buf_empty(&g_duart_state.rxb_buf);
}

int8_t serialb_get(void)
{
    return ring_buf_get(&g_duart_state.rxb_buf);
}

void serialb_put(char c)
{
    while (is_ring_buf_full(&g_duart_state.txb_buf))
    {}

    ring_buf_put(&g_duart_state.txb_buf, c);
    g_duart_state.imr |= ISR_CH_B_TX_READY;
    DUART_IMR = g_duart_state.imr;
}

uint8_t read_serial_inputs(void)
{
    return DUART_IP;
}