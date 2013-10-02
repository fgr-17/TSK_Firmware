/********************************************************************************************************
 * 																										*
 * 	@file SD_Card.h																							*
 * 																										* 
 *	@brief Header del archivo log.c																		*
 * 																										*
 * 	@version 1.0																						*
 * 																										*
 * 	@author la gente de Texas ;)																		*
 * 																										*
 * 	@date 06/2011																						*
 * 																										*
 ********************************************************************************************************/

#ifndef SDCARD_H
#define SDCARD_H

/********************************************************************************************************
 * 									Inclusión de archivos necesarios									*
 ********************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "msp430.h"
#include "HAL_Board.h"
#include "HAL_SDCard.h"

#include "ff.h"

/********************************************************************************************************
 * 										Definición de constantes										*
 ********************************************************************************************************/

#define NAMES_SIZE  200
#define LINE_SIZE   17
#define PATH_SIZE   50
#define PATH_LENGTHS_SIZE 10
#define NAME_LENGTHS_SIZE 50

/********************************************************************************************************
 * 											Funciones externas											*
 ********************************************************************************************************/
 
extern void SDCard(void);
extern int f_putc (TCHAR, FIL*);

extern FRESULT GetDirectory(char* directoryName);
extern FRESULT GetFile(char* fileName);
extern FRESULT WriteFile(char* fileName, char* text, WORD size);
extern FRESULT AppWriteFile(char* fileName, char* text, WORD size);
extern FRESULT DeleteFileDirectory(char* fileName);
extern FRESULT ExistingFile(char* fileName);
extern FRESULT CreateDirectory(char* DirectoryName);
extern FRESULT ExistingDirectory(char* DirectoryName);

extern int FileSize(char* fileName);

extern void ActiveDirectory(void);
extern void ActiveFile(void);
extern void SDDisplay(uint8_t position);
extern void ClearTempData(void);
extern void die(FRESULT rc);

/********************************************************************************************************
 * 											Variables globales											*
 ********************************************************************************************************/

extern char names[NAMES_SIZE];																		// GetDirectory tira en este buffer todos los items.
extern char output[LINE_SIZE];
extern char path[PATH_SIZE];

extern uint8_t pathLengths[PATH_LENGTHS_SIZE];
extern uint8_t nameLengths[NAME_LENGTHS_SIZE];														// indice de inicio de cada item de "names[...]".

extern uint8_t numNames;
extern uint8_t namesIndex;
extern uint8_t listIndex;
extern uint8_t selectedIndex;
extern uint8_t selectedLength;


#endif /* SDCARD_H */

/********************************************************************************************************
 * 											Final del archivo 											*
 ********************************************************************************************************/
