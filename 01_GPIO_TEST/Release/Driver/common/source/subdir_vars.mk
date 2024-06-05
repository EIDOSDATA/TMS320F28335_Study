################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../Driver/common/source/DSP2833x_ADC_cal.asm \
../Driver/common/source/DSP2833x_CSMPasswords.asm \
../Driver/common/source/DSP2833x_CodeStartBranch.asm \
../Driver/common/source/DSP2833x_DBGIER.asm \
../Driver/common/source/DSP2833x_DisInt.asm \
../Driver/common/source/DSP2833x_usDelay.asm 

C_SRCS += \
../Driver/common/source/DSP2833x_Adc.c \
../Driver/common/source/DSP2833x_CpuTimers.c \
../Driver/common/source/DSP2833x_DMA.c \
../Driver/common/source/DSP2833x_DefaultIsr.c \
../Driver/common/source/DSP2833x_ECan.c \
../Driver/common/source/DSP2833x_ECap.c \
../Driver/common/source/DSP2833x_EPwm.c \
../Driver/common/source/DSP2833x_EQep.c \
../Driver/common/source/DSP2833x_Gpio.c \
../Driver/common/source/DSP2833x_I2C.c \
../Driver/common/source/DSP2833x_Mcbsp.c \
../Driver/common/source/DSP2833x_MemCopy.c \
../Driver/common/source/DSP2833x_PieCtrl.c \
../Driver/common/source/DSP2833x_PieVect.c \
../Driver/common/source/DSP2833x_Sci.c \
../Driver/common/source/DSP2833x_Spi.c \
../Driver/common/source/DSP2833x_SysCtrl.c \
../Driver/common/source/DSP2833x_Xintf.c 

C_DEPS += \
./Driver/common/source/DSP2833x_Adc.d \
./Driver/common/source/DSP2833x_CpuTimers.d \
./Driver/common/source/DSP2833x_DMA.d \
./Driver/common/source/DSP2833x_DefaultIsr.d \
./Driver/common/source/DSP2833x_ECan.d \
./Driver/common/source/DSP2833x_ECap.d \
./Driver/common/source/DSP2833x_EPwm.d \
./Driver/common/source/DSP2833x_EQep.d \
./Driver/common/source/DSP2833x_Gpio.d \
./Driver/common/source/DSP2833x_I2C.d \
./Driver/common/source/DSP2833x_Mcbsp.d \
./Driver/common/source/DSP2833x_MemCopy.d \
./Driver/common/source/DSP2833x_PieCtrl.d \
./Driver/common/source/DSP2833x_PieVect.d \
./Driver/common/source/DSP2833x_Sci.d \
./Driver/common/source/DSP2833x_Spi.d \
./Driver/common/source/DSP2833x_SysCtrl.d \
./Driver/common/source/DSP2833x_Xintf.d 

OBJS += \
./Driver/common/source/DSP2833x_ADC_cal.obj \
./Driver/common/source/DSP2833x_Adc.obj \
./Driver/common/source/DSP2833x_CSMPasswords.obj \
./Driver/common/source/DSP2833x_CodeStartBranch.obj \
./Driver/common/source/DSP2833x_CpuTimers.obj \
./Driver/common/source/DSP2833x_DBGIER.obj \
./Driver/common/source/DSP2833x_DMA.obj \
./Driver/common/source/DSP2833x_DefaultIsr.obj \
./Driver/common/source/DSP2833x_DisInt.obj \
./Driver/common/source/DSP2833x_ECan.obj \
./Driver/common/source/DSP2833x_ECap.obj \
./Driver/common/source/DSP2833x_EPwm.obj \
./Driver/common/source/DSP2833x_EQep.obj \
./Driver/common/source/DSP2833x_Gpio.obj \
./Driver/common/source/DSP2833x_I2C.obj \
./Driver/common/source/DSP2833x_Mcbsp.obj \
./Driver/common/source/DSP2833x_MemCopy.obj \
./Driver/common/source/DSP2833x_PieCtrl.obj \
./Driver/common/source/DSP2833x_PieVect.obj \
./Driver/common/source/DSP2833x_Sci.obj \
./Driver/common/source/DSP2833x_Spi.obj \
./Driver/common/source/DSP2833x_SysCtrl.obj \
./Driver/common/source/DSP2833x_Xintf.obj \
./Driver/common/source/DSP2833x_usDelay.obj 

ASM_DEPS += \
./Driver/common/source/DSP2833x_ADC_cal.d \
./Driver/common/source/DSP2833x_CSMPasswords.d \
./Driver/common/source/DSP2833x_CodeStartBranch.d \
./Driver/common/source/DSP2833x_DBGIER.d \
./Driver/common/source/DSP2833x_DisInt.d \
./Driver/common/source/DSP2833x_usDelay.d 

OBJS__QUOTED += \
"Driver\common\source\DSP2833x_ADC_cal.obj" \
"Driver\common\source\DSP2833x_Adc.obj" \
"Driver\common\source\DSP2833x_CSMPasswords.obj" \
"Driver\common\source\DSP2833x_CodeStartBranch.obj" \
"Driver\common\source\DSP2833x_CpuTimers.obj" \
"Driver\common\source\DSP2833x_DBGIER.obj" \
"Driver\common\source\DSP2833x_DMA.obj" \
"Driver\common\source\DSP2833x_DefaultIsr.obj" \
"Driver\common\source\DSP2833x_DisInt.obj" \
"Driver\common\source\DSP2833x_ECan.obj" \
"Driver\common\source\DSP2833x_ECap.obj" \
"Driver\common\source\DSP2833x_EPwm.obj" \
"Driver\common\source\DSP2833x_EQep.obj" \
"Driver\common\source\DSP2833x_Gpio.obj" \
"Driver\common\source\DSP2833x_I2C.obj" \
"Driver\common\source\DSP2833x_Mcbsp.obj" \
"Driver\common\source\DSP2833x_MemCopy.obj" \
"Driver\common\source\DSP2833x_PieCtrl.obj" \
"Driver\common\source\DSP2833x_PieVect.obj" \
"Driver\common\source\DSP2833x_Sci.obj" \
"Driver\common\source\DSP2833x_Spi.obj" \
"Driver\common\source\DSP2833x_SysCtrl.obj" \
"Driver\common\source\DSP2833x_Xintf.obj" \
"Driver\common\source\DSP2833x_usDelay.obj" 

C_DEPS__QUOTED += \
"Driver\common\source\DSP2833x_Adc.d" \
"Driver\common\source\DSP2833x_CpuTimers.d" \
"Driver\common\source\DSP2833x_DMA.d" \
"Driver\common\source\DSP2833x_DefaultIsr.d" \
"Driver\common\source\DSP2833x_ECan.d" \
"Driver\common\source\DSP2833x_ECap.d" \
"Driver\common\source\DSP2833x_EPwm.d" \
"Driver\common\source\DSP2833x_EQep.d" \
"Driver\common\source\DSP2833x_Gpio.d" \
"Driver\common\source\DSP2833x_I2C.d" \
"Driver\common\source\DSP2833x_Mcbsp.d" \
"Driver\common\source\DSP2833x_MemCopy.d" \
"Driver\common\source\DSP2833x_PieCtrl.d" \
"Driver\common\source\DSP2833x_PieVect.d" \
"Driver\common\source\DSP2833x_Sci.d" \
"Driver\common\source\DSP2833x_Spi.d" \
"Driver\common\source\DSP2833x_SysCtrl.d" \
"Driver\common\source\DSP2833x_Xintf.d" 

ASM_DEPS__QUOTED += \
"Driver\common\source\DSP2833x_ADC_cal.d" \
"Driver\common\source\DSP2833x_CSMPasswords.d" \
"Driver\common\source\DSP2833x_CodeStartBranch.d" \
"Driver\common\source\DSP2833x_DBGIER.d" \
"Driver\common\source\DSP2833x_DisInt.d" \
"Driver\common\source\DSP2833x_usDelay.d" 

ASM_SRCS__QUOTED += \
"../Driver/common/source/DSP2833x_ADC_cal.asm" \
"../Driver/common/source/DSP2833x_CSMPasswords.asm" \
"../Driver/common/source/DSP2833x_CodeStartBranch.asm" \
"../Driver/common/source/DSP2833x_DBGIER.asm" \
"../Driver/common/source/DSP2833x_DisInt.asm" \
"../Driver/common/source/DSP2833x_usDelay.asm" 

C_SRCS__QUOTED += \
"../Driver/common/source/DSP2833x_Adc.c" \
"../Driver/common/source/DSP2833x_CpuTimers.c" \
"../Driver/common/source/DSP2833x_DMA.c" \
"../Driver/common/source/DSP2833x_DefaultIsr.c" \
"../Driver/common/source/DSP2833x_ECan.c" \
"../Driver/common/source/DSP2833x_ECap.c" \
"../Driver/common/source/DSP2833x_EPwm.c" \
"../Driver/common/source/DSP2833x_EQep.c" \
"../Driver/common/source/DSP2833x_Gpio.c" \
"../Driver/common/source/DSP2833x_I2C.c" \
"../Driver/common/source/DSP2833x_Mcbsp.c" \
"../Driver/common/source/DSP2833x_MemCopy.c" \
"../Driver/common/source/DSP2833x_PieCtrl.c" \
"../Driver/common/source/DSP2833x_PieVect.c" \
"../Driver/common/source/DSP2833x_Sci.c" \
"../Driver/common/source/DSP2833x_Spi.c" \
"../Driver/common/source/DSP2833x_SysCtrl.c" \
"../Driver/common/source/DSP2833x_Xintf.c" 


