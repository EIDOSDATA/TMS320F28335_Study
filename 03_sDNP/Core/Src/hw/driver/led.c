/*
 * led.c
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */

#include "led.h"


#define LED_BLOCK_WRITE(block, data)      *((volatile uint16 *)(OFFSET_ADDR + block)) = data

void ledInit(void)
{
    LED_BLOCK_WRITE(0, 0xFF);
    LED_BLOCK_WRITE(1, 0xFF);
    LED_BLOCK_WRITE(2, 0xFF);
    LED_BLOCK_WRITE(3, 0xFF);
    LED_BLOCK_WRITE(6, 0xFF);
}

