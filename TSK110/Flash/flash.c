/****************************************************************************************************************
 * 
 * @file       flash.c
 * 
 * @addtogroup Flash
 * 
 * @{
 * 
 ****************************************************************************************************************/

/****************************************************************************************************************
 * 												Inclusión de Archivos											*
 ****************************************************************************************************************/
 
#include <msp430f5529.h>
#include <stdint.h>

#include "Flash/flash.h"
#include "Flash/flash_mapeo_variables.h"


/****************************************************************************************************************
 * 											Prototipos de funciones												*
 ****************************************************************************************************************/

void Escribir_Byte_en_Flash(uint8_t Data, uint16_t Address);
void Escribir_Word_en_Flash(uint16_t Data, uint16_t Address);
void Escribir_DWord_en_Flash(uint32_t Data, uint16_t Address);

/****************************************************************************************************************
 * 											Implementación de funciones											*
 ****************************************************************************************************************/

/******************************************************************************
 * @brief  Escribe un 1 byte en flash, alineado en direccion par
 * @param  Data to be written, and offset
 * @return none
 ******************************************************************************/

void Escribir_Byte_en_Flash(uint8_t Data, uint16_t Address)
{
    uint8_t * Flash_ptr;                   								// Puntero que direcciona memoria de la flash
    static uint8_t Flash_Contents[VARIABLES_POR_BLOQUE * 2];       		// Array con toda la información de un bloque
    uint16_t i;															// Índice del for

    Flash_ptr = (uint8_t *)(MEMORIA_BASE);								// Inicializo el puntero al bloque de memoria seteado por la constante "MEMORIA_BASE"

    for (i = 0; i < (VARIABLES_POR_BLOQUE * 2); i++)                	// Recorro todo el bloque de memoria
    {
        Flash_Contents[i] = *Flash_ptr;     							// Guardo en el array lo apuntado por el puntero
        if (Flash_ptr == (uint8_t*) Address)							// Pregunto si llegué a la zona de memoria que quiero escribir
            Flash_Contents[i] = Data;									// Si llegué, escribo el dato pasado como parámetro
        *Flash_ptr++;													// Incremento el puntero de la flash
    }

    Flash_ptr = (uint8_t *)(MEMORIA_BASE);								// Vuelvo a inicializar el puntero hacia el comienzo del bloque elegido
    FCTL3 = FWKEY;                          							// Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  							// Set Erase bit
    *Flash_ptr = 0;                         							// Dummy write to erase Flash seg
    FCTL1 = FWKEY + WRT;                    							// Set WRT bit for write operation

    for (i = 0; i < (VARIABLES_POR_BLOQUE * 2); i++)					// Vuelvo a recorrer el bloque de memoria elegido
    {
        *Flash_ptr++ = Flash_Contents[i];   							// Descargo el buffer leído y modificado hacia la memoria flash
    }

    FCTL1 = FWKEY;                          							// Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   							// Set LOCK bit.
}

/******************************************************************************
 * @brief  Escribe un word(2 bytes) en flash
 * @param  Data to be written, and offset
 * @return none
 ******************************************************************************/

void Escribir_Word_en_Flash(uint16_t Data, uint16_t Address)
{
    uint16_t * Flash_ptr;                   							// Puntero que direcciona memoria de la flash
    static uint16_t Flash_Contents[VARIABLES_POR_BLOQUE];       		// Array con toda la información de un bloque
    uint8_t i;															// Índice del for

    Flash_ptr = (uint16_t *)(MEMORIA_BASE);								// Inicializo el puntero al bloque de memoria seteado por la constante "MEMORIA_BASE"

																		// LEO TODO UN BLOQUE DE MEMORIA:
    for (i = 0; i < VARIABLES_POR_BLOQUE; i++)                			// Recorro todo el bloque de memoria
    {
        Flash_Contents[i] = *Flash_ptr;     							// Guardo en el array lo apuntado por el puntero
        if (Flash_ptr == (uint16_t *)Address)							// Pregunto si llegué a la zona de memoria que quiero escribir
            Flash_Contents[i] = Data;									// Si llegué, escribo el dato pasado como parámetro
        *Flash_ptr++;													// Incremento el puntero de la flash
    }

    Flash_ptr = (uint16_t *)(MEMORIA_BASE);								// Vuelvo a inicializar el puntero hacia el comienzo del bloque elegido
    FCTL3 = FWKEY;                          							// Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  							// Set Erase bit
    *Flash_ptr = 0;                         							// Dummy write to erase Flash seg
    FCTL1 = FWKEY + WRT;                    							// Set WRT bit for write operation

    for (i = 0; i < VARIABLES_POR_BLOQUE; i++)							// Vuelvo a recorrer el bloque de memoria elegido
    {
        *Flash_ptr++ = Flash_Contents[i];   							// Descargo el buffer leído y modificado hacia la memoria flash
    }

    FCTL1 = FWKEY;                          							// Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   							// Set LOCK bit.
}

/******************************************************************************
 * @brief  Writes the settings to flash
 * @param  Data to be written, and offset
 * @return none
 ******************************************************************************/

void Escribir_DWord_en_Flash(uint32_t Data, uint16_t Address)
{
    uint32_t * Flash_ptr;                   							// Puntero que direcciona memoria de la flash
    static uint32_t Flash_Contents[VARIABLES_POR_BLOQUE / 2];      		// Array con toda la información de un bloque
    uint16_t i;															// Índice del for

    Flash_ptr = (uint32_t*)(MEMORIA_BASE);								// Inicializo el puntero al bloque de memoria seteado por la constante "MEMORIA_BASE"

    for (i = 0; i < (VARIABLES_POR_BLOQUE / 2); i++)                	// Recorro todo el bloque de memoria
    {
        Flash_Contents[i] = *Flash_ptr;     							// Guardo en el array lo apuntado por el puntero
        if (Flash_ptr == (uint32_t *)Address)							// Pregunto si llegué a la zona de memoria que quiero escribir
            Flash_Contents[i] = Data;									// Si llegué, escribo el dato pasado como parámetro
        *Flash_ptr++;													// Incremento el puntero de la flash
    }

    Flash_ptr = (uint32_t *)(MEMORIA_BASE);								// Vuelvo a inicializar el puntero hacia el comienzo del bloque elegido
    FCTL3 = FWKEY;                          							// Clear Lock bit
    FCTL1 = FWKEY + ERASE;                  							// Set Erase bit
    *Flash_ptr = 0;                         							// Dummy write to erase Flash seg
    FCTL1 = FWKEY + WRT;                    							// Set WRT bit for write operation

    for (i = 0; i < (VARIABLES_POR_BLOQUE / 2); i++)					// Vuelvo a recorrer el bloque de memoria elegido
    {
        *Flash_ptr++ = Flash_Contents[i];   							// Descargo el buffer leído y modificado hacia la memoria flash
    }

    FCTL1 = FWKEY;                          							// Clear WRT bit
    FCTL3 = FWKEY + LOCK;                   							// Set LOCK bit.
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
