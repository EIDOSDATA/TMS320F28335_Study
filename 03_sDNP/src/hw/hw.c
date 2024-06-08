/*
 * hw.c
 *
 *  Created on: 2024. 5. 2.
 *      Author: ShinSung Industrial Electric
 */

#include "hw.h"

void hwInit(void)
{
    InitPeripheralClocks();
    InitGpio();
    InitFlash();
    XintfInit();

}
