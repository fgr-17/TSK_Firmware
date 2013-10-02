/********************************************************************************************************************
 *	@file Timer_A0.h																								*
 * 																													*
 * 	@brief header del archivo del "Timer_A1.c"																		*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/
 
#ifndef TIMERA_H_
#define TIMERA_H_

/********************************************************************************************************************
 * 												DEFINICIÓN DE CONSTANTES											*
 ********************************************************************************************************************/

#define FRECUENCIA_MUESTREO_INIT		1					// cada 1ms

#define FRECUENCIA_MUESTREO_INIT_CON	150					// Frecuencia de muestreo inicial en kHz
#define CANTIDAD_DE_CANALES 			2					// Cantidad de canales a convertir

#define FRECUENCIA_CLOCK_ADC 			(FRECUENCIA_MUESTREO_INIT_CON * CANTIDAD_DE_CANALES)		// Frecuencia clk del ADC

/********************************************************************************************************************
 * 												FUNCIONES EXTERNAS													*
 ********************************************************************************************************************/

extern void confTimerACanales(void);
extern int confTimerAConductividad(void);

#endif /*TIMERA_H_*/
