################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
PWM/pwm.obj: ../PWM/pwm.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv5/tools/compiler/msp430/bin/cl430" -vmspx -g -O0 --define=__MSP430F5529__ --include_path="C:/ti/ccsv5/ccs_base/msp430/include" --include_path="C:/Documents and Settings/fede/Mis documentos/TSK100/Firmware_rep/Repo/TSK110" --include_path="C:/ti/ccsv5/tools/compiler/msp430/include" --diag_warning=225 --silicon_errata=CPU22 --silicon_errata=CPU21 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="PWM/pwm.pp" --obj_directory="PWM" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


