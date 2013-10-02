/************************************************************************************************************
 * 
 *	@file Conductividad.h
 * 
 * 	@brief Calculo de la conductividad calibrada. Header de Conductividad.c
 * 
 *  @version 1.0
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 06/2011
 * 
 ************************************************************************************************************/

#ifndef CONDUCTIVIDAD_H_
#define CONDUCTIVIDAD_H_

/************************************************************************************************************
 * 											Definición de constantes										*
 ************************************************************************************************************/
  
#define valorBajoCondSetpointAddress				(MEMORIA_BASE + 0x001E)
#define valorAltoCondSetpointAddress				(MEMORIA_BASE + 0x0020)
#define valorBajoCondIngresadoSetpointAddress		(MEMORIA_BASE + 0x0022)
#define valorAltoCondIngresadoSetpointAddress		(MEMORIA_BASE + 0x0024)
#define valorCompTempCondSetpointAddress			(MEMORIA_BASE + 0x0026)
#define valorCompTempCondFlagSetpointAddress		(MEMORIA_BASE + 0x0028)

#define digitoCond1000 								(0)
#define digitoCond0100 								(1)
#define digitoCond0010 								(2)
#define digitoCond0001 								(3)
 
#define posDigitoCond1000 							35
#define posDigitoCond0100 							51
#define posDigitoCond0010 							67
#define posDigitoCond0001 							83 
 
#define MaximoValorConductividad					2000
#define MAX_CompensacionTemp						500											// 5,00
 
#define N_PROM 										128											// 375	// Cantidad de mediciones a promediar
#define SHIFT_DIV_N_PROM							7											// Para hacer una división por shift		
#define ESCALA_PENDIENTE_CONDUCTIVIDAD				4											// Cantidad de bits de escala de la pendiente para mayor resolución

#define CONDUCTIVIDAD_MAX							1500										// Valor máximo posible de conductividad
#define CONDUCTIVIDAD_MIN							0											// Valor mínimo posible de conductividad

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

extern int Conductividad(void);
extern int calculoPendienteConductividad(void);

/************************************************************************************************************
 * 												Variables globales											*
 ************************************************************************************************************/

extern volatile int32_t conductividadPromedioObtenida;											

extern volatile int16_t conductividadProcesada;													// Conductividad calibrada
extern volatile int16_t conductividadProcesada_recibida;										// Valor final de conductividad recibido del Slave

extern volatile int16_t conductividadProcesada_prev;											// Valor final de conductividad previo
extern volatile int16_t conductividadProcesada_recibida_prev;									// Valor final de conductividad recibido_previo

extern volatile int32_t Pendiente_Conductividad;												// Pendiente de conductividad calculada desde la PC				
extern volatile int32_t Offset_Conductividad;													// Offset de conductividad calculada desde la PC

extern volatile int32_t I_Maximo_Promedio;														// Promedio de los valores máximos de corriente
extern volatile int32_t V_Maximo_Promedio;														// Promedio de los valores máximos de tensión

extern volatile int32_t V_Acumulador;	 														// Acumulador del promedio del array de tensión		
extern volatile int32_t I_Acumulador; 															// Acumulador del promedio del array de corriente

extern volatile int32_t I_Integral_Media_Onda_Promediado;										// Integral de media onda de la Corriente, promediada 
extern volatile int32_t V_Integral_Media_Onda_Promediado;										// Integral de media onda de la Tensión, promediada

#endif /*CONDUCTIVIDAD_H_*/

/******************************************************************************************************** 	
 * 											Final del Archivo											*
 ********************************************************************************************************/
