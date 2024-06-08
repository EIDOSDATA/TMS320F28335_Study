/*
 * Fram.c
 *
 *  Created on: 2023. 11. 15.
 *      Author: ShinSung Industrial Electric
 */
/*
#include "DSP28x_Project.h"
#include "decoder.h"
*/
#include "fram.h"


#define MASKING            0xFF
#define FRAM_DELAY  2
#define FRAM_WRITE(Address)          *((volatile uint16 *)(0x200000 + Address))
#define FRAM_READ(Address)           (*(volatile uint16 *)(0x200000 + Address)& MASKING)


uint16 FRAM_ByteRead(uint16 Addr)
{
    uint16 Data = 0;

    Data = FRAM_READ(Addr);

    Data &= 0xFF;

    return Data;
}


uint16 FRAM_WordRead(uint16 addr)
{
    uint16 Delay = FRAM_DELAY;
    uint16 Data = 0;

    Data = FRAM_READ(addr);

    while(Delay--);

    Data |= FRAM_READ(addr+1)<<8;

    return Data;
}
void FRAM_ByteWrite(uint16 Addr, uint16 Data)
{
    FRAM_WRITE(Addr) = Data&MASKING;
}
void FRAM_WordWrite(uint16 addr, uint16 Data)
{
    uint16 Delay = FRAM_DELAY;

    FRAM_WRITE(addr++) = Data&MASKING;

    Data = Data>>8;

    while(Delay--);

    FRAM_WRITE(addr) = Data;

}
