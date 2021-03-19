/*
 * usart_ring.c
 * take from https://istarik.ru/blog/stm32/151.html
 * github - https://github.com/stDstm/Example_STM32F103.git, dir - Ring_buff_uart
 *
 */

#include "core/ring_buffer.h"
#include <stdlib.h>

/******************************************************************/
int ringbuffer_init(ringbuffer_Data*data, uint16_t a_buffer_size)
{
	data->buffer_head = 0;
	data->buffer_tail = 0;
	data->buffer_size = a_buffer_size;
	data->buffer = malloc(a_buffer_size);

	if(data->buffer == 0) return 1;
	else return 0;
}
/******************************************************************/
void ringbuffer_clear(ringbuffer_Data*data)
{
	data->buffer_head = 0;
	data->buffer_tail = 0;
}

/******************************************************************/
uint16_t ringbuffer_get_available_size(ringbuffer_Data*data)
{
	return ((uint16_t)(data->buffer_size + data->buffer_head - data->buffer_tail)) % data->buffer_size;
}

/******************************************************************/
uint8_t ringbuffer_read_byte(ringbuffer_Data*data)
{
	if(data->buffer_head == data->buffer_tail) {
		return 0;
	} else {
		uint8_t c = data->buffer[data->buffer_tail];
		data->buffer_tail = (uint16_t)(data->buffer_tail + 1) % data->buffer_size;
		return c;
	}
}
/******************************************************************/
int ringbuffer_write_byte(ringbuffer_Data*data,uint8_t a_byte)
{
	//индекс кольцевого буфера;
	uint16_t i = (uint16_t)(data->buffer_head + 1) % data->buffer_size;

	if(i != data->buffer_tail) {
		data->buffer[data->buffer_head] = a_byte;
		data->buffer_head = i;
		return 0;
	}
	return 1;
}







