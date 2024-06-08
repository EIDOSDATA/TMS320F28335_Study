/*
 * Decoder.c
 *
 *  Created on: 2023. 10. 31.
 *      Author: ShinSung Industrial Electric
 */


#include "src/common/hw/include/decoder.h"


/*
 * Pin Description
 * Pin          Net Name
 * GPIO53       MEM_CS0
 * GPIO52       MEM_CS1
 * GPIO51       MEM_CS2
 */

/*HW Register*/
volatile struct GPIO_DATA_REGS* pGpioDataRegs = &GpioDataRegs;

static Delay_loop16(uint16 delay);

void ChipSeclect(Zone7DeviceEnum Device)
{
    switch(Device)
    {
        case FLASH:     /*000(MEM_CS0 = 0, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case FRAM:      /*001(MEM_CS0 = 1, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case MMI_READ:      /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBSET.bit.GPIO54    = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;
        case MMI_WRITE:     /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO54  = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;

            break;

        case W5100S:    /*011(MEM_CS0 = 1, MEM_CS1 = 1, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        default:
            break;
    }

}

void ChipSelectNone(void)
{
    Delay_loop16(1);
    pGpioDataRegs->GPBCLEAR.bit.GPIO50 = 1;
    Delay_loop16(1);
}

void Zone7BusTake(Zone7DeviceEnum Device)
{
   // SEMAPHORE_PEND(SemZone7, WAIT_FOREVER);
    switch(Device)
    {
        case FLASH:     /*000(MEM_CS0 = 0, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case FRAM:      /*001(MEM_CS0 = 1, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case MMI_READ:      /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBSET.bit.GPIO54    = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;
        case MMI_WRITE:     /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO54  = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;

            break;

        case W5100S:    /*011(MEM_CS0 = 1, MEM_CS1 = 1, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        default:
            break;
    }
    Delay_loop16(10);
}

void Zone7BusRelease(void)
{
    pGpioDataRegs->GPBCLEAR.bit.GPIO50 = 1;
    Delay_loop16(15);
 //   SEMAPHORE_POST(SemZone7);
}

void Zone7BusTaskWithoutSem(Zone7DeviceEnum Device)
{
    switch(Device)
    {
        case FLASH:     /*000(MEM_CS0 = 0, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case FRAM:      /*001(MEM_CS0 = 1, MEM_CS1 = 0, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO52  = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        case MMI_READ:      /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBSET.bit.GPIO54    = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;
        case MMI_WRITE:     /*010(MEM_CS0 = 0, MEM_CS1 = 1, MEM_CS2 = 0)*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO54  = 1;        /*MMI Direction*/

            pGpioDataRegs->GPBCLEAR.bit.GPIO53  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;

            break;

        case W5100S:    /*011(MEM_CS0 = 1, MEM_CS1 = 1, MEM_CS2 = 0)*/
            pGpioDataRegs->GPBSET.bit.GPIO53    = 1;
            pGpioDataRegs->GPBSET.bit.GPIO52    = 1;
            pGpioDataRegs->GPBCLEAR.bit.GPIO51  = 1;
            pGpioDataRegs->GPBSET.bit.GPIO50    = 1;
            break;

        default:
            break;
    }

    Delay_loop16(1);
}

static Delay_loop16(uint16 delay)
{
    while(delay--);
}
