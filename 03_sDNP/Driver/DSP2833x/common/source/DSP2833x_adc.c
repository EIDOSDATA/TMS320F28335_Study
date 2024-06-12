// TI File $Revision: /main/5 $
// Checkin $Date: October 23, 2007   13:34:09 $
//###########################################################################
//
// FILE:	DSP2833x_Adc.c
//
// TITLE:	DSP2833x ADC Initialization & Support Functions.
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
// Defines
//
#define ADC_usDELAY  5000L

//
// InitAdc - This function initializes ADC to a known state.
//
void 
InitAdc(void)
{
    extern void DSP28x_usDelay(Uint32 Count);

    //
    //                          *IMPORTANT*
    // The ADC_cal function, which  copies the ADC calibration values from 
    // TI reserved OTP into the ADCREFSEL and ADCOFFTRIM registers, occurs 
    // automatically in the Boot ROM. If the boot ROM code is bypassed during
    // the debug process, the following function MUST be called for the ADC to
    // function according to specification. The clocks to the ADC MUST be 
    // enabled before calling this function. See the device data manual and/or
    // the ADC Reference Manual for more information.
    //
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1;
    ADC_cal();
    EDIS;
    // To powerup the ADC the ADCENCLK bit should be set first to enable
    // clocks, followed by powering up the bandgap, reference circuitry, and ADC core.
    // Before the first conversion is performed a 5ms delay must be observed
    // after power up to give all analog circuits time to power up and settle

    AdcRegs.ADCTRL3.bit.ADCBGRFDN = 0x3;    // Power up bandgap/reference circuitry

    AdcRegs.ADCTRL3.bit.ADCPWDN = 1;        // Power up rest of ADC

    // Please note that for the delay function below to operate correctly the
    // CPU_RATE define statement in the DSP2833x_Examples.h file must
    // contain the correct CPU clock period in nanoseconds.

    DELAY_US(ADC_usDELAY);                 // Delay after powering up ADC, 20us

    /*Specific ADC Setup for SPEC320*/

    AdcRegs.ADCREFSEL.bit.REF_SEL = 1;

    AdcRegs.ADCTRL3.bit.ADCCLKPS = 1;       // ADCCLKPS = HSPCLK(25) / 2 (ADCCLKPS = 12.5MHz)

    AdcRegs.ADCTRL1.bit.CPS = 1;            // Start 1->1805 2->970      // ADCCLK = ADCCLKPS / 2(ADC의 구동 클럭을 결정 : ADCCLK = 2.5MHz)
    AdcRegs.ADCTRL1.bit.ACQ_PS = 2;         // 샘플/홀드 시간 설정 : ACQ_PS + 1 = 3 사이클
    AdcRegs.ADCTRL1.bit.SEQ_CASC = 1;       // Cascaded mode


    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1; // EVASOC로 ADC 시퀀스1 시동
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;   // ADC 시퀀스1이 끝나면 인터럽트 요청

    // Configure ADC
    AdcRegs.ADCMAXCONV.all = 0x000F;        // Setup 2 conv's on SEQ1

    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0;  // Setup ADCINA0 as 1st SEQ1 conv.

    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3;  // Setup ADCINA1 as 2nd SEQ1 conv.

    AdcRegs.ADCCHSELSEQ2.bit.CONV04 = 0x4;  // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV05 = 0x5;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV06 = 0x6;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ2.bit.CONV07 = 0x7;  // Setup ADCINA1 as 2nd SEQ1 conv.

    AdcRegs.ADCCHSELSEQ3.bit.CONV08 = 0x8;  // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ3.bit.CONV09 = 0x9;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ3.bit.CONV10 = 0xA;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ3.bit.CONV11 = 0xB;  // Setup ADCINA1 as 2nd SEQ1 conv.

    AdcRegs.ADCCHSELSEQ4.bit.CONV12 = 0xC;  // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ4.bit.CONV13 = 0xD;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ4.bit.CONV14 = 0xE;  // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ4.bit.CONV15 = 0xF;  // Setup ADCINA1 as 2nd SEQ1 conv.

}
void
ADC_SWTrigger(void)
{
    AdcRegs.ADCTRL2.bit.SOC_SEQ1 = 1;
}
void
ADC_Interrupt_Clear(void)
{
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;
}
//
// End of file
//

