#include "DSP28x_Project.h"
#include "main.h"
#include "f2833x_pinmux.h"

/**
 * main.c
 */
int main(void)
{
    InitSysCtrl();

    EALLOW;

    GPIO_setPinMuxConfig();

    /* GPIO 60 ~ 61 */
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;

    /* GPIO 02 ~ 07 */
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;

    EDIS;
    /* GPIO CFG MEMORY PROTECTION */
    while (1)
    {
        GpioDataRegs.GPBTOGGLE.bit.GPIO60 = 1;
        GpioDataRegs.GPBTOGGLE.bit.GPIO61 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO4 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO6 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO7 = 1;
        DELAY_US(1000000);
    }
    return 0;
}

/*
 * C2000 Ware PATHt : OK
 * */
