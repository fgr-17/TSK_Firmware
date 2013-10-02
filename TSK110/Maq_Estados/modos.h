/****************************************************************************************
 *
 * 	Archivo : modos.h
 * 
 * 	Descripción: 	header del archivo "modos.c"
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 11/2011
 * 
 ****************************************************************************************/

#ifndef MODOS_H_
#define MODOS_H_

/****************************************************************************************
 * 								Declaración de constantes								*
 ****************************************************************************************/

#define CADENA_HORA_IND_HOUR					1
#define CADENA_HORA_IND_MIN						2
#define CADENA_HORA_IND_SEC						3

#define CADENA_TEMPERATURA_IND_PARTE_ALTA		1
#define CADENA_TEMPERATURA_IND_PARTE_BAJA		2

// Constantes para leer datos

#define LEER_DATO_IDENTIFICADOR_IND				0
#define LEER_DATO_BYTE3							1
#define LEER_DATO_BYTE2							2
#define LEER_DATO_BYTE1							3
#define LEER_DATO_BYTE0							4

#define LEER_DATO_IDENTIFICADOR					'd'
	
#define CANT_VARIABLES_VALIDAS					2
#define CANT_PARAMETROS_VALIDOS					3

#define LONG_SEPARADOR_NOMBRES			4

/****************************************************************************************
 * 								Definición de tipos										*
 ****************************************************************************************/
 
typedef union {
	uint8_t bytes[2];
	uint16_t word;
} T_Bytes_a_Word;

typedef union {
	uint8_t bytes[4];
	uint32_t dword;
} T_Bytes_a_DWord;

typedef enum {
	ENVIAR_FECHA,
	ENVIAR_HORA,
	ENVIAR_PENDIENTE,
	ENVIAR_OFFSET,
	ENVIAR_DATOS,
	ENVIAR_TEMPERATURA,
	ENVIAR_CORRIENTE,
	ENVIAR_TENSION
} T_Modo_Envio_Datos;

/****************************************************************************************
 * 									Funciones Externas									*
 ****************************************************************************************/

extern int Modos_Inicializar(void);
extern int Modos_Maq_Estados(void);

#endif /*MODOS_H_*/
/****************************************************************************************
 * 									Fin del Archivo										*
 ****************************************************************************************/
