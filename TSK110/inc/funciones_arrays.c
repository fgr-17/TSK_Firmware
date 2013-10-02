/********************************************************************************************************
 *
 *  @file 	funciones_arrays.c
 * 
 * 	@brief 	Funciones básicas de manejo de arrays.
 * 
 * 	@author Roux Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 		09/2013
 * 	@company	NEROX SRL
 * 
 ********************************************************************************************************/

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/

#include <stdint.h>

/********************************************************************************************************
 * 										Prototipos de funciones											*
 ********************************************************************************************************/

int CopiarArray_8_bits(uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);			// strcpy de 8 bits
int CopiarArray_16_bits(uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);			// strcpy de 16 bits

int CompararArray_8_bits(const uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);	// strcmp de 8 bits
int CompararArray_16_bits(const uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);	// strcmp de 16 bits

/********************************************************************************************************
 * 										Implementación de funciones										*
 ********************************************************************************************************/

/************************************************************************************************************
 *	@brief 		Copia un array de 16 bits a otro de 8 bits. 			
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CopiarArray_16_bits(uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
	{
		array_destino[i] = array_origen[i];
	}
	
	return 0;	
}

/************************************************************************************************************
 *	@brief 		Compara dos array de 8 bits y devuelve la diferencia del primer
 *  @brief		elemento no coincidente entre los 2
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CompararArray_8_bits(const uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
		if(array_destino[i] != array_origen[i])
			return (array_destino[i] - array_origen[i]);

	return 0;
}

/************************************************************************************************************
 *	@brief 		Compara dos array de 16 bits y devuelve la diferencia del primer
 *  @brief		elemento no coincidente entre los 2
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CompararArray_16_bits(const uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
		if(array_destino[i] != array_origen[i])
			return (array_destino[i] - array_origen[i]);

	return 0;
}

/************************************************************************************************************
 *	@brief 		Copia un array de 8 bits a otro de 8 bits. 			
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CopiarArray_8_bits(uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
	{
		array_destino[i] = array_origen[i];
	}
	
	return 0;
}

/************************************************************************************************************
 * 											Final del Archivo												*
 ************************************************************************************************************/
