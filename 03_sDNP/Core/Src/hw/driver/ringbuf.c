/*
 * Ringbuf.c
 *
 *  Created on: 2023. 9. 13.
 *      Author: ShinSung Industrial Electric
 */

#include "ringbuf.h"

/*
 *
 */
#define GET_MIN(a,b)            (((a) > (b))? (b):(a))


/*
 * Private Function
 *
 */

/*
 * Description : memory copy from ringbuffer to pbuf
 */
static Core_Read(Ringbuf_t *phandle, void *pbuf, uint16 bufsize)
{
    uint16 *pS1 = phandle->pbuf;
    uint16 *pS2 = pbuf;
    uint16 index = phandle->tail;

    uint16 contiguous = phandle->max - index;
    uint16 remained = (contiguous < bufsize) ? bufsize - contiguous : 0;

    uint16 cut = bufsize - remained;

    memcpy(pS2, &pS1[index], cut);
    memcpy(pS2 + cut, pS1, remained);

    return bufsize;
}
/*
 *  Description : ringbuffer index adjust after Core read
 */
static bool Core_Consume(Ringbuf_t *phandle, uint16 consume_size)
{
    uint16 max = phandle->max;
    uint16 tail = phandle->tail;
    uint16 new_position = tail + consume_size;

    if(new_position >= max)             phandle->tail = new_position - max;

    else                                    phandle->tail += consume_size;

    phandle->tail = phandle->tail%phandle->max;

    phandle->length = ((phandle->head - phandle->tail) + phandle->max)%phandle->max;

    return true;
}
/*
 *  Description : public function
 */
uint16 Ringbuf_Read(Ringbuf_t *phandle, void *pbuf, uint16 bufsize)
{
    if(bufsize > phandle->length)        return false;

    uint16 bytes_read = Core_Read(phandle, pbuf, bufsize);

    if(bytes_read)      Core_Consume(phandle, bytes_read);

    else                bytes_read = 0;

    return bytes_read;
}
/*
 * Description : public function
 */
bool Ringbuf_Write(Ringbuf_t *phandle, void *pbuf, uint16 writesize)
{
    if(!phandle)        return false;
    uint16 *pS1 = phandle->pbuf;
    uint16 *pS2 = pbuf;
    uint16 contiguous = phandle->max - phandle->head;

    uint16 remained = (contiguous < writesize)?    writesize - contiguous : 0;

    memcpy(pS1 + phandle->head, pS2, writesize - remained);
    memcpy(pS1, pS2 + writesize - remained, remained);

    phandle->head  += writesize;

    phandle->head   = phandle->head%phandle->max;

    phandle->length = ((phandle->head - phandle->tail) + phandle->max)%phandle->max;

    return true;
}

/* type         : public function
 * Description  :
 */
bool Ringbuf_Create(Ringbuf_t *phandle, void *pbuf, uint16 size)
{
    if(!phandle)        return false;

    if(!pbuf)           return false;

    phandle->head = 0;
    phandle->tail = 0;
    phandle->length = 0;
    phandle->max = size;

    memset(pbuf, 0, size);
    phandle->pbuf = pbuf;

    return true;
}
