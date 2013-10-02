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

#ifndef FUNCIONES_ARRAYS_H_

#define FUNCIONES_ARRAYS_H_

/********************************************************************************************************
 * 										Prototipos de funciones											*
 ********************************************************************************************************/

extern int CopiarArray_8_bits(uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);				// strcpy de 8 bits
extern int CopiarArray_16_bits(uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);			// strcpy de 16 bits

extern int CompararArray_8_bits(const uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);		// strcmp de 8 bits
extern int CompararArray_16_bits(const uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);	// strcmp de 16 bits

#endif /*FUNCIONES_ARRAYS_H_*/

/********************************************************************************************************
 * 									Final del Archivo													*
 ********************************************************************************************************/
