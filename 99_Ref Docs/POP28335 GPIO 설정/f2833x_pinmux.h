//*****************************************************************************
//
//  f2833x_pinmux.h - Created using TI Pinmux 1.20.0+3587 on 2024. 6. 8. at 오후 12:00:26.
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
//  These values will provide the functionality requested when written into
//  the registers for which the #defines are named.  For example, using the
//  C2000Ware device support header files, use the defines like in this
//  sample function:
//
//  void samplePinMuxFxn(void)
//  {
//      EALLOW;
//      //
//      // Write generated values to mux registers
//      //
//      GpioCtrlRegs.GPAMUX1.all  = GPAMUX1_VALUE;
//      GpioCtrlRegs.GPAMUX2.all  = GPAMUX2_VALUE;
//      GpioCtrlRegs.GPBMUX1.all  = GPBMUX1_VALUE;
//        . . .
//      EDIS;
//  }
//
//*****************************************************************************

//
// Port A mux register values
//
// Pin 7 (GPIO2) to GPIO2 (mode 0)
// Pin 10 (GPIO3) to GPIO3 (mode 0)
// Pin 11 (GPIO4) to GPIO4 (mode 0)
// Pin 5 (GPIO0) to GPIO0 (mode 0)
// Pin 13 (GPIO6) to GPIO6 (mode 0)
// Pin 16 (GPIO7) to GPIO7 (mode 0)
#define GPAMUX1_MASK		0x0000f3f3
#define GPAMUX2_MASK		0x00000000
#define GPAMUX1_VALUE		0x00000000
#define GPAMUX2_VALUE		0x00000000

//
// Port B mux register values
//
// Pin 111 (GPIO60) to GPIO60 (mode 0)
// Pin 161 (GPIO46) to GPIO46 (mode 0)
// Pin 96 (GPIO54) to GPIO54 (mode 0)
// Pin 98 (GPIO56) to GPIO56 (mode 0)
#define GPBMUX1_MASK		0x30000000
#define GPBMUX2_MASK		0x03033000
#define GPBMUX1_VALUE		0x00000000
#define GPBMUX2_VALUE		0x00000000

//
// Port C mux register values
//
#define GPCMUX1_MASK		0x00000000
#define GPCMUX2_MASK		0x00000000
#define GPCMUX1_VALUE		0x00000000
#define GPCMUX2_VALUE		0x00000000

//*****************************************************************************
//
// Function prototype for function to write values above into their
// corresponding registers. This function is found in f2833x_pinmux.c. Its use
// is completely optional.
//
//*****************************************************************************
extern void GPIO_setPinMuxConfig(void);
