/********************************************************************************************************
 * 																										*
 * 	@file log.h																							*
 * 																										* 
 *	@brief Header del archivo log.c																		*
 * 																										*
 * 	@version 1.0																						*
 * 																										*
 * 	@author RUIZ, Maximiliano H. (maximiliano.hernan.ruiz@gmail.com)									*
 * 																										*
 * 	@date 06/2011																						*
 * 																										*
 ********************************************************************************************************/
 
#ifndef LOG_H_
#define LOG_H_

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/
#include "FatFs/ff.h"
#include "StrBus/strbus.h"

/********************************************************************************************************
 * 										Definición de constantes										*
 ********************************************************************************************************/
 
#define ArchivoGeneral 								"info.txt"
#define root 										"Datos"
#define EXTENSION_ARCHIVO_LOG						".txt"

#define CONDUCTIVIDAD_A_CANTIDAD_DECIMALES			0
#define TEMPERATURA_A_CANTIDAD_DECIMALES			1
#define CONDUCTIVIDAD_B_CANTIDAD_DECIMALES			0
#define TEMPERATURA_B_CANTIDAD_DECIMALES			1

#define FILENAME_LEN								20

/********************************************************************************************************
 * 											Declaración de tipos										*
 ********************************************************************************************************/

typedef struct t_stream_archivo
{
				FRESULT rc;																			// Respuesta de f_open
			    FATFS fatfs;																		// Objeto para montar el dispositivo de almacenamiento
			    FIL fil;																			// Objeto de archivo
			    
			    char fileName[FILENAME_LEN];														// Nombre del archivo apuntado
} T_Stream_Archivo;
 
 
 
/********************************************************************************************************
 * 										Prototipos de funciones	externas								*
 ********************************************************************************************************/

extern int Log_Datos_Binarios(char* TipoInfo, char* Info); 
extern int Archivo_Log_Datos_Binario(void);

extern int ArchivoLogGeneral(char* TipoInfo, char* Info);
extern int ArchivoLogDatos(void);
extern int Leer_Stream_Archivo(void);
extern int Init_Leer_Stream_Archivo(void);

/********************************************************************************************************
 * 											Variables globales											*
 ********************************************************************************************************/

extern T_Stream_Archivo stream_archivo;

#endif /*LOG_H_*/

/******************************************************************************************************** 	
 * 											Final del Archivo											*
 ********************************************************************************************************/
