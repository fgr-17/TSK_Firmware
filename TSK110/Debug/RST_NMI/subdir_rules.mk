################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
RST_NMI/RST_NMI.obj: ../RST_NMI/RST_NMI.c $(GEN_OPTS) $(GEN_SRCS)
	@echo 'Building file: $<'
	@echo 'Invoking: Compiler'
	@echo 'Flags: -vmspx -g -O0 --define=__MSP430F5529__ --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/StrBus" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/RST_NMI" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A0" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/XBOB_4" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Log" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/FatFs" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/SD_Card" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/Sensores" --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/dsp" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A2" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/sgn" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Hardware" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/F5xx_F6xx_Core_Lib" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/DMA" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/MSP-EXP430F5529_HAL" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/PWM" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Flash" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/ADC" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/UART" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Maq_Estados" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --silicon_errata=CPU22 --silicon_errata=CPU21 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile'
	@echo 'Flags (cont-d): --preproc_dependency="RST_NMI/RST_NMI.pp" --obj_directory="RST_NMI"'
	$(shell echo -vmspx -g -O0 --define=__MSP430F5529__ --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/StrBus" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/RST_NMI" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A0" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/XBOB_4" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Log" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/FatFs" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/SD_Card" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/Sensores" --include_path="C:/Program Files/Texas Instruments/ccsv4/msp430/include" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/dsp" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Timer_A2" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores/sgn" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Hardware" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/F5xx_F6xx_Core_Lib" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Medicion_Sensores" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/DMA" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/MSP-EXP430F5529_HAL" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/PWM" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Flash" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/ADC" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/UART" --include_path="C:/Users/Dase/Documents/CCS/Modulo_RS232/Maq_Estados" --include_path="C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/include" --diag_warning=225 --silicon_errata=CPU22 --silicon_errata=CPU21 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=minimal --preproc_with_compile > "RST_NMI/RST_NMI_ccsCompiler.opt")
	$(shell echo --preproc_dependency="RST_NMI/RST_NMI.pp" --obj_directory="RST_NMI" >> "RST_NMI/RST_NMI_ccsCompiler.opt")
	$(if $(strip $(GEN_OPTS_QUOTED)), $(shell echo $(GEN_OPTS_QUOTED) >> "RST_NMI/RST_NMI_ccsCompiler.opt"))
	$(if $(strip $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#")), $(shell echo $(subst #,$(wildcard $(subst $(SPACE),\$(SPACE),$<)),"#") >> "RST_NMI/RST_NMI_ccsCompiler.opt"))
	"C:/Program Files/Texas Instruments/ccsv4/tools/compiler/msp430/bin/cl430" -@"RST_NMI/RST_NMI_ccsCompiler.opt"
	@echo 'Finished building: $<'
	@echo ' '

