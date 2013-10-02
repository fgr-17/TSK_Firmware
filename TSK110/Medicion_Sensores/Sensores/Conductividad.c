/************************************************************************************************************
 * 
 *	@file Conductividad.c
 * 
 * 	@brief Calculo de la conductividad calibrada
 * 
 *  @version 1.0
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 06/2011
 * 
 ************************************************************************************************************/

/************************************************************************************************************
 * 											Inclusión de archivos											*
 ************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include <dma.h>
#include <dsp.h>

#include <pwm.h>
#include <adc.h>

#include <Conductividad.h>
#include <Temperatura.h>

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

int calculoFactorCompensacionTemperatura(void);
int Conductividad(void);

/************************************************************************************************************
 * 												Variables globales											*
 ************************************************************************************************************/

volatile int32_t I_Integral_Media_Onda_Promediado = 0;												// Integral de media onda de la Corriente, promediada 
volatile int32_t V_Integral_Media_Onda_Promediado = 0;												// Integral de media onda de la Tensión, promediada

volatile int32_t V_Acumulador = 0; 																	// Acumulador del promedio del array de tensión		
volatile int32_t I_Acumulador = 0; 																	// Acumulador del promedio del array de corriente

volatile int32_t I_Maximo_Promedio = 0;																// Promedio de los valores máximos de corriente
volatile int32_t V_Maximo_Promedio = 0;																// Promedio de los valores máximos de tensión

volatile uint16_t valorBajoConductividad = 6;            											// Primera medicion calibracion
volatile uint16_t valorAltoConductividad = 740;          											// Segunda medicion calibracion

int16_t	valorBajoIngConductividad = 0;																// Primera valor medicion ingresado calibracion
int16_t valorAltoIngConductividad = 1257;															// Segundo valor medicion ingresado calibracion

volatile int32_t Pendiente_Conductividad = 753 << ESCALA_PENDIENTE_CONDUCTIVIDAD;					// Pendiente de conductividad calculada desde la PC				
volatile int32_t Offset_Conductividad = -1;															// Offset de conductividad calculada desde la PC

volatile int16_t conductividadProcesada = 0;														// Valor final de conductividad
volatile int16_t conductividadProcesada_recibida = 0;												// Valor final de conductividad recibido del Slave

volatile int16_t conductividadProcesada_prev = 0;													// Valor final de conductividad previo
volatile int16_t conductividadProcesada_recibida_prev = 0;											// Valor final de conductividad recibido_previo

long PConductividad;
long PConductividadCompensada;
long OffsetConductividadCompensada;
long factorCorreccion;

#define ESCALA_COMPENSACION_TEMPERATURA 	7														// Escala del coeficiente de temperatura
long compensacionTemperatura = (21 << ESCALA_COMPENSACION_TEMPERATURA);								// Valor del coeficiente de temperatura * 100
long corrimiento_temperatura;
int8_t compensacionPorTemperaturaFlag = 1;															// Flag que indica si se compensa(1) o no por temperatura(0)


/************************************************************************************************************
 * 											Implementación de funciones										*
 ************************************************************************************************************/

/************************************************************************************************************
 * 																											*
 * 	@brief Calculo del factor de correccion por temperatura													*
 * 																											* 
 *  @returns 0 funcionamiento correcto																		*
 * 																											*
 ************************************************************************************************************/

int calculoFactorCompensacionTemperatura(void)
{
 	corrimiento_temperatura = (long)(temperaturaProcesada - 250);
 	
 	factorCorreccion   = (long)(compensacionTemperatura);
 	factorCorreccion  *= (long)(corrimiento_temperatura);
 	factorCorreccion >>= (long) ESCALA_COMPENSACION_TEMPERATURA;	
 	factorCorreccion  += (long) 10000;

	return 0;
}

/*********************************************************************************************************
 * 
 *  @brief Calculo de la pendiente de conductivadad con los valores tomados e ingresados en la calibracion
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 *********************************************************************************************************/

/*
*****************************************************************************************
*********************************Calibracion Conductividad*******************************
*****************************************************************************************
;......................................................
;Calculo para la pendiente
;valorBajoConductividad   		-----> 	Primera medicion
;valorAltoConductividad   		-----> 	Segunda medicion
;
;valorBajoIngConductividad		-----> 	Primera valor medicion ingresado calibracion
;valorAltoIngConductividad		-----> 	Segundo valor medicion ingresado calibracion
;
***************************************************************************************** */
/*
int calculoPendienteConductividad(void)
{

	PConductividad = valorAltoIngConductividad - valorBajoIngConductividad; 
	PConductividad *= 10000; 
	PConductividad /= (valorAltoConductividad - valorBajoConductividad);
		 
	return 0;
}
*/
/*********************************************************************************************************
 * 
 *  @brief Procesamiento de lo que entrega el AD, se muestra por pantalla
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 *********************************************************************************************************/

int Conductividad(void)
{
	int32_t calculo_parcial = 0;
	int32_t I_Valor_Pico_Proporcional = 0;
	int32_t V_Valor_Pico_Proporcional = 0;
	
 	if((I_Integral_Media_Onda_Promediado < 0) || (V_Integral_Media_Onda_Promediado < 0))
		calculo_parcial = 0;
	else
	{
		I_Valor_Pico_Proporcional = I_Integral_Media_Onda_Promediado;
		V_Valor_Pico_Proporcional = V_Integral_Media_Onda_Promediado;
		
 		calculo_parcial = I_Valor_Pico_Proporcional;
 		calculo_parcial *= Pendiente_Conductividad;													// Multiplico por el valor de pendiente calibrada
	 	calculo_parcial >>= (int32_t)ESCALA_PENDIENTE_CONDUCTIVIDAD;								// Quito el factor de escala de la pendiente
 		calculo_parcial /= V_Valor_Pico_Proporcional; 	 
	 	calculo_parcial += Offset_Conductividad;													// Sumo el valor de conductividad
	}
 	 
 	if(compensacionPorTemperaturaFlag)
	{
	/****************************************************************************************************
	 *  	        (CSensorB - CSensorA)    ( 1 + Coef_Temp  (Temp - 250) )
	 *	PSensor =   --------------------- *        ---------   ----------- 
	 *      	     (SensorB - SensorA)            10000           10
	 *
	 * 
	 * 
	 *  	                   CSensorA       (1 + Coef_Temp (Temp - 250) )
	 *	OSensor = SensorA - ------------- *      ---------  ----------
	 *  		                PSensor               10000       10
	 * 
	 ****************************************************************************************************/

		calculoFactorCompensacionTemperatura();
	 	
	 	calculo_parcial *= 10000;
	 	calculo_parcial /= factorCorreccion;
	 }
  	/*****************************************************************************************************/
        
   	if(calculo_parcial < (int32_t)CONDUCTIVIDAD_MIN) calculo_parcial = (int32_t)CONDUCTIVIDAD_MIN;
	if(calculo_parcial > (int32_t)CONDUCTIVIDAD_MAX) calculo_parcial = (int32_t)CONDUCTIVIDAD_MAX;
   
   	conductividadProcesada_prev = conductividadProcesada; 
	conductividadProcesada = (int16_t) calculo_parcial;
   
	return 0;
}

/******************************************************************************************************** 	
 * 											Final del Archivo											*
 ********************************************************************************************************/

