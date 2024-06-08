/*
 * Decoder.h
 *
 *  Created on: 2023. 10. 31.
 *      Author: ShinSung Industrial Electric
 */

#ifndef BSP_ETHERNET_DECODER_H_
#define BSP_ETHERNET_DECODER_H_

#include "hw_def.h"

typedef enum
{
    FLASH,
    FRAM,
    MMI_READ,
    MMI_WRITE,
    W5100S

} Zone7DeviceEnum;

void ChipSeclect(Zone7DeviceEnum Device);
void ChipSelectNone(void);

extern void Zone7BusTake(Zone7DeviceEnum Device);
extern void Zone7BusRelease(void);



#endif /* BSP_ETHERNET_DECODER_H_ */
