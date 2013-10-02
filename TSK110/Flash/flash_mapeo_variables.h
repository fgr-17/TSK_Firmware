/*******************************************************************************
 * @file		flash.h															
 * @brief		Mapeo de memoria de las variables guardadas en flash. Incluir
 * @brief		después del archivo flash.h
 * 
 * @addtogroup	Flash
 * @{
 *******************************************************************************/

#ifndef FLASH_MAPEO_VARIABLES_H_
#define FLASH_MAPEO_VARIABLES_H_

/****************************************************************************************************************
 * 											Inclusión de archivos												*
 ****************************************************************************************************************/

#include "Flash/flash.h"

/****************************************************************************************************************
 * 											Declaración de constantes											*
 ****************************************************************************************************************/

// Ubicación de las diferentes variables guardadas en flash:

#define MEMORIA_BASE						(INFO_D_BASE)									// Segmento de memoria utilizado. Tiene que apuntar a algún inicio de bloque de la flash
#define MODO_ESTABLECIDO_ADDRESS     		(MEMORIA_BASE)									// Direccion en flash de comienzo de la variable "modo_establecido"

#define PENDIENTE_RESISTENCIA_ADDRESS		(MODO_ESTABLECIDO_ADDRESS + 4)					// Direccion en flash de comienzo de la variable "Pendiente_Resistencia"
#define OFFSET_RESISTENCIA_ADDRESS			(PENDIENTE_RESISTENCIA_ADDRESS + 4)				// Direccion en flash de comienzo de la variable "Offset_Resistencia"

#define PENDIENTE_CONDUCTIVIDAD_ADDRESS		(OFFSET_RESISTENCIA_ADDRESS + 4)				// Direccion en flash de comienzo de la variable "Pendiente_Conductividad"
#define OFFSET_CONDUCTIVIDAD_ADDRESS		(PENDIENTE_CONDUCTIVIDAD_ADDRESS + 4)			// Direccion en flash de comienzo de la variable "Offset_Conductividad"

#define NIVEL_ALARMA_ADDRESS			 	(OFFSET_CONDUCTIVIDAD_ADDRESS + 4)				// Dirección en flash de comienzo de la variable "terminal.nivel_alarma"
#define ALARMA_ACTIVA_ADDRESS				(NIVEL_ALARMA_ADDRESS + 4)						// Dirección en flash de comienzo de la variable "terminal.alarma_activa"

/****************************************************************************************************************
 * 												Funciones externas												*
 ****************************************************************************************************************/

extern int RecuperarVariablesFlash(void);

/****************************************************************************************************************
 * 											Variables Globales Externas											*
 ****************************************************************************************************************/

extern uint16_t modo_establecido;

extern int32_t Pendiente_Resistencia;
extern int32_t Offset_Resistencia;

extern volatile int32_t Pendiente_Conductividad;
extern volatile int32_t Offset_Conductividad;

#endif /*FLASH_MAPEO_VARIABLES_H_*/
/********************************************************************************
 * @}																			*
 ********************************************************************************/
