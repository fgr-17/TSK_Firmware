/********************************************************************************************************************
 *	@file dsp.c																										*
 * 																													*
 * 	@brief Procesamiento de se�ales																					*
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
 * 											DEFINICI�N DE CONSTANTES												*
 ********************************************************************************************************************/

#define MUESTRAS_A_DESPRECIAR 		0

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

Sgn_Res Leer_Sgn(Type_Sgn sgn_a_leer, int16_t delay);			// Lee una muestra de una se�al
int Incrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);				// Incrementa el �ndice de una se�al en forma circular

Sgn_Res Leer_e_Incrementar_Sgn(Type_Sgn* p_sgn_a_leer);			// Lee e incrementa el buffer de una se�al
int Decrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);				// Decrementa el �ndice de una se�al en forma circular

// Carga los campos de la estructura "Type_Sgn":
int Asociar_Sgn (Type_Sgn* sgn_in, Sgn_Res* array_a_ascociar, int16_t sgn_largo, uint16_t fs);
int Procesar_Sgn_Conductividad(DMA_Type V_Array, DMA_Type I_Array);

/********************************************************************************************************************
 * 											SE�ALES DE ALMACENADAS													*
 ********************************************************************************************************************/ 

# define xN 	16

int16_t sgn_senoidal_largo = xN;			// Cantidad de muestras de la se�al
uint16_t sgn_senoidal_fs = FSW_INICIAL;		// Frecuencia de sampleo de la se�al

Sgn_Res sgn_senoidal_muestras[xN] = {
	#include "Medicion_Sensores/sgn/sen_90_15kHz_16m.h"
};


/********************************************************************************************************************
 * 											IMPLEMENTACION DE FUNCIONES												*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 
 *  @brief Lee una muestra de una se�al con un cierto delay.
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
		return ERROR_SGN_LEIDA;							// Devuelvo error si el delay es mayor al largo de la se�al
	}
	
	pre_indice = sgn_a_leer.sgn_indice - delay;			// Aplico el delay al indice de la se�al
	
	if(pre_indice < 0)									// Si el indice con delay es mayor al �ndice actual	
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
	
	muestra_leida = sgn_a_leer.sgn_muestras[indice];	// Actualizo la muestra le�da.
	
	return muestra_leida; 
}

/********************************************************************************************************************
 * 
 * 	@brief Decrementa el indice de una se�al.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Decrementar_Indice_Sgn (Type_Sgn* sgn_a_leer)

{
		
	return 0; 														// La funci�n termin� OK!
	
}

/********************************************************************************************************************
 * 
 * 	@brief Incrementa el indice de una se�al.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Incrementar_Indice_Sgn (Type_Sgn* sgn_a_leer)
{
	(*sgn_a_leer).sgn_indice++;									// Incremento el �ndice del array

	if(sgn_a_leer->sgn_indice >= sgn_a_leer->sgn_largo)			// Si llegu� al final del array
		sgn_a_leer->sgn_indice = 0;								// Lo inicializo al principio

	return 0;													// La funci�n termin� OK!
	
}

/********************************************************************************************************************
 * 
 * 	@brief Lee una muestra de una se�al con un cierto delay.
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
 * 	@brief Lee una muestra de una se�al con un cierto delay.
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
	sgn_in->sgn_indice 	= 0;						// Inicializo el �ndice del array
	
	return 0;										// La funcion termin� OK!
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
	
	int i ;																			// �ndice del bucle for			
	
	Sgn_Res I_Med;
	Sgn_Res V_Med;
	
	//////////////////////////////////////////
	// Calculo el promedio de los 2 arrays	//
	//////////////////////////////////////////
	
	V_Acumulador = 0;
	I_Acumulador = 0;
		
	for(i = 0; i < LARGO_BUFFER_DMA; i++)				
	{
		V_Acumulador += (int32_t) V_Array.Buffer[i];								// Acumulador de los valores de tensi�n
		I_Acumulador += (int32_t) I_Array.Buffer[i];								// Acumulador de los valores de corriente
	}
	
	V_Med = (Sgn_Res) (V_Acumulador >> SHIFT_DIV_BUFFER);							// Calculo el valor medio de tensi�n
	I_Med = (Sgn_Res) (I_Acumulador >> SHIFT_DIV_BUFFER);							// Calculo el valor medio de corriente

	V_Acumulador = 0;
	I_Acumulador = 0;
	
	for(i = 0; i < LARGO_BUFFER_DMA; i++)											// Hago la suma de un signo de las 2 se�ales
	{
		if(V_Array.Buffer[i] >= V_Med)
			V_Acumulador += (int32_t) (V_Array.Buffer[i] - V_Med);					// Positivo para tensi�n
		if(I_Array.Buffer[i] <= I_Med)
			I_Acumulador += (int32_t) (I_Med - I_Array.Buffer[i]);					// Negativo para corriente (la se�al est� invertida)
	}
	
	if(LARGO_BUFFER_DMA > 0)
	{
		V_Maximo_Promedio = (Sgn_Res) (V_Acumulador >> SHIFT_DIV_BUFFER);			// Calculo el valor medio de tensi�n
		I_Maximo_Promedio = (Sgn_Res) (I_Acumulador >> SHIFT_DIV_BUFFER);			// Calculo el valor medio de corriente
	}
	
	return 0;
		
}

/************************************************************************************
 * 								Fin del Archivo										*
 ************************************************************************************/
