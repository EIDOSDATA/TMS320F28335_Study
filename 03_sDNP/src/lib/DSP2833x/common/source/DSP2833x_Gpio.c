// TI File $Revision: /main/1 $
// Checkin $Date: August 18, 2006   13:46:25 $
//###########################################################################
//
// FILE:	DSP2833x_Gpio.c
//
// TITLE:	DSP2833x General Purpose I/O Initialization & Support Functions.
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
// InitGpio - This function initializes the Gpio to a known (default) state.
//
// For more details on configuring GPIO's as peripheral functions,
// refer to the individual peripheral examples and/or GPIO setup example. 
//
void 
InitGpio(void)
{
    //  InitGpioData();

        EALLOW;

       //pullup Register Enable(0) or Disable(1)
       // Pull-ups can be enabled or disabled.
    /*****************************************************************
      MUX : 0 input/output   1, 2, 3 ±â´É  defalt 0
        CTRL : Qualification control        defalt 0   0 ~ 255
        QSEL ; select input qualifiacation  defalt 0   0:SYNCH OUT  1:3 SAMPLES  2:6 SAMPLES  3: ASYNCH OUT
        DIR  : 0 input  1 output            defalt 0
        SET  : 0 ignored   1 output data latch to high
        CLEAR  : 0 ignored   1 output data latch to LOW
        TOGGLE :  0 ignored   1 output data latch to TOGGLE from its current state

        PUD  : 0 Pullup register enabled, 0 disabled.

    *****************************************************************/
       //************* OUT PUT ************************
       //CPU RUN = 0, 1  // 35, 48
       //DO = 2(8), 3(7), // 20(1), 21(2), 22(3), 23(4), 24(5), 25(6)
       //DO_CP = 4
       //DO_OE = 5

       //MEM = 6, 7, 8, 9
       //AD = 10, 11
       //RTC_RST 34
       //MMI_DIR 36
       //RS232 = 57(DTR), 58(RTS)
       //************* IN PUT ************************
       //RS232 = 59(DCD), 60(DSR), 61(CTS)
       //DI    = 49(8), 50(7), 51(6), 52(5), 53(4), 54(3), 55(2), 56(1)

       // VA PW1
        GpioCtrlRegs.GPAPUD.bit.GPIO1   = 0;     // Enable pull-up on GPIO1 (CAP6)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 0;  // Synch to SYSCLKOUT GPIO1 (CAP6)
        GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 2;   // Configure GPIO1 as CAP6

        //VA PW2
        GpioCtrlRegs.GPAPUD.bit.GPIO3   = 0;     // Enable pull-up on GPIO3 (CAP5)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO3 = 0;  // Synch to SYSCLKOUT GPIO3 (CAP5)
        GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 2;   // Configure GPIO3 as CAP5

        //VA PW3
        GpioCtrlRegs.GPAPUD.bit.GPIO5   = 0;      // Enable pull-up on GPIO5 (CAP1)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO5 = 0;    // Synch to SYSCLKOUT GPIO5 (CAP1)
        GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 3;     // Configure GPIO5 as CAP1

        //VA PW4
        GpioCtrlRegs.GPAPUD.bit.GPIO7   = 0;     // Enable pull-up on GPIO7 (CAP2)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 0;    // Synch to SYSCLKOUT GPIO7 (CAP2)
        GpioCtrlRegs.GPAMUX1.bit.GPIO7  = 3;    // Configure GPIO7 as CAP2

        //VA PW5
        GpioCtrlRegs.GPAPUD.bit.GPIO9   = 0;      // Enable pull-up on GPIO9 (CAP3)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO9 = 0;    // Synch to SYSCLKOUT GPIO9 (CAP3)
        GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 3;     // Configure GPIO9 as CAP3

        //VA PW6
        GpioCtrlRegs.GPAPUD.bit.GPIO11   = 0;   // Enable pull-up on GPIO11 (CAP4)
        GpioCtrlRegs.GPAQSEL1.bit.GPIO11 = 0; // Synch to SYSCLKOUT GPIO11 (CAP4)
        GpioCtrlRegs.GPAMUX1.bit.GPIO11  = 3;  // Configure GPIO11 as CAP4


        //DIGITAL INPUT
        GpioCtrlRegs.GPADIR.bit.GPIO12    = 0;  // input TXRDYA


        GpioCtrlRegs.GPADIR.bit.GPIO16    = 0;  // input CTS2 DNP PORT
        GpioCtrlRegs.GPADIR.bit.GPIO17    = 0;  // input LAN CS



        //DIGITAL OUTPUT
        GpioCtrlRegs.GPADIR.bit.GPIO13    = 1;  // output RTS2 DNP PORT

        GpioCtrlRegs.GPAPUD.bit.GPIO18   = 1;  // Enable pullup
        GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO18  = 0;  // IO
        GpioCtrlRegs.GPADIR.bit.GPIO18   = 1;  // output

        GpioCtrlRegs.GPAPUD.bit.GPIO19   = 1;  // Enable pullup
        GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO19  = 0;  // IO
        GpioCtrlRegs.GPADIR.bit.GPIO19   = 1;  // output

        GpioCtrlRegs.GPAPUD.bit.GPIO20   = 1;  // Enable pullup
        GpioDataRegs.GPACLEAR.bit.GPIO20 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO20  = 0;  // IO
        GpioCtrlRegs.GPADIR.bit.GPIO20   = 1;  // output

        //485 DE3
        GpioCtrlRegs.GPAPUD.bit.GPIO21   = 1;  // Enable pullup
        GpioDataRegs.GPACLEAR.bit.GPIO21 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO21  = 0;  // IO
        GpioCtrlRegs.GPADIR.bit.GPIO21   = 1;  // output

        //UART RST  HIGH ACTIVE
        GpioCtrlRegs.GPAPUD.bit.GPIO25   = 1;  // Enable pullup
    //  GpioDataRegs.GPACLEAR.bit.GPIO25 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO25  = 0;  // IO
    //  GpioCtrlRegs.GPADIR.bit.GPIO25   = 1;  // output

        //MOD RTS
        GpioCtrlRegs.GPAPUD.bit.GPIO26   = 1;  // Enable pullup
    //  GpioDataRegs.GPACLEAR.bit.GPIO26 = 1;  // Load output latch
        GpioCtrlRegs.GPAMUX2.bit.GPIO26  = 0;  // IO
    //  GpioCtrlRegs.GPADIR.bit.GPIO26   = 1;  // output



        //CPU RUN 1 2 3
        GpioCtrlRegs.GPBPUD.bit.GPIO48  = 1;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO48  = 1;  // output

        //cpu do en
        GpioCtrlRegs.GPBPUD.bit.GPIO49  = 1;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO49  = 1;  // output
        GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;  // Load output latch

        //MEM_CS_EN
        GpioCtrlRegs.GPBPUD.bit.GPIO50  = 1;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO50  = 1;  // output
        GpioDataRegs.GPBSET.bit.GPIO50  = 1;

        //MEM_CS2
        GpioCtrlRegs.GPBPUD.bit.GPIO51  = 1;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO51  = 1;  // output

        //MEM_CS1
        GpioCtrlRegs.GPBPUD.bit.GPIO52   = 0;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO52   = 1;  // output

        //MEM_CS0
        GpioCtrlRegs.GPBPUD.bit.GPIO53  = 1;  // Enable pullup
        GpioCtrlRegs.GPBDIR.bit.GPIO53  = 1;  // output

        //CPU RUN 1 2 3
        GpioCtrlRegs.GPBDIR.bit.GPIO54    = 1;  // output 5
        GpioCtrlRegs.GPBDIR.bit.GPIO55    = 1;  // output 4
        GpioCtrlRegs.GPBDIR.bit.GPIO59    = 1;  // output LAN INIT
        GpioCtrlRegs.GPBDIR.bit.GPIO60    = 0;  // ipput  debug CTS
        GpioCtrlRegs.GPBDIR.bit.GPIO61    = 1;  // output debug RTS


    ///////////////////////////////////
        //SCI => RS232  GPIO 22, 23(B, 485)  62 63(C, usb)
        GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up for GPIO14 (SCITXDB)
        GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up for GPIO15 (SCIRXDB)

        GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO15 (SCIRXDB)

        GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 2;   // Configure GPIO14 for SCITXDB operation
        GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 2;   // Configure GPIO15 for SCIRXDB operation

        //SCI => RS232  GPIO 62, 63
        GpioCtrlRegs.GPBPUD.bit.GPIO62 = 0;    // Enable pull-up for GPIO62 (SCIRXDC)
        GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;    // Enable pull-up for GPIO63 (SCITXDC)

        /* Set qualification for selected pins to asynch only */
        // Inputs are synchronized to SYSCLKOUT by default.
        // This will select asynch (no qualification) for the selected pins.

        GpioCtrlRegs.GPBQSEL2.bit.GPIO62 = 3;  // Asynch input GPIO62 (SCIRXDC)

        /* Configure SCI-C pins using GPIO regs*/
        // This specifies which of the possible GPIO pins will be SCI functional pins.

        GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;   // Configure GPIO62 for SCIRXDC operation
        GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;   // Configure GPIO63 for SCITXDC operation


        //RTC I2C
    //  GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
    //  GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;    // Enable pull-up for GPIO33 (SCLA)

        /* Set qualification for selected pins to asynch only */
        // This will select asynch (no qualification) for the selected pins.
        // Comment out other unwanted lines.

    //  GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    //    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)

        /* Configure SCI pins using GPIO regs*/
        // This specifies which of the possible GPIO pins will be I2C functional pins.
        // Comment out other unwanted lines.

    //  GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO32 for SDAA operation
    //  GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO33 for SCLA operation

        //RTC RST
        GpioDataRegs.GPBSET.bit.GPIO58    = 1;  // Load output latch
        GpioCtrlRegs.GPBMUX2.bit.GPIO58   = 0;  // IO
        GpioCtrlRegs.GPBDIR.bit.GPIO58    = 1;  // output

            //RTC CLK
        GpioDataRegs.GPBSET.bit.GPIO57    = 1;  // Load output latch
        GpioCtrlRegs.GPBMUX2.bit.GPIO57   = 0;  // IO
        GpioCtrlRegs.GPBDIR.bit.GPIO57    = 1;  // output

            //RTC IO
        //GpioCtrlRegs.GPBPUD.bit.GPIO34  = 1;  // Enable pullup
        GpioDataRegs.GPBSET.bit.GPIO56    = 1;  // Load output latch
        GpioCtrlRegs.GPBMUX2.bit.GPIO56   = 0;  // IO
        GpioCtrlRegs.GPBDIR.bit.GPIO56    = 1;  // output

       EDIS;

    //   InitGpioData();

}

void LED_Toggle(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO48 = 1;
}
	
//
// End of file
//

