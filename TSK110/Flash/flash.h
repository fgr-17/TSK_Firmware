/***************************************************************************//**
 * @file       flash.h
 * @addtogroup Flash
 * @{
 ******************************************************************************/
 
#ifndef SETTINGS_H
#define SETTINGS_H

/************************************************************
 * 				Declaración de constantes					*
 ************************************************************/

/************************************************************
 * 				Memory Organization MSP430F5529				*
 ************************************************************
 * Information Memory (flash)								*
 * 															*
 * Info A - 128B (0019FFh - 001980h)						*
 * Info B - 128B (00197Fh - 001900h)						*
 * Info C - 128B (0018FFh - 001880h)						*
 * Info D - 128B (00187Fh - 001800h)						*
 ************************************************************/

// Ver "MSP430F551x/MSP430F552x datasheet", pág.20. 2009-2010 TI.

// Declaración de los segmentos de memoria de información de la flash 
 
#define INFO_A_FIN							0x0019FF
#define	INFO_A_BASE							0x001980

#define	INFO_B_FIN							0x00197F
#define	INFO_B_BASE							0x001900

#define	INFO_C_FIN 							0x0018FF
#define	INFO_C_BASE							0x001880

#define	INFO_D_FIN 							0x00187F
#define	INFO_D_BASE							0x001800


// Declaración de constantes asociadas a la memoria:

#define	TAMANIO_DATO						2									// Tamaño mínimo de una variable, en bytes
#define TAMANIO_BLOQUE						128									// Tamaño de cada bloque de memoria, en bytes 

#define VARIABLES_POR_BLOQUE				(TAMANIO_BLOQUE/TAMANIO_DATO)		// Cantidad máxima de variables por bloque

/************************************************************
 * 					Funciones globales						*
 ************************************************************/

extern void Escribir_Byte_en_Flash(uint8_t Data, uint16_t Address);				// Escribo una variable de 8 bits en flash
extern void Escribir_Word_en_Flash(uint16_t Data, uint16_t Address);			// Escribo 2 bytes en una dirección de memoria en flash
extern void Escribir_DWord_en_Flash(uint32_t Data, uint16_t Address);			// Escribo una variable de 32 bits

#endif /* SETTINGS_H */
/********************************************************************************
 * @}																			*
 ********************************************************************************/
