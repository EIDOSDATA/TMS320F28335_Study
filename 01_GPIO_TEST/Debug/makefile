################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./Core/Src/f2833x_pinmux.obj" \
"./Core/Src/main.obj" \
"./Driver/common/source/DSP2833x_ADC_cal.obj" \
"./Driver/common/source/DSP2833x_Adc.obj" \
"./Driver/common/source/DSP2833x_CSMPasswords.obj" \
"./Driver/common/source/DSP2833x_CodeStartBranch.obj" \
"./Driver/common/source/DSP2833x_CpuTimers.obj" \
"./Driver/common/source/DSP2833x_DBGIER.obj" \
"./Driver/common/source/DSP2833x_DMA.obj" \
"./Driver/common/source/DSP2833x_DefaultIsr.obj" \
"./Driver/common/source/DSP2833x_DisInt.obj" \
"./Driver/common/source/DSP2833x_ECan.obj" \
"./Driver/common/source/DSP2833x_ECap.obj" \
"./Driver/common/source/DSP2833x_EPwm.obj" \
"./Driver/common/source/DSP2833x_EQep.obj" \
"./Driver/common/source/DSP2833x_Gpio.obj" \
"./Driver/common/source/DSP2833x_I2C.obj" \
"./Driver/common/source/DSP2833x_Mcbsp.obj" \
"./Driver/common/source/DSP2833x_MemCopy.obj" \
"./Driver/common/source/DSP2833x_PieCtrl.obj" \
"./Driver/common/source/DSP2833x_PieVect.obj" \
"./Driver/common/source/DSP2833x_Sci.obj" \
"./Driver/common/source/DSP2833x_Spi.obj" \
"./Driver/common/source/DSP2833x_SysCtrl.obj" \
"./Driver/common/source/DSP2833x_Xintf.obj" \
"./Driver/common/source/DSP2833x_usDelay.obj" \
"./Driver/headers/source/DSP2833x_GlobalVariableDefs.obj" \
"../28335_RAM_lnk.cmd" \
"../DSP2833x_Headers_nonBIOS.cmd" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include Core/Src/subdir_vars.mk
-include Driver/common/source/subdir_vars.mk
-include Driver/headers/source/subdir_vars.mk
-include subdir_rules.mk
-include Core/Src/subdir_rules.mk
-include Driver/common/source/subdir_rules.mk
-include Driver/headers/source/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
01_GPIO_TEST.out 

EXE_OUTPUTS__QUOTED += \
"01_GPIO_TEST.out" 


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "01_GPIO_TEST.out"

# Tool invocations
01_GPIO_TEST.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: C2000 Linker'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 -O2 --opt_for_speed=5 --advice:performance=all -g --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi -z -m"01_GPIO_TEST.map" --stack_size=0x300 --warn_sections -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/lib" -i"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="01_GPIO_TEST_linkInfo.xml" --rom_model -o "01_GPIO_TEST.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXE_OUTPUTS__QUOTED)
	-$(RM) "Core\Src\f2833x_pinmux.obj" "Core\Src\main.obj" "Driver\common\source\DSP2833x_ADC_cal.obj" "Driver\common\source\DSP2833x_Adc.obj" "Driver\common\source\DSP2833x_CSMPasswords.obj" "Driver\common\source\DSP2833x_CodeStartBranch.obj" "Driver\common\source\DSP2833x_CpuTimers.obj" "Driver\common\source\DSP2833x_DBGIER.obj" "Driver\common\source\DSP2833x_DMA.obj" "Driver\common\source\DSP2833x_DefaultIsr.obj" "Driver\common\source\DSP2833x_DisInt.obj" "Driver\common\source\DSP2833x_ECan.obj" "Driver\common\source\DSP2833x_ECap.obj" "Driver\common\source\DSP2833x_EPwm.obj" "Driver\common\source\DSP2833x_EQep.obj" "Driver\common\source\DSP2833x_Gpio.obj" "Driver\common\source\DSP2833x_I2C.obj" "Driver\common\source\DSP2833x_Mcbsp.obj" "Driver\common\source\DSP2833x_MemCopy.obj" "Driver\common\source\DSP2833x_PieCtrl.obj" "Driver\common\source\DSP2833x_PieVect.obj" "Driver\common\source\DSP2833x_Sci.obj" "Driver\common\source\DSP2833x_Spi.obj" "Driver\common\source\DSP2833x_SysCtrl.obj" "Driver\common\source\DSP2833x_Xintf.obj" 
	-$(RM) "Driver\common\source\DSP2833x_usDelay.obj" "Driver\headers\source\DSP2833x_GlobalVariableDefs.obj" 
	-$(RM) "Core\Src\f2833x_pinmux.d" "Core\Src\main.d" "Driver\common\source\DSP2833x_Adc.d" "Driver\common\source\DSP2833x_CpuTimers.d" "Driver\common\source\DSP2833x_DMA.d" "Driver\common\source\DSP2833x_DefaultIsr.d" "Driver\common\source\DSP2833x_ECan.d" "Driver\common\source\DSP2833x_ECap.d" "Driver\common\source\DSP2833x_EPwm.d" "Driver\common\source\DSP2833x_EQep.d" "Driver\common\source\DSP2833x_Gpio.d" "Driver\common\source\DSP2833x_I2C.d" "Driver\common\source\DSP2833x_Mcbsp.d" "Driver\common\source\DSP2833x_MemCopy.d" "Driver\common\source\DSP2833x_PieCtrl.d" "Driver\common\source\DSP2833x_PieVect.d" "Driver\common\source\DSP2833x_Sci.d" "Driver\common\source\DSP2833x_Spi.d" "Driver\common\source\DSP2833x_SysCtrl.d" "Driver\common\source\DSP2833x_Xintf.d" "Driver\headers\source\DSP2833x_GlobalVariableDefs.d" 
	-$(RM) "Driver\common\source\DSP2833x_ADC_cal.d" "Driver\common\source\DSP2833x_CSMPasswords.d" "Driver\common\source\DSP2833x_CodeStartBranch.d" "Driver\common\source\DSP2833x_DBGIER.d" "Driver\common\source\DSP2833x_DisInt.d" "Driver\common\source\DSP2833x_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

