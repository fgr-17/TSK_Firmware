/********************************************************************************************************************
 *	@file 	sensores.c																								*
 * 																													*
 * 	@brief	rutinas de atenci�n de la medici�n de los sensores.														*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author	ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 11/2011																									*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 											Inclusi�n de archivos													*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include <strbus.h>

#include <sensores.h>

#include <adc.h>
#include <dma.h>
#include <pwm.h>
#include <dsp.h>

#include <hardware_macros.h>
#include <hardware_drivers.h>

#include <xbob_4.h>
#include <Temperatura.h>
#include <Conductividad.h>
#include <log.h>
 
/********************************************************************************************************************
 * 											Prototipos de funciones													*
 ********************************************************************************************************************/
 
int Medicion_Sensores(void);

/********************************************************************************************************************
 * 										Implementaci�n de funciones													*
 ********************************************************************************************************************/

/********************************************************************************************************************
 *	@brief 		Realizo una medici�n con los sensores seleccionados		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************************/

int Medicion_Sensores(void)
{	
	if(terminal.medicion == TERMINAL_MEDIR)															// El RTC habilit� una medici�n (se cumpli� el tiempo de adquisici�n)
	{
		switch(terminal.modos_estado)																// Chequeo el estado ppal. del equipo
		{ 
		case CALIBRACION_T:																			// Estoy calibrando temperatura
			PCB_Analogico_ON();																		// Activo el Regulador anal�gico	
			terminal.medicion = TERMINAL_MIDIENDO;													// Seteo como estado midiendo
			Iniciar_Medicion_Sensores();															// Inicializo el ADC para la medici�n de temperatura																							
			Resistencia_Calibracion();																// Ajusto la medici�n por medio de la calibraci�n.																																																															
			PCB_Analogico_OFF();																	// Desactivo el Regulador anal�gico
			break;
			
		case CALIBRACION_K:
			PCB_Analogico_ON();																		// Activo el Regulador anal�gico
			terminal.medicion = TERMINAL_MIDIENDO;
			Iniciar_Medicion_Sensores();															// Mido temperatura
			PWM_Init_TA1_1(&pwm1);																	// Inicializaci�n del Timer para salida PWM	 				
			PWM_Cargar_Sgn(&pwm1, sgn_senoidal_muestras, sgn_senoidal_largo, sgn_senoidal_fs);		// Asocio una se�al al DAC PWM:												
			Iniciar_Medicion_Conductividad();														// Mido un promedio de conductividad						
			PWM_Desactivar_Salida(&pwm1);															// Apago la se�al de PWM
			Temperatura();																			// Convierto medici�n a �C
			Conductividad();																		// Convierto medici�n a uS							
			PCB_Analogico_OFF();																	// Desactivo el Regulador anal�gico
			break;
			
		case MASTER_MIDE:
			PCB_Analogico_ON();																		// Activo el Regulador anal�gico
			terminal.medicion = TERMINAL_MIDIENDO;
			Iniciar_Medicion_Sensores();		
			PWM_Init_TA1_1(&pwm1);																	// Inicializaci�n del Timer para salida PWM	 				
			PWM_Cargar_Sgn(&pwm1, sgn_senoidal_muestras, sgn_senoidal_largo, sgn_senoidal_fs);		// Asocio una se�al al DAC PWM:												
			Iniciar_Medicion_Conductividad();														// Mido un promedio de conductividad						
			PWM_Desactivar_Salida(&pwm1);							
			Temperatura();		
			Conductividad();																																	
			// Mostrar_Mediciones();
			Enviar_Medicion_Binario();
			Archivo_Log_Datos_Binario();
			PCB_Analogico_OFF();																	// Desactivo el Regulador anal�gico				
			break;
			
		case SLAVE_MIDE:
			PCB_Analogico_ON();																		// Activo el Regulador anal�gico
			terminal.medicion = TERMINAL_MIDIENDO;
			Iniciar_Medicion_Sensores();	
			PWM_Init_TA1_1(&pwm1);																	// Inicializaci�n del Timer para salida PWM	 				
			PWM_Cargar_Sgn(&pwm1, sgn_senoidal_muestras, sgn_senoidal_largo, sgn_senoidal_fs);		// Asocio una se�al al DAC PWM:												
			Iniciar_Medicion_Conductividad();						
			PWM_Desactivar_Salida(&pwm1);															// Apago la se�al de PWM
			Temperatura();		
			Conductividad();																																	
			PCB_Analogico_OFF();																	// Desactivo el Regulador anal�gico					
			break;
		
		default:
			break;																					// ERROR EN EL SETEO DE ESTADOS CON LA MEDICION.		
		}		
	}

	return 0;
}
 
/********************************************************************************************************************
 * 												FINAL DEL ARCHIVO													*
 ********************************************************************************************************************/
