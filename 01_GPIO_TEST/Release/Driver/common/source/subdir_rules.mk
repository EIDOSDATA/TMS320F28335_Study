################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Driver/common/source/%.obj: ../Driver/common/source/%.asm $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Core/Inc" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Driver/common/include" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Driver/headers/include" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --advice:performance=all --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Driver/common/source/$(basename $(<F)).d_raw" --obj_directory="Driver/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

Driver/common/source/%.obj: ../Driver/common/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Core/Inc" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Driver/common/include" --include_path="C:/Users/eidos/GitHub/TMS320F28335_Study/01_GPIO_TEST/Driver/headers/include" --include_path="C:/ti/ccs1271/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --advice:performance=all --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Driver/common/source/$(basename $(<F)).d_raw" --obj_directory="Driver/common/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


