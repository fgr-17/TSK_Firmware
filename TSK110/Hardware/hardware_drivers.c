/********************************************************************************************************************
 *	@file 	hardware_drivers.c																						*
 * 																													*
 * 	@brief 	manejo del modo PWM del timer																			*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 												INCLUSIÓN DE ARCHIVOS												*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include <hardware_macros.h>
#include <hardware_drivers.h>
#include <hardware_constantes.h>

/********************************************************************************************************************
 * 											PROTOTIPOS DE FUNCIONES												*
 ********************************************************************************************************************/

int Inicializacion_Hardware_Conductividad(void);
int Inicializacion_Hardware_Board(void);

int PCB_Analogico_ON (void);
int PCB_Analogico_OFF (void);

/********************************************************************************************************************
 * 											IMPLEMENTACIÓN DE FUNCIONES												*
 ********************************************************************************************************************/
 
/********************************************************************************************************************
 * 
 * 	@brief Inicialización de entradas/salidas asociadas a la medición de conductividad.
 * 
 * 	@params	none
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/
 
int Inicializacion_Hardware_Conductividad(void)
{
	// Pines de entrada para la placa : "TSK100.PcbDoc"
	if(Dir_Pin(CONDUCTIVIDAD_1_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Conductividad 1
	if(Dir_Pin(CONDUCTIVIDAD_2_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Conductividad 2		
	if(Dir_Pin(CONDUCTIVIDAD_3_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Conductividad 3	
	
	// Pin de salida para la señal PWM:
	if(Dir_Pin(DAC_PWM_PIN_O, DIR_SALIDA, FUNCION_IO))				return 1;		// Defino el pin que uso como salida PWM
	
	return 0; 														// Finalización OK!
}

/********************************************************************************************************************
 * 																													*
 * 	@brief Inicialización del hardware de la placa																	*	
 * 																													*
 * 	@params	none																									*
 * 																													*
 * 	@returns 0 funcionamiento correcto																				*
 * 																													*
 ********************************************************************************************************************/

int Inicializacion_Hardware_Board(void)
{
	
	// Pines de salida de los leds
	if(Dir_Pin(LED_CALIB_PIN_O, DIR_SALIDA, FUNCION_PERIF))			return 1;		// Defijo el pin de salida del led CALIBRACION
	if(Dir_Pin(LED_CONFIG_PIN_O, DIR_SALIDA, FUNCION_PERIF))		return 1;		// Defijo el pin de salida del led CONFIGURACION
	if(Dir_Pin(LED_SLAVE_PIN_O, DIR_SALIDA, FUNCION_PERIF))			return 1;		// Defijo el pin de salida del led SLAVE			
	if(Dir_Pin(LED_MASTER_PIN_O, DIR_SALIDA, FUNCION_PERIF))		return 1;		// Defijo el pin de salida del led MASTER
	
	// Pines de salida particulares.
	if(Dir_Pin(REG_ANALOG_SHUTDOWN_PIN_O, DIR_SALIDA, FUNCION_IO))	return 1;		// Defino el pin de salida de apagado del PCB Analógico
	if(Dir_Pin(DAC_PWM_PIN_O, DIR_SALIDA, FUNCION_IO))				return 1;		// Defino el pin de salida de apagado del PCB Analógico
	
	// Inicialización de pines de ADC de los sensores.
	if(Dir_Pin(CONDUCTIVIDAD_1_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Canal 0 Conductividad 1
	if(Dir_Pin(CONDUCTIVIDAD_2_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Canal 1 Conductividad 2
	if(Dir_Pin(CONDUCTIVIDAD_3_PIN_I, DIR_ENTRADA, FUNCION_PERIF))	return 1;		// Defino el pin de entrada del ADC12 Canal 2 Conductividad 3
	
	if(Dir_Pin(TEMPERATURA_PIN_I, DIR_ENTRADA, FUNCION_PERIF))		return 1;		// Defino el pin de entrada del ADC12 Canal 3 Temperatura

    // Configure SDCard ports
    P3OUT |= BIT7;                  // SD_CS to high
    P3DIR |= BIT7;
    P4OUT &= ~(BIT1 + BIT3);        // SIMO, SCK
    P4DIR &= ~BIT2;                 // SOMI pin is input
    P4DIR |= BIT1 + BIT3;

	// PCB_Analogico_ON();																// Activo el Regulador analógico

	REFCTL0 &= ~REFMSTR;                      										// Reset REFMSTR to hand over control to ADC 
		
	return 0;														// Finalización OK!		
}

/********************************************************************************************************************
 * 
 * 	@brief Enciendo el hardware dedicado al procesamiento analógico de señales(pin SHUTDOWN del regulador analógico)
 * 
 * 	@params	none
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int PCB_Analogico_ON (void)
{
	ANALOG_SHUTDOWN_PORT_OUT |= ANALOG_SHUTDOWN_PIN_OUT;
	return 0;	
}

/********************************************************************************************************************
 * 
 * 	@brief Apago el hardware dedicado al procesamiento analógico de señales(pin SHUTDOWN del regulador analógico)
 * 
 * 	@params	none
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int PCB_Analogico_OFF (void)
{
	ANALOG_SHUTDOWN_PORT_OUT &= ~ANALOG_SHUTDOWN_PIN_OUT;
	return 0;	
} 


/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
