################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Timer_A1/Timer_A1.obj: ../Timer_A1/Timer_A1.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" -vmspx -g -O0 --define=__MSP430F5529__ --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/Sensores" --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/dsp" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/sgn" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Hardware" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A1" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A2" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/F5xx_F6xx_Core_Lib" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/DMA" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/MSP-EXP430F5529_HAL" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/PWM" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Modbus" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Flash" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/ADC" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/UART" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Maq_Estados" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --silicon_errata=CPU22 --silicon_errata=CPU21 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile --preproc_dependency="Timer_A1/Timer_A1.pp" --obj_directory="Timer_A1" $(GEN_OPTS_QUOTED) $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")
	@echo 'Finished building: $<'
	@echo ' '


