/********************************************************************************************************************
 *	@file hardware_macros.h																							*
 * 																													*
 * 	@brief Header del archivo "hardware_macros.c"																			*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 11/2011																									*
 ********************************************************************************************************************/

#ifndef HARDWARE_MACROS_H_
#define HARDWARE_MACROS_H_

/********************************************************************************************************************
 * 											DEFINICIÓN DE CONSTANTES												*
 ********************************************************************************************************************/

#define DIR_ENTRADA 		0					// Asigno un bit como entrada
#define DIR_SALIDA 			1					// Asigno un bit como salida

#define FUNCION_IO			0					// El pin es entrada/salida				
#define FUNCION_PERIF		1 					// El pin tiene asociada función de periférico

/******************************************************************************************** 	
 * 									Prototipos de funciones	externas						*
 ********************************************************************************************/

extern int Dir_Pin(uint8_t Puerto, uint8_t Bit, uint8_t Dir, uint8_t Funcion);
extern int Activar_Pin(uint8_t Puerto, uint8_t Bit);
extern int Pasivar_Pin(uint8_t Puerto, uint8_t Bit);
extern int Toggle_Pin(uint8_t Puerto, uint8_t Bit);


#endif /*HARDWARE_MACROS_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
