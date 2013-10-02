/****************************************************************************************************************
 * 
 *	@file calibrar_temperatura.h
 * 
 * 	@brief header de la máquina de estados de Calibración de Temperatura
 * 
 *  @version 1.0
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 01/2012
 * 
 ****************************************************************************************************************/

#ifndef CALIBRAR_TEMPERATURA_H_
#define CALIBRAR_TEMPERATURA_H_

/****************************************************************************************************************
 * 												Definición de Tipos												*
 ****************************************************************************************************************/

#define CALIBRAR_TEMPERATURA_ESTADOS_BASE				0x22
typedef enum { 	
				ESPERO_PENDIENTE = CALIBRAR_TEMPERATURA_ESTADOS_BASE,
				ESPERO_OFFSET,
				INACTIVO_ERROR,
				INACTIVO_OK
			 } T_Calibrar_Temperatura_Estados;
#define CALIBRAR_TEMPERATURA_ESTADO_INICIAL				ESPERO_PENDIENTE

/****************************************************************************************************************
 * 												Variables Globales												*
 ****************************************************************************************************************/


#endif /*CALIBRAR_TEMPERATURA_H_*/

/****************************************************************************************************************
 *	 											Final del Archivo												*
 ****************************************************************************************************************/

