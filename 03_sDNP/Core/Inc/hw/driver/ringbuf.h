/*
 * Ringbuf.h
 *
 *  Created on: 2023. 9. 13.
 *      Author: ShinSung Industrial Electric
 */

#ifndef COMPONENTS_UTILITY_RINGBUF_H_
#define COMPONENTS_UTILITY_RINGBUF_H_

#include <string.h>
#include "types.h"

typedef struct
{
    uint16  head;
    uint16  tail;
    uint16  length;
    uint16  max;

    void    *pbuf;
} Ringbuf_t;


bool Ringbuf_Create(Ringbuf_t *phandle, void *pbuf, uint16 size);
bool Ringbuf_Write(Ringbuf_t *phandle, void *pbuf, uint16 writesize);
uint16 Ringbuf_Read(Ringbuf_t *phandle, void *pbuf, uint16 bufsize);

/*
 * inline function
 */
inline uint16 Ringbuf_Length(const Ringbuf_t *phandle)
{
    return phandle->length;
}

#endif /* COMPONENTS_UTILITY_RINGBUF_H_ */
