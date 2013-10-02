/********************************************************************************************************************
 *	@file dsp.c																										*
 * 																													*
 * 	@brief Procesamiento de señales																					*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 											INCLUSION DE ARCHIVOS													*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include "DMA/dma.h"
#include "PWM/pwm.h"
#include "Medicion_Sensores/dsp/dsp.h"
#include "Medicion_Sensores/Sensores/Conductividad.h"

/********************************************************************************************************************
 * 											DEFINICIÓN DE CONSTANTES												*
 ********************************************************************************************************************/

#define MUESTRAS_A_DESPRECIAR 		0

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

Sgn_Res Leer_Sgn(Type_Sgn sgn_a_leer, int16_t delay);			// Lee una muestra de una señal
int Incrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);				// Incrementa el índice de una señal en forma circular

Sgn_Res Leer_e_Incrementar_Sgn(Type_Sgn* p_sgn_a_leer);			// Lee e incrementa el buffer de una señal
int Decrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);				// Decrementa el índice de una señal en forma circular

// Carga los campos de la estructura "Type_Sgn":
int Asociar_Sgn (Type_Sgn* sgn_in, Sgn_Res* array_a_ascociar, int16_t sgn_largo, uint16_t fs);
int Procesar_Sgn_Conductividad(DMA_Type V_Array, DMA_Type I_Array);

/********************************************************************************************************************
 * 											SEÑALES DE ALMACENADAS													*
 ********************************************************************************************************************/ 

# define xN 	16

int16_t sgn_senoidal_largo = xN;			// Cantidad de muestras de la señal
uint16_t sgn_senoidal_fs = FSW_INICIAL;		// Frecuencia de sampleo de la señal

Sgn_Res sgn_senoidal_muestras[xN] = {
	#include "Medicion_Sensores/sgn/sen_90_15kHz_16m.h"
};


/********************************************************************************************************************
 * 											IMPLEMENTACION DE FUNCIONES												*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 
 *  @brief Lee una muestra de una señal con un cierto delay.
 * 
 * 	@returns Sgn_Res, muestra leida
 * 
 ********************************************************************************************************************/

Sgn_Res Leer_Sgn(Type_Sgn sgn_a_leer, int16_t delay)
{
	int16_t pre_indice;
	int16_t indice;
	Sgn_Res muestra_leida;
	
	if (delay >= sgn_a_leer.sgn_largo)
	{
		return ERROR_SGN_LEIDA;							// Devuelvo error si el delay es mayor al largo de la señal
	}
	
	pre_indice = sgn_a_leer.sgn_indice - delay;			// Aplico el delay al indice de la señal
	
	if(pre_indice < 0)									// Si el indice con delay es mayor al índice actual	
	{
		indice = sgn_a_leer.sgn_largo + pre_indice; 	// Vuelvo a leer desde el final
	}
	else if(pre_indice >= sgn_a_leer.sgn_largo)			// Si el delay es hacia adelante y desborda el array
	{
		indice = pre_indice - sgn_a_leer.sgn_largo;		// Vuelvo a leer desde el principio
	}
	else
	{
		indice = pre_indice;							// Sino leo directamente
	}
	
	muestra_leida = sgn_a_leer.sgn_muestras[indice];	// Actualizo la muestra leída.
	
	return muestra_leida; 
}

/********************************************************************************************************************
 * 
 * 	@brief Decrementa el indice de una señal.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Decrementar_Indice_Sgn (Type_Sgn* sgn_a_leer)

{
		
	return 0; 														// La función terminó OK!
	
}

/********************************************************************************************************************
 * 
 * 	@brief Incrementa el indice de una señal.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Incrementar_Indice_Sgn (Type_Sgn* sgn_a_leer)
{
	(*sgn_a_leer).sgn_indice++;									// Incremento el índice del array

	if(sgn_a_leer->sgn_indice >= sgn_a_leer->sgn_largo)			// Si llegué al final del array
		sgn_a_leer->sgn_indice = 0;								// Lo inicializo al principio

	return 0;													// La función terminó OK!
	
}

/********************************************************************************************************************
 * 
 * 	@brief Lee una muestra de una señal con un cierto delay.
 * 
 * 	@returns Sgn_Res, muestra leida
 * 
 ********************************************************************************************************************/ 

Sgn_Res Leer_e_Incrementar_Sgn(Type_Sgn* p_sgn_a_leer)
{
	Sgn_Res muestra_leida;
	
	if((muestra_leida = Leer_Sgn((*p_sgn_a_leer), 0)) == ERROR_SGN_LEIDA)
		return ERROR_SGN_LEIDA;
	
	if(Incrementar_Indice_Sgn(p_sgn_a_leer))
		return ERROR_SGN_LEIDA;	
	
	return muestra_leida;
}

/********************************************************************************************************************
 * 
 * 	@brief Lee una muestra de una señal con un cierto delay.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/ 

int Asociar_Sgn (Type_Sgn* sgn_in, Sgn_Res* array_a_ascociar, int16_t sgn_largo, uint16_t fs)
{
	if(sgn_largo <= 0)								// Si el largo es cero o negativo
		return 1;									// devuelvo error
		
	if(fs <= 0)										// Si la fs es cero o negativa
		return 1;									// devuelvo error.
	
	sgn_in->sgn_muestras = array_a_ascociar;			// Copio el array con las muestras
	sgn_in->sgn_largo	= sgn_largo;				// Guardo el largo del array
	sgn_in->fs			= fs;						// Establezco la fs
	sgn_in->sgn_indice 	= 0;						// Inicializo el índice del array
	
	return 0;										// La funcion terminó OK!
}

/********************************************************************************************************************
 * 
 * 	@brief Procesa las muestras obtenidas para obtener el valor de conductividad
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Procesar_Sgn_Conductividad(DMA_Type V_Array, DMA_Type I_Array)
{
	
	int i ;																			// Índice del bucle for			
	
	Sgn_Res I_Med;
	Sgn_Res V_Med;
	
	//////////////////////////////////////////
	// Calculo el promedio de los 2 arrays	//
	//////////////////////////////////////////
	
	V_Acumulador = 0;
	I_Acumulador = 0;
		
	for(i = 0; i < LARGO_BUFFER_DMA; i++)				
	{
		V_Acumulador += (int32_t) V_Array.Buffer[i];								// Acumulador de los valores de tensión
		I_Acumulador += (int32_t) I_Array.Buffer[i];								// Acumulador de los valores de corriente
	}
	
	V_Med = (Sgn_Res) (V_Acumulador >> SHIFT_DIV_BUFFER);							// Calculo el valor medio de tensión
	I_Med = (Sgn_Res) (I_Acumulador >> SHIFT_DIV_BUFFER);							// Calculo el valor medio de corriente

	V_Acumulador = 0;
	I_Acumulador = 0;
	
	for(i = 0; i < LARGO_BUFFER_DMA; i++)											// Hago la suma de un signo de las 2 señales
	{
		if(V_Array.Buffer[i] >= V_Med)
			V_Acumulador += (int32_t) (V_Array.Buffer[i] - V_Med);					// Positivo para tensión
		if(I_Array.Buffer[i] <= I_Med)
			I_Acumulador += (int32_t) (I_Med - I_Array.Buffer[i]);					// Negativo para corriente (la señal está invertida)
	}
	
	if(LARGO_BUFFER_DMA > 0)
	{
		V_Maximo_Promedio = (Sgn_Res) (V_Acumulador >> SHIFT_DIV_BUFFER);			// Calculo el valor medio de tensión
		I_Maximo_Promedio = (Sgn_Res) (I_Acumulador >> SHIFT_DIV_BUFFER);			// Calculo el valor medio de corriente
	}
	
	return 0;
		
}

/************************************************************************************
 * 								Fin del Archivo										*
 ************************************************************************************/
