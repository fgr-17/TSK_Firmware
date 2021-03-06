
/********************************************************************************************************************
 *	@file 	sensores.h																								*
 * 																													*
 * 	@brief	header del archivo "sensores.h"																			*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author	ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 11/2011																									*
 ********************************************************************************************************************/

#ifndef SENSORES_H_
#define SENSORES_H_

/********************************************************************************************************************
 *						 						Definici�n de constantes											*
 ********************************************************************************************************************/

#define CUENTA_LOG_MAX			3																		// Divisor de cuenta del muestreo

/********************************************************************************************************************
 *						 							Funciones globales												*
 ********************************************************************************************************************/

extern int Medicion_Sensores(void);																		// Medici�n seg�n el estado del micro

#endif /*SENSORES_H_*/

/********************************************************************************************************************
 * 												FINAL DEL ARCHIVO													*
 ********************************************************************************************************************/
