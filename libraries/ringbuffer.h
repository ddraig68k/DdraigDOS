#ifndef DDRAIG_OS_RINGBUFFER_H
#define DDRAIG_OS_RINGBUFFER_H

#include "ddraig.h"

typedef struct ring_buffer
{
	uint8_t 			*buffer;
	volatile uint16_t 	head;
	volatile uint16_t 	tail;
	volatile uint16_t 	length;
} ringbuffer_t;

void ring_buf_init(ringbuffer_t *rb, uint8_t *buf_data, uint16_t size);
uint8_t ring_buf_get(ringbuffer_t *rb);
void ring_buf_put(ringbuffer_t *rb, const uint8_t c);

static inline uint8_t is_ring_buf_full(ringbuffer_t *rb)
{
	uint16_t head = rb->head;
	uint16_t tail = rb->tail;
	return (((head + 1) % rb->length) == tail);
}

static inline uint8_t is_ring_buf_empty(ringbuffer_t *rb)
{
	uint16_t head = rb->head;
	uint16_t tail = rb->tail;
	return (head == tail);
}

static inline uint16_t ring_buf_avail(ringbuffer_t *rb)
{
	uint16_t head = rb->head;
	uint16_t tail = rb->tail;
	return rb->length - (head - tail);
}

#endif
