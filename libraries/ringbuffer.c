#include "libraries/ringbuffer.h"

void ring_buf_init(ringbuffer_t *rb, uint8_t *buf_data, uint16_t size)
{
	rb->buffer = buf_data;
	rb->head = 0;
	rb->tail = 0;
	rb->length = size;
}

uint8_t ring_buf_get(ringbuffer_t *rb)
{
	if (!rb->buffer)
		return 0;

	uint8_t ret = rb->buffer[rb->tail];
	rb->tail = (rb->tail + 1) % rb->length;

	return ret;
}

void ring_buf_put(ringbuffer_t *rb, const uint8_t c)
{
	if (!rb->buffer)
		return;

	rb->buffer[rb->head] = c;
	rb->head = (rb->head + 1) % rb->length;
	if (rb->head == rb->tail)
		rb->tail = (rb->tail + 1) % rb->length;
}
