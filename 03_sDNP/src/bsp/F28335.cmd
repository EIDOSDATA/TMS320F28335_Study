/*
// TI File $Revision: /main/10 $
// Checkin $Date: July 9, 2008   13:43:56 $
//###########################################################################
//
// FILE:	F28335.cmd
//
// TITLE:	Linker Command File For F28335 Device
//
//###########################################################################
// $TI Release: $
// $Release Date: $
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
*/

/* ======================================================
// For Code Composer Studio V2.2 and later
// ---------------------------------------
// In addition to this memory linker command file, 
// add the header linker command file directly to the project. 
// The header linker command file is required to link the
// peripheral structures to the proper locations within 
// the memory map.
//
// The header linker files are found in <base>\headers\cmd
//   
// For BIOS applications add:      DSP2833x_Headers_BIOS.cmd
// For nonBIOS applications add:   DSP2833x_Headers_nonBIOS.cmd    
========================================================= */

/* ======================================================
// For Code Composer Studio prior to V2.2
// --------------------------------------
// 1) Use one of the following -l statements to include the 
// header linker command file in the project. The header linker
// file is required to link the peripheral structures to the proper 
// locations within the memory map                                    */

/* Uncomment this line to include file only for non-BIOS applications */
/* -l DSP2833x_Headers_nonBIOS.cmd */

/* Uncomment this line to include file only for BIOS applications */
/* -l DSP2833x_Headers_BIOS.cmd */

/* 2) In your project add the path to <base>\headers\cmd to the
   library search path under project->build options, linker tab, 
   library search path (-i).
/*========================================================= */

/* Define the memory block start/length for the F28335  
   PAGE 0 will be used to organize program sections
   PAGE 1 will be used to organize data sections

    Notes: 
          Memory blocks on F28335 are uniform (ie same
          physical memory) in both PAGE 0 and PAGE 1.  
          That is the same memory region should not be
          defined for both PAGE 0 and PAGE 1.
          Doing so will result in corruption of program 
          and/or data. 
          
          L0/L1/L2 and L3 memory blocks are mirrored - that is
          they can be accessed in high memory or low memory.
          For simplicity only one instance is used in this
          linker file. 
          
          Contiguous SARAM memory blocks can be combined 
          if required to create a larger memory block. 
 */


MEMORY
{
PAGE 0:    /* Program Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE1 for data allocation */

   /*RAM Funcation RUN Block*/
   RAMM0_1     : origin = 0x000050, length = 0x0007B0     /* on-chip RAM block M0, M1 */

   ENTRY	   : origin = 0x300000, length = 0x000070	  /* on-chip FLASH for _c_int00(entry point)*/
   FW_VERSION  : origin = 0x300070, length = 0x000010	  /* Firmware version */

   FLASH_H     : origin = 0x300080, length = 0x000F80	  /* on-chip FLASH for ramfunction, parameter ?*/
   FLASH_FG    : origin = 0x301000, length = 0x00F000     /* on-chip FLASH for .econst section */
   FLASH_BE    : origin = 0x310000, length = 0x027FFF     /* on-chip FLASH for .text section   */

   OTP         : origin = 0x380400, length = 0x000400     /* on-chip OTP */
   ADC_CAL     : origin = 0x380080, length = 0x000009     /* ADC_cal function in Reserved memory */


PAGE 1 :   /* Data Memory */
           /* Memory (RAM/FLASH/OTP) blocks can be moved to PAGE0 for program allocation */
           /* Registers remain on PAGE1                                                  */

   BOOT_RSVD   : origin = 0x000000, length = 0x000050     /* Part of M0, BOOT rom will use this for stack */
   PIEVECT     : origin = 0x000D00, length = 0x100
   RAML0	   : origin = 0x008000, length = 0x000A00	  /* on-chip RAM block L0 for system stack*/
   RAML1_7     : origin = 0x008A00, length = 0x007480     /* on-chip RAM block L1, L2, L3, L4, L5, L6, L7*/
   DATA_RAML7  : origin = 0x00FE80, length = 0x000100
   DMA_RAML7   : origin = 0x00FF80,	length = 0x000080	  /* DMA - access block */
   ZONE6SRAM   : origin = 0x100000, length = 0x040000     /* XINTF zone 6 */
}

/* Allocate sections to memory blocks.
   Note:
         codestart user defined section in DSP28_CodeStartBranch.asm used to redirect code
                   execution when booting to flash
         ramfuncs  user defined section to store functions that will be copied from Flash into RAM
*/

SECTIONS
{

   /* Allocate program areas: */
   .cinit              : > FLASH_BE      	PAGE = 0
   .pinit			   : > FLASH_BE	   		PAGE = 0
   .text               : > FLASH_BE      	PAGE = 0
   .cio                : > RAMM0_1     		PAGE = 0

	FW_Ver			   : > FW_VERSION		PAGE = 0

	boot 			   : > ENTRY			PAGE = 0
	{
		-l "C:\ti\bios_6_83_00_18\packages\ti\targets\rts2800\lib\boot.a28FP"<boot_cg.o28FP>(.text)
    }



#ifdef __TI_COMPILER_VERSION__
    #if __TI_COMPILER_VERSION__ >= 15009000
        .TI.ramfunc : {} LOAD 	= FLASH_H,
                         RUN 	= RAMM0_1,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         PAGE 	= 0

    #else
        ramfuncs       : LOAD 	= FLASH_H,
                         RUN 	= RAMM0_1,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         PAGE 	= 0

    #endif
#endif

   /* Allocate uninitalized data sections: */
   .stack              : > RAML0       				PAGE = 1
   .ebss               : > RAML1_7       			PAGE = 1
   .data			   : > DATA_RAML7         		PAGE = 1

   /* Initalized sections go in Flash */
   /* For SDFlash to program these, they must be allocated to page 0 */

   .econst             : > FLASH_FG      			PAGE = 0
   .switch             : > FLASH_FG        		    PAGE = 0

   /* Uncomment the section below if calling the IQNexp() or IQexp()
      functions from the IQMath.lib library in order to utilize the
      relevant IQ Math table in Boot ROM (This saves space and Boot ROM
      is 1 wait-state). If this section is not uncommented, IQmathTables2
      will be loaded into other memory (SARAM, Flash, etc.) and will take
      up space, but 0 wait-state is possible.
   */


   /* Allocate DMA-accessible RAM sections: */

   DMARAML7         : > DMA_RAML7,     	PAGE = 1

   /* Allocate of XINTF Zone 6 to storing data */

   ZONE6DATA	    : > ZONE6SRAM,	   		PAGE = 1
   DNP_Stack		: > ZONE6SRAM,	   		PAGE = 1

   /* .reset is a standard section used by the compiler.  It contains the */
   /* the address of the start of _c_int00 for C Code.   /*
   /* When using the boot ROM this section and the CPU vector */
   /* table is not needed.  Thus the default type is set here to  */
   /* DSECT  */

   /* Allocate ADC_cal function (pre-programmed by factory into TI reserved memory) */
   .adc_cal     : load = ADC_CAL,   PAGE = 0, TYPE = NOLOAD

}

/*
//===========================================================================
// End of file.
//===========================================================================
*/

