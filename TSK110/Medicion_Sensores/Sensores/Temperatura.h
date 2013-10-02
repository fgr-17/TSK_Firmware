/*********************************************************************************************************
 * 
 *	@file Temperatura.h
 * 
 * 	@brief Header del archivo Temperatura.c
 * 
 *  @version 1.0
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 06/2011
 * 
 *********************************************************************************************************/

#ifndef TEMPERATURA_H_
#define TEMPERATURA_H_

/************************************************************************************************************
 * 											Inclusión de archivos											*
 ************************************************************************************************************/
 
#include <msp430f5529.h>
#include <stdint.h>
#include <string.h>

#include <adc.h>

/************************************************************************************************************
 * 											Definición de constnates										*
 ************************************************************************************************************/
 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define F_LEN_CONSTANTEcONVER					10
#define F_LEN_PENDIENTEtABLA					10
#define F_LEN_TOTAL								(F_LEN_CONSTANTEcONVER + F_LEN_PENDIENTEtABLA) 				
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ConstanteConver							(198626)
#define PendienteTabla							(2640) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define TEMPERATURA_MAXIMA 						500																						// 50.0°C
#define TEMPERATURA_MINIMA						0																						// 0.0°c
#define R_TEMPERATURA_MAXIMA					1194																					// Resistencia de la PT1000 correspondiente a 50°C						
#define R_TEMPERATURA_MINIMA					1000																					// Resistencia de la PT1000 correspondiente a 0°C
#define TEMPERATURA_SPAN						(TEMPERATURA_MAXIMA - TEMPERATURA_MINIMA)												// Spam de la escala de temperatura
#define R_TEMPERATURA_SPAN						(R_TEMPERATURA_MAXIMA - R_TEMPERATURA_MINIMA)											// Span de la medición de resistencia
#define ESCALA_PENDIENTE_TEMPERATURA			16																						// Bits de escala para mayor resolución de temperatura
////////////////////////////////////////////// Calculo las constantes para conversión de resistencia a temperatura:  //////////////////////////////////////////////
#define PENDIENTE_TEMPERATURA					((TEMPERATURA_SPAN << ESCALA_PENDIENTE_TEMPERATURA) / R_TEMPERATURA_SPAN)				// Pendiente de conversión
#define OFFSET_TEMPERATURA						((R_TEMPERATURA_MINIMA * PENDIENTE_TEMPERATURA) >> ESCALA_PENDIENTE_TEMPERATURA)		// Offset (ES NEGATIVO)		
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define PT100ASetpointAddress     				(MEMORIA_BASE + 0x0010) 																// Dir. de memoria la calibracion
#define PT100BSetpointAddress     				(MEMORIA_BASE + 0x0012)																	// Dir. de memoria la calibracion
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ESCALA_PENDIENTE_RESISTENCIA			10																						// Cantidad de bits de escala de la pendiente para mayor resolución
#define ESCALA_DECIMALES_RESISTENCIA			100																						// Cantidad de ceros de decimales de resistencia
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************************************************
 * 											Funciones externas												*
 ************************************************************************************************************/

extern int Temperatura(void);
extern int calculoPendientePT100(void);
extern int Calculo_Pendiente_Resistencia(void);
extern int Resistencia_Calibracion(void);
 
/************************************************************************************************************
 * 											Variables externas												*
 ************************************************************************************************************/

extern volatile int16_t temperaturaProcesada;
extern volatile int16_t temperaturaProcesada_recibida;
extern volatile int16_t temperaturaProcesada_prev;				 										// Valor de temperatura calibrada previo
extern volatile int16_t temperaturaProcesada_recibida_prev;												// Valor final de conductividad recibido del Slave previo

extern int32_t Resistencia_Medida;

extern int32_t Pendiente_Resistencia;																	// Valor de la pendiente
extern int32_t Offset_Resistencia;																		// Valor del offset

#endif /*TEMPERATURA_H_*/

/************************************************************************************************************
 * 											Final del archivo												*
 ************************************************************************************************************/
