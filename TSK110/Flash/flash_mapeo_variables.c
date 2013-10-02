
/***********************************************************************************	
 * @file       flash_mapeo_variables.c
 * @addtogroup Flash
 * @{
 ***********************************************************************************/
/****************************************************************************************************************
 * 												Inclusión de archivos											*
 ****************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include "StrBus/strbus.h"
#include "Flash/flash_mapeo_variables.h"

/****************************************************************************************************************
 * 											Prototipos de Funciones												*
 ****************************************************************************************************************/

int RecuperarVariablesFlash(void);										// Leo la flash y escribo su contenido en las variables de la RAM

/****************************************************************************************************************
 * 												Variables Globales												*
 ****************************************************************************************************************/

uint16_t modo_establecido;

/****************************************************************************************************************
 * 											Implementación de Funciones											*
 ****************************************************************************************************************/

/******************************************************************************
 * @brief  	Recupera todas las variables guardadas en flash
 * 
 * @param 	none
 *   
 * @return	0 -> success
 ******************************************************************************/

int RecuperarVariablesFlash(void)
{
	uint16_t* modo_establecido_ptr;
	
	int32_t* Pendiente_Resistencia_ptr;
	int32_t* Offset_Resistencia_ptr;
	
	int32_t* Pendiente_Conductividad_ptr;
	int32_t* Offset_Conductividad_ptr;
	
	uint8_t* alarma_activa_ptr;
	int16_t* nivel_alarma_ptr;
	
	modo_establecido_ptr 		= (uint16_t*)MODO_ESTABLECIDO_ADDRESS;						// Guardo la dirección de la variable en flash
	modo_establecido 			= (uint16_t) (*modo_establecido_ptr);							// Guardo lo apuntado en la flash
	
	Pendiente_Resistencia_ptr 	= (int32_t*) PENDIENTE_RESISTENCIA_ADDRESS;
	Pendiente_Resistencia 		= (int32_t) (*Pendiente_Resistencia_ptr);
	
	Offset_Resistencia_ptr 		= (int32_t*) OFFSET_RESISTENCIA_ADDRESS;
	Offset_Resistencia 			= (int32_t) (*Offset_Resistencia_ptr);

	Pendiente_Conductividad_ptr	= (int32_t*) PENDIENTE_CONDUCTIVIDAD_ADDRESS;
	Pendiente_Conductividad 	= (int32_t) (*Pendiente_Conductividad_ptr);
	
	Offset_Conductividad_ptr 	= (int32_t*) OFFSET_CONDUCTIVIDAD_ADDRESS;
	Offset_Conductividad 		= (int32_t) (*Offset_Conductividad_ptr);
	
	alarma_activa_ptr			= (uint8_t*) ALARMA_ACTIVA_ADDRESS;
	terminal.alarma_activa		= (uint8_t) (*alarma_activa_ptr);
	
	nivel_alarma_ptr			= (int16_t*) NIVEL_ALARMA_ADDRESS;
	terminal.nivel_alarma		= (int16_t) (*nivel_alarma_ptr);
		
	return 0;
}

/****************************************************************************************************************
 *@}											Fin del archivo 												*
 ****************************************************************************************************************/
