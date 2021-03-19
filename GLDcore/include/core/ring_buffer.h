/*
 * usart_ring.h
 * take from https://istarik.ru/blog/stm32/151.html
 * github - https://github.com/stDstm/Example_STM32F103.git, dir Ring_buff_uart
 */

#ifndef __RING_BUFFER_H_INCLUDED__
#define __RING_BUFFER_H_INCLUDED__

#include <stdint.h>
typedef struct{
	volatile uint16_t buffer_head;
	volatile uint16_t buffer_tail;
	volatile uint16_t buffer_size;
	uint8_t*buffer;
}ringbuffer_Data;

/**
 * @brief создаем буфер размером a_buffer_size
 */
int ringbuffer_init(ringbuffer_Data*data, uint16_t a_buffer_size);

/**
 * @brief возвращает доступный размер кольцевого буфера
 */
uint16_t ringbuffer_get_available_size(ringbuffer_Data*data);

/**
 * @brief читает байт из кольцевого буфера
 */
uint8_t ringbuffer_read_byte(ringbuffer_Data*data);

/**
 * @brief  Установить указатель записи на начало приёмного буфера
 * @warning Обращать внимание на борьбу за ресурсы
 */
void ringbuffer_clear(ringbuffer_Data*data);

/**
 * @brief записывает байт в буфер
 */
int ringbuffer_write_byte(ringbuffer_Data*data,uint8_t a_byte);


#endif //__RING_BUFFER_H_INCLUDED__

















