//*****************************************************************************
//
// f2833x_pinmux.c - Function to write the generated pin mux values to the
//                   appropriate registers.
// Created using TI Pinmux 1.20.0+3587 on 2024. 6. 8. at 오후 12:00:26.
//
//*****************************************************************************
//
// Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
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
//
//*****************************************************************************
//
// NOTE!! This file uses bit field structures and macros from C2000Ware.
// This function is provided for your convenience and to serve as an example
// of the use of the generated header file, but its use is not required.
//
// To download C2000Ware:  http://www.ti.com/tool/C2000Ware
//
//*****************************************************************************

#include "DSP28x_Project.h"
#include "f2833x_pinmux.h"

//*****************************************************************************
//
// Configures the pin mux registers, using the generated register values.
//
// This function writes the values generated by the pin mux tool to their
// corresponding GPIO control registers. These generated values should be found
// in the generated "f2833x_pinmux.h."
//
//*****************************************************************************
void
GPIO_setPinMuxConfig(void)
{
    EALLOW;

    //
    // Clear the mux register fields that are about to be changed
    //
    GpioCtrlRegs.GPAMUX1.all	&= ~GPAMUX1_MASK;
    GpioCtrlRegs.GPAMUX2.all	&= ~GPAMUX2_MASK;
    GpioCtrlRegs.GPBMUX1.all	&= ~GPBMUX1_MASK;
    GpioCtrlRegs.GPBMUX2.all	&= ~GPBMUX2_MASK;
    GpioCtrlRegs.GPCMUX1.all	&= ~GPCMUX1_MASK;
    GpioCtrlRegs.GPCMUX2.all	&= ~GPCMUX2_MASK;

    //
    // Write pin muxing to mux registers
    //
    GpioCtrlRegs.GPAMUX1.all	|=  GPAMUX1_VALUE;
    GpioCtrlRegs.GPAMUX2.all	|=  GPAMUX2_VALUE;
    GpioCtrlRegs.GPBMUX1.all	|=  GPBMUX1_VALUE;
    GpioCtrlRegs.GPBMUX2.all	|=  GPBMUX2_VALUE;
    GpioCtrlRegs.GPCMUX1.all	|=  GPCMUX1_VALUE;
    GpioCtrlRegs.GPCMUX2.all	|=  GPCMUX2_VALUE;

    EDIS;
}
