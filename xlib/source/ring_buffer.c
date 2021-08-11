#include "xlib/ring_buffer.h"

/******************************************************************************/
void x_ring_put_str(uint8_t* a_symbol, x_ring_buffer_t* a_pbuf)
{
    while (*a_symbol != 0) {
        a_pbuf->buffer[a_pbuf->idxIn++] = (*a_symbol++);	
        if (a_pbuf->idxIn >= a_pbuf->size) a_pbuf->idxIn = 0;
    }
}
/******************************************************************************/
void x_ring_put(uint8_t a_symbol, x_ring_buffer_t* a_pbuf)
{
    if(a_pbuf->idxIn>=a_pbuf->size) return;
    a_pbuf->buffer[a_pbuf->idxIn++] = a_symbol;
    if (a_pbuf->idxIn >= a_pbuf->size) a_pbuf->idxIn = 0;
}
/******************************************************************************/
uint8_t x_ring_pop(x_ring_buffer_t *a_pbuf)
{
    uint8_t retval = 0;
    if(a_pbuf->idxOut>=a_pbuf->size) return retval;
    retval = a_pbuf->buffer[a_pbuf->idxOut++];
    if (a_pbuf->idxOut >= a_pbuf->size) a_pbuf->idxOut = 0;
    return retval;
}
/******************************************************************************/
uint16_t x_ring_get_count(x_ring_buffer_t *a_pbuf)
{
    uint16_t retval = 0;
    if (a_pbuf->idxIn < a_pbuf->idxOut) retval = a_pbuf->size + a_pbuf->idxIn - a_pbuf->idxOut;
    else retval = a_pbuf->idxIn - a_pbuf->idxOut;
    return retval;
}
/******************************************************************************/
int32_t x_ring_get_symbol(uint16_t a_num ,x_ring_buffer_t *a_pbuf)
{
    uint32_t pointer = a_pbuf->idxOut + a_num;
    int32_t  retval = -1;
    if (a_num < x_ring_get_count(a_pbuf))
    {
        if (pointer > a_pbuf->size) pointer -= a_pbuf->size;
        retval = a_pbuf->buffer[ pointer ] ;
    }
    return retval;
}
/******************************************************************************/
void x_ring_clear(x_ring_buffer_t* a_pbuf)
{
    a_pbuf->idxIn = 0;
    a_pbuf->idxOut = 0;
}
/******************************************************************************/
x_bool_t x_ring_init(x_ring_buffer_t *a_pring, uint8_t *a_pbuf, uint16_t a_size)
{
    a_pring->size = a_size;
    a_pring->buffer = a_pbuf;
    x_ring_clear( a_pring );

    return ( a_pring->buffer ? _x_true : _x_false ) ;
}
/******************************************************************************/