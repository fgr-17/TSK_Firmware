/*******************************************************************************************
 *
 * 	Archivo : xbob_4.c
 * 
 * 	Descripción: software de manejo de la placa controladora de TV XBOB-4
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 12/2011
 * 
 *******************************************************************************************/

#ifndef XBOB_4_H_
#define XBOB_4_H_

/********************************************************************************************************
 * 										Definición de constnates										*
 ********************************************************************************************************/

#define LONGITUD_BUFFER_CARACTERES	64					// Cantidad de caracteres del buffer a transmitir


//////////////////////////////////////////////////////////////////////////
//			Definición de caracteres de comando a transmitir			//
//////////////////////////////////////////////////////////////////////////

#define XBOB4_CSI_0								0x1B							// Primer byte del comando CSI
#define XBOB4_CSI_1								0x5B							// Segundo byte del comando CSI
#define XBOB4_CSI_2								'a'								// Segundo byte del comando CSI

#define LEN_XBOB4_CMD_CSI						3								// Largo [en bytes] del comando CSI

#define XBOB4_CR								0x0D							// Carriage Return
#define XBOB4_LF								0x0A							// Line Feed
#define XBOB4_BS								0x08							// Backspace
#define XBOB4_FF								0x0C							// Form feed

typedef enum {CR, LF, BS, FF} T_Comandos_Simples;								// Comandos simples de 1 byte

#define CANTIDAD_COMANDOS_SIMPLES 				4

#define K1_BYTE_INI								0								// Byte de comienzo de la medición de K1[0..3]
#define T1_BYTE_INI								2								// Byte de comienzo de la medición de T1[4..7]
#define AM_BYTE_INI								8								// Byte de comienzo del flag de alarma 1[8]
#define K2_BYTE_INI								4								// Byte de comienzo de la medición de K2[9..12]
#define T2_BYTE_INI								6								// Byte de comienzo de la medición de T2[13..16]
#define AS_BYTE_INI								9								// Byte de comienzo del flag de alarma 2[17]

#define FRAME_MEDICIONES_LEN					10								// Cantidad de Bytes del frame que contiene las mediciones
#define MEDICIONES_SIZE							2								// Cantidad de mediciones del TSK100

/********************************************************************************************************
 * 										Prototipo de funciones externas									*
 ********************************************************************************************************/

extern void Test_Comunicacion_XBOB_4(void);
extern void Cartel_Inicio(void);

extern int Enviar_Medicion_Binario(void);
extern int Mostrar_Mediciones(void);
extern int Rotulos_Mediciones(void);
extern int Mover_Cursor(char* fila, char* columna);
extern int my_itoa(int numero, char* cadena, uint8_t decimales);
extern int Inicializar_XBOB(void);
							
/********************************************************************************************************
 * 											Variables Globales											*
 ********************************************************************************************************/

extern volatile uint8_t caracteres_xbob_4 [LONGITUD_BUFFER_CARACTERES];

// extern T_Modbus canal_rx_2;
// extern T_Modbus canal_tx_2;


#endif /*XBOB_4_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
