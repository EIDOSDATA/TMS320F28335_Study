// TI File $Revision: /main/2 $
// Checkin $Date: March 15, 2007   16:54:36 $
//###########################################################################
//
// FILE:   DSP2833x_ECap.c
//
// TITLE:  DSP2833x eCAP Initialization & Support Functions.
//
//###########################################################################
// $TI Release: 2833x/2823x Header Files V1.32 $
// $Release Date: June 28, 2010 $
// $Copyright:
// Copyright (C) 2009-2023 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File

//
// InitECap - This function initializes the eCAP(s) to a known state.
//
void 
InitECap(void **pRegister)
{
    Uint16 i;
    volatile struct ECAP_REGS *pECAP = &ECap1Regs;
    *pRegister = (void*)pECAP;

    for(i=0; i<6; i++)
    {
        pECAP[i].ECEINT.all               = 0;        //  ECAP Interrupt Clear
        pECAP[i].ECCLR.all                = 0xFFFF;   //  ECAP Interrupt flag Clear
        pECAP[i].ECCTL1.bit.CAPLDEN       = 0;        //  Disable CAP1-4 Register Load at capture event time
        pECAP[i].ECCTL2.bit.TSCTRSTOP     = 0;        //  TSCTR(Time Stamp Counter) Stop

        pECAP[i].ECCTL2.bit.CAP_APWM      = 0;        //  Capture Mode
        pECAP[i].ECCTL2.bit.CONT_ONESHT   = 0;        //  Continuous Mode
        pECAP[i].ECCTL2.bit.STOP_WRAP     = 3;        //  Stop at Capture Event 4(One Shot Mode)

        pECAP[i].ECCTL1.bit.CAP1POL       = 0;        //  CAP1 Polarity rising edge
        pECAP[i].ECCTL1.bit.CAP2POL       = 0;        //  CAP2 Polarity rising edge
        pECAP[i].ECCTL1.bit.CAP3POL       = 0;        //  CAP3 Polarity rising edge
        pECAP[i].ECCTL1.bit.CAP4POL       = 0;        //  CAP4 Polarity rising edge

        pECAP[i].ECCTL1.bit.CTRRST1       = 1;        //  Reset counter After capture event1(Difference operation)
        pECAP[i].ECCTL1.bit.CTRRST2       = 1;        //  Reset counter After capture event2(Difference operation)
        pECAP[i].ECCTL1.bit.CTRRST3       = 1;        //  Reset counter After capture event3(Difference operation)
        pECAP[i].ECCTL1.bit.CTRRST4       = 1;        //  Reset counter After capture event4(Difference operation)
        pECAP[i].ECCTL1.bit.PRESCALE      = 0;        //  No Prescale divide by 1 (150M/1)

        pECAP[i].ECCTL2.bit.SYNCI_EN      = 0;        //  Disable sync in
        pECAP[i].ECCTL2.bit.SYNCO_SEL     = 2;        //  Disable sync out

        pECAP[i].ECCTL2.bit.TSCTRSTOP     = 1;        //  Start Counter
        pECAP[i].ECCTL1.bit.CAPLDEN       = 1;        //  Eisable CAP1-4 Register Load at capture event time
    }
#if 0
    /*Initialize ECAP 1*/
    ECap1Regs.ECEINT.all = 0;                   //  ECAP Interrupt Clear
    ECap1Regs.ECCLR.all = 0xFFFF;               //  ECAP Interrupt flag Clear
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;           //  Disable CAP1-4 Register Load at capture event time
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;         //  TSCTR(Time Stamp Counter) Stop

    ECap1Regs.ECCTL2.bit.CAP_APWM = 0;          //  Capture Mode
    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 1;       //  One Shot Mode
    ECap1Regs.ECCTL2.bit.STOP_WRAP = 3;         //  Stop at Capture Event 4(One Shot Mode)

    ECap1Regs.ECCTL1.bit.CAP1POL = 0;           //  CAP1 Polarity rising edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 0;           //  CAP2 Polarity rising edge
    ECap1Regs.ECCTL1.bit.CAP3POL = 0;           //  CAP3 Polarity rising edge
    ECap1Regs.ECCTL1.bit.CAP4POL = 0;           //  CAP4 Polarity rising edge

    ECap1Regs.ECCTL1.bit.CTRRST1 = 1;           //  Reset counter After capture event1(Difference operation)
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;           //  Reset counter After capture event2(Difference operation)
    ECap1Regs.ECCTL1.bit.CTRRST3 = 1;           //  Reset counter After capture event3(Difference operation)
    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;           //  Reset counter After capture event4(Difference operation)
    ECap1Regs.ECCTL1.bit.PRESCALE = 0;          //  No Prescale divide by 1 (150M/1)

    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;          //  Disable sync in
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 2;         //  Disable sync out

    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;         //  Start Counter
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1;           //  Eisable CAP1-4 Register Load at capture event time
#endif
}

//
// InitECapGpio - This function initializes GPIO pins to function as ECAP pins
//
// Each GPIO pin can be configured as a GPIO pin or up to 3 different
// peripheral functional pins. By default all pins come up as GPIO
// inputs after reset.
//
// Caution:
// For each eCAP peripheral
// Only one GPIO pin should be enabled for ECAP operation.
// Comment out other unwanted lines.
//
void 
InitECapGpio()
{
    InitECap1Gpio();
#if (DSP28_ECAP2)
    InitECap2Gpio();
#endif // endif DSP28_ECAP2
#if (DSP28_ECAP3)
    InitECap3Gpio();
#endif // endif DSP28_ECAP3
#if (DSP28_ECAP4)
    InitECap4Gpio();
#endif // endif DSP28_ECAP4
#if (DSP28_ECAP5)
    InitECap5Gpio();
#endif // endif DSP28_ECAP5
#if (DSP28_ECAP6)
    InitECap6Gpio();
#endif // endif DSP28_ECAP6
}

//
// InitECap1Gpio - 
//
void 
InitECap1Gpio(void)
{
    EALLOW;
    
    //
    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    //GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;    // Enable pull-up on GPIO5 (CAP1)
    GpioCtrlRegs.GPAPUD.bit.GPIO24 = 0;     // Enable pull-up on GPIO24 (CAP1)
    //GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;   // Enable pull-up on GPIO34 (CAP1)

    //
    // Inputs are synchronized to SYSCLKOUT by default.
    // Comment out other unwanted lines.
    //
    //GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 0;   //Synch to SYSCLKOUT GPIO5 (CAP1)
    GpioCtrlRegs.GPAQSEL2.bit.GPIO24 = 0;   //Synch to SYSCLKOUT GPIO24 (CAP1)
    //GpioCtrlRegs.GPBQSEL1.bit.GPIO34 = 0;  //Synch to SYSCLKOUT GPIO34 (CAP1)

    //
    // Configure eCAP-1 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be eCAP1 functional pins.
    // Comment out other unwanted lines.
    //
    //GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 3;    // Configure GPIO5 as CAP1
    GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 1;    // Configure GPIO24 as CAP1
    //GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 1;   // Configure GPIO24 as CAP1

    EDIS;
}

#if DSP28_ECAP2
//
// InitECap2Gpio -
//
void 
InitECap2Gpio(void)
{
    EALLOW;
    
    //
    // Enable internal pull-up for the selected pins
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;     // Enable pull-up on GPIO7 (CAP2)
    //GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;    // Enable pull-up on GPIO25 (CAP2)
    //GpioCtrlRegs.GPBPUD.bit.GPIO37 = 0;    // Enable pull-up on GPIO37 (CAP2)

    //
    // Inputs are synchronized to SYSCLKOUT by default.
    // Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 0;    //Synch to SYSCLKOUT GPIO7 (CAP2)
    //GpioCtrlRegs.GPAQSEL2.bit.GPIO25 = 0;  //Synch to SYSCLKOUT GPIO25 (CAP2)
    //GpioCtrlRegs.GPBQSEL1.bit.GPIO37 = 0;  //Synch to SYSCLKOUT GPIO37 (CAP2)

    //
    // Configure eCAP-2 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be eCAP2 functional 
    // pins. Comment out other unwanted lines.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 3;    // Configure GPIO7 as CAP2
    //GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;   // Configure GPIO25 as CAP2
    //GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 3;   // Configure GPIO37 as CAP2

    EDIS;
}
#endif // endif DSP28_ECAP2

#if DSP28_ECAP3
//
// InitECap3Gpio - 
//
void 
InitECap3Gpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO9 = 0;      // Enable pull-up on GPIO9 (CAP3)
// GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     // Enable pull-up on GPIO26 (CAP3)

// Inputs are synchronized to SYSCLKOUT by default.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO9 = 0;    // Synch to SYSCLKOUT GPIO9 (CAP3)
// GpioCtrlRegs.GPAQSEL2.bit.GPIO26 = 0;   // Synch to SYSCLKOUT GPIO26 (CAP3)

/* Configure eCAP-3 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAP3 functional pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 3;     // Configure GPIO9 as CAP3
// GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 1;    // Configure GPIO26 as CAP3

    EDIS;
}
#endif // endif DSP28_ECAP3


#if DSP28_ECAP4
void InitECap4Gpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;   // Enable pull-up on GPIO11 (CAP4)
// GpioCtrlRegs.GPAPUD.bit.GPIO27 = 0;   // Enable pull-up on GPIO27 (CAP4)

// Inputs are synchronized to SYSCLKOUT by default.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 0; // Synch to SYSCLKOUT GPIO11 (CAP4)
// GpioCtrlRegs.GPAQSEL2.bit.GPIO27 = 0; // Synch to SYSCLKOUT GPIO27 (CAP4)

/* Configure eCAP-4 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAP4 functional pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 3;  // Configure GPIO11 as CAP4
// GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 1;  // Configure GPIO27 as CAP4

    EDIS;
}
#endif // endif DSP28_ECAP4


#if DSP28_ECAP5
void InitECap5Gpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;     // Enable pull-up on GPIO3 (CAP5)
// GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;    // Enable pull-up on GPIO48 (CAP5)

// Inputs are synchronized to SYSCLKOUT by default.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 0;  // Synch to SYSCLKOUT GPIO3 (CAP5)
// GpioCtrlRegs.GPBQSEL2.bit.GPIO48 = 0; // Synch to SYSCLKOUT GPIO48 (CAP5)

/* Configure eCAP-5 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAP5 functional pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 2;   // Configure GPIO3 as CAP5
// GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 1;  // Configure GPIO48 as CAP5

    EDIS;
}
#endif // endif DSP28_ECAP5


#if DSP28_ECAP6
void InitECap6Gpio(void)
{
   EALLOW;

/* Enable internal pull-up for the selected pins */
// Pull-ups can be enabled or disabled by the user.
// This will enable the pullups for the specified pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;     // Enable pull-up on GPIO1 (CAP6)
// GpioCtrlRegs.GPBPUD.bit.GPIO49 = 0;    // Enable pull-up on GPIO49 (CAP6)

// Inputs are synchronized to SYSCLKOUT by default.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 0;  // Synch to SYSCLKOUT GPIO1 (CAP6)
// GpioCtrlRegs.GPBQSEL2.bit.GPIO49 = 0; // Synch to SYSCLKOUT GPIO49 (CAP6)

/* Configure eCAP-5 pins using GPIO regs*/
// This specifies which of the possible GPIO pins will be eCAP6 functional pins.
// Comment out other unwanted lines.

   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 2;   // Configure GPIO1 as CAP6
// GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 1;  // Configure GPIO49 as CAP6

    EDIS;
}
#endif // endif DSP28_ECAP6



//===========================================================================
// End of file.
//===========================================================================
