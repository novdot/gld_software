/**
    RING buffer functions
*/
#ifndef _XLIB_RING_BUFFER_H_INCLUDED_
#define _XLIB_RING_BUFFER_H_INCLUDED_

#include <stdint.h>
#include "xlib/types.h"

typedef struct{
    uint8_t *buffer;
    uint16_t idxIn;
    uint16_t idxOut;
    uint16_t size;
} x_ring_buffer_t;

typedef enum{
    x_ring_error_ok = 0
}x_ring_error_t;

void x_ring_put_str(uint8_t* a_symbol, x_ring_buffer_t* a_pbuf);
void x_ring_put(uint8_t a_symbol, x_ring_buffer_t* a_pbuf);
uint8_t x_ring_pop(x_ring_buffer_t *a_pbuf);
uint16_t x_ring_get_count(x_ring_buffer_t *a_pbuf);
int32_t x_ring_get_symbol(uint16_t a_num ,x_ring_buffer_t *a_pbuf);
void x_ring_clear(x_ring_buffer_t* a_pbuf);
x_bool_t x_ring_init(x_ring_buffer_t *a_pring, uint8_t *a_pbuf, uint16_t size);

#endif //_XLIB_RING_BUFFER_H_INCLUDED_
