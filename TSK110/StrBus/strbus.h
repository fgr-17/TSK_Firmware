/********************************************************************************************
 *
 * 	@file 	strbus.h
 * 
 * 	@brief 	header del archivo strbus.h
 * 
 * 	@author Roux, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date	11/2011
 * 
 ********************************************************************************************/
#ifndef STRBUS_H_
#define STRBUS_H_

/********************************************************************************************
 * 									Inclusión de archivos									*
 ********************************************************************************************/ 
 
#include <stdint.h>
#include "inc/tipos_genericos.h"

/********************************************************************************************
 * 									Definición de constantes								*
 ********************************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////
//									CONSTANTES GENERALES									//
//////////////////////////////////////////////////////////////////////////////////////////////
 
#define CANT_BYTES_TAG						1												// Cantidad de bytes usados para tag
#define CANT_BYTES_BUFFER 					5												// Cantidad de bytes de información y pre/sufijos
#define CANT_BYTES_CRC						2												// Cantidad de bytes del CRC
#define LARGO_FRAME							(CANT_BYTES_BUFFER + CANT_BYTES_CRC)			// Cantidad total de bytes del frame

#define LARGO_BUFFER_FRAME 					30												// Longitud total del buffer
#define BYTES_STREAM_ARCHIVO				11												// [h][m][s][k1|k1][t1|t1][k2|k2][t2|t2]

#define FRAME_N								4												// 4 bytes de frame: [CMD][D1][D0][CRC]
// #define FRAME_N								3												// 3 bytes de frame: [CMD][D0][CHK]
#define BUFFER_N							16												// Buffer de recepción

//////////////////////////////////////////////////////////////////////////////////////////////
//									CONSTANTES DEL STRBUS									//
//////////////////////////////////////////////////////////////////////////////////////////////

#define STRBUS_FUN_DUMMY					0xFF
#define STRBUS_FUN_ESCRIBIR_REGISTRO		0x06

//////////////////////////////////////////////////////////////////////////////////////////////
//									PERIODO ENTRE ADQUISICIÓN								//
//////////////////////////////////////////////////////////////////////////////////////////////

#define TS_MODO_MEDIR 						3												// Tiempo entre adquisiciones en modo medir: 3 seg
		
#define TS_MODO_CALIBRACION_T				3												// Tiempo entre adquisiciones durante calibración de temperatura: 3 seg
#define TS_MODO_CALIBRACION_K				3												// Tiempo entre adquisiciones durante calibración de conductividad: 3 seg

//////////////////////////////////////////////////////////////////////////////////////////////
//								RETORNO DE LA TAREA ATENDER_CANAL_RX						//
//////////////////////////////////////////////////////////////////////////////////////////////

#define ATENDER_CANAL_RX_OK					0
#define	ATENDER_CANAL_RX_NO_DATOS			-1
#define ATENDER_CANAL_RX_ERROR_LEER_DATO	-2
#define ATENDER_CANAL_RX_BYTES_PENDIENTE	-3
#define ATENDER_CANAL_RX_FRAME_INCORRECTO	-4

//////////////////////////////////////////////////////////////////////////////////////////////
//								RETORNO DE LA TAREA ATENDER_CANAL_TX						//
//////////////////////////////////////////////////////////////////////////////////////////////

#define ATENDER_CANAL_TX_OK					0
#define ATENDER_CANAL_TX_ENVIANDO			-1
#define ATENDER_CANAL_TX_NO_DATOS			-2
#define ATENDER_CANAL_TX_DESACTIVADO		-3

//////////////////////////////////////////////////////////////////////////////////////////////
//								RETORNO DE LA FUNCION: LEER_DATO_BUFFER						//
//////////////////////////////////////////////////////////////////////////////////////////////

#define LEER_DATO_BUFFER_OK					0
#define LEER_DATO_BUFFER_VACIO				-1
#define LEER_DATO_BUFFER_ERROR				-2

//////////////////////////////////////////////////////////////////////////////////////////////
//							RETORNO DE LA FUNCION: ESCRIBIR_DATO_BUFFER						//
//////////////////////////////////////////////////////////////////////////////////////////////

#define ESCRIBIR_DATO_OK					0
#define ESCRIBIR_DATO_BUFFER_LLENO			-1

//////////////////////////////////////////////////////////////////////////////////////////////
//							RETORNO DE LA FUNCION : COMPROBAR_PUNTERO_FRAME					//
//////////////////////////////////////////////////////////////////////////////////////////////

#define COMPROBAR_PUNTERO_FRAME_OK			0
#define COMPROBAR_PUNTERO_FRAME_ABAJO		-1
#define COMPROBAR_PUNTERO_FRAME_ARRIBA		-2

/********************************************************************************************
 *						 				Tipos de Enumeración								*
 ********************************************************************************************/
 

#define T_TERMINAL_MED_BASE 	0
typedef enum{
				TERMINAL_MIDIENDO = T_TERMINAL_MED_BASE, 
				TERMINAL_LIBRE, 
				TERMINAL_MEDIR
			 } T_Terminal_Med;

typedef enum {
				BUFFER_VACIO, 
				BUFFER_LLENO, 
				BUFFER_CARGANDO
			 } T_Estado_Buffer; 

#define T_RET_LEER_FRAME_BASE 	0x0A
typedef enum {
				FRAME_OK = T_RET_LEER_FRAME_BASE,
				ERROR_CRC,
				ERROR_CMD,
				ERROR_DIRECCION,
				ERROR_FUNCION,
				FRAME_INACTIVO
			 } T_Ret_Leer_Frame;		
		
#define T_NRO_TERMINAL_BASE		0xA0		
typedef enum 	{
				TERMINAL_PC = T_NRO_TERMINAL_BASE,
				TERMINAL_MASTER,
				TERMINAL_SLAVE,
				TERMINAL_DUMMY
				} T_Nro_Terminal;


typedef enum {
				RESPONDER_OK,
				
				RESPONDER_ESTOY,
				
				RESPONDER_ERROR_CRC,
				RESPONDER_ERROR_CMD,
				RESPONDER_ERROR_UBQ,
				RESPODNER_ERROR_UNK,
				RESPONDER_ERROR_CON,
				RESPONDER_ERROR_NOI,
				RESPONDER_ERROR_DAT,							
				
				RESPONDER_HORA,
				
				RESPONDER_T,				
				RESPONDER_R,						
				RESPONDER_K,
				RESPONDER_K_T,
				RESPONDER_DATO,						
				
				RESPONDER_INIT_MEDICION_S,										// Inicializo la medición del slave.
				
				RESPONDER_FRAME_XBOB,											// Envío el frame
				RESPONDER_FRAME_ARCHIVO,
				RESPONDER_FIN_ARCHIVO,
				RESPONDER_CONFIGURACION_SLAVE,
				
				RESPONDER_PC_A_SLAVE,											// El master repite lo que recibe de la PC al Slave
				RESPONDER_SLAVE_A_PC,											// El master repite lo que recibe del Slave a la PC
				
				NO_RESPONDER
} T_Responder;

// ESTE TIPO DEBERÍA ESTAR DEFINIDO EN "modos.h" !!!
	
#define MODOS_ESTADOS_BASE		0x11
typedef enum modos_estados 	{ 	
								CONFIGURACION = MODOS_ESTADOS_BASE,
								
								MASTER_ESPERA_SLAVE,
								MASTER_MIDE,
								
								SLAVE_MIDE,
								
								DATOS_DISPONIBLES,
								PC_RECIBIO_NARCH,
								SELECCION_ARCHIVO,
								DESCARGO_DATOS,
								
								CALIBRACION_K, 
								ESPERO_PENDIENTE_K,
								ESPERO_OFFSET_K, 								
								
								CALIBRACION_T, 
								ESPERO_PENDIENTE_T,
								ESPERO_OFFSET_T,
								
								RECIBE_HORA,
								RECIBE_FECHA,
								RECIBE_ALARMA,
								RECIBE_UMBRAL,
								
								MASTER_REPETIDOR
							
							} T_Modos_Estados;

#define MODOS_ESTADO_INICIAL CONFIGURACION

typedef enum {COMANDO_PENDIENTE, COMANDO_LIBRE} T_estado_cmd;
typedef enum {TERMINAL_DESCONECTADA, TERMINAL_CONECTADA, TERMINAL_ERROR} T_Terminal_Estado_Com;
typedef enum {DATO_VALIDO, DATO_INVALIDO, ERROR_VARIABLE, ERROR_PARAMETRO} T_Estado_Dato;

//////////////////////////////////////////////////////
//			COMANDOS ADMITIDOS DE COM				//
//////////////////////////////////////////////////////

#define MODOS_RET_UBIC			0x54		// Error de comando fuera de contexto
#define MODOS_RET_ERROR			0x55		// Error de comando no reconocido
#define	MODOS_RET_OK			0x56		// Comando procesado correctamente
#define MODOS_RET_DET			0x57		// Maquina de estados detenida

#define MODOS_CMD_OFFSET 		0x0A

typedef enum {
				MODOS_CMD_ESTAS = MODOS_CMD_OFFSET,
				
				MODOS_CMD_CONFIGURACION,
				
				MODOS_CMD_INIT_MASTER,	
				MODOS_CMD_INIT_MEDICION_M,
				MODOS_CMD_FIN_MEDICION_M,
				
				MODOS_CMD_INIT_MEDICION_S,
				MODOS_CMD_FIN_MEDICION_S,
				
				MODOS_CMD_INIT_CALIBRACION_K,
				MODOS_CMD_FIN_CALIBRACION_K,
				MODOS_CMD_CANCEL_CALIBRACION_K,
				MODOS_CMD_OK_CALIBRACION_K,
				MODOS_CMD_FAIL_CALIBRACION_K,
				
				MODOS_CMD_INIT_CALIBRACION_T,
				MODOS_CMD_FIN_CALIBRACION_T,
				MODOS_CMD_CANCEL_CALIBRACION_T,
				MODOS_CMD_OK_CALIBRACION_T,
				MODOS_CMD_FAIL_CALIBRACION_T,
				
				MODOS_CMD_ARCHIVO_OK,
				
				MODOS_CMD_INIT_DESCARGA,
				
				MODOS_CMD_INIT_SETEAR_F_Y_H,
				
				MODOS_CMD_RECIBIR_UMBRAL,
				MODOS_CMD_RECIBIR_ALARMA,

				MODOS_CMD_INI_REPETIDOR,
				MODOS_CMD_FIN_REPETIDOR,
												
				MODOS_CMD_DUMMY,		

				MODOS_CMD_DATO,
				
				MODOS_CMD_PC_A_SLAVE,
				MODOS_CMD_SLAVE_A_PC
				
			 } T_Modos;	

#define MODOS_CMD_INICIAL		MODOS_CMD_DUMMY
	
typedef enum {
				UART0,
				UART1
			} T_Uart;	


#define CAMPOS_N	3

typedef struct{
	uint8_t cmd;
	uint8_t	d1;
	uint8_t	d0;
	uint8_t	chk;											// Checksum
} t_cframe;

typedef union{
	uint8_t  	byte_frame [CAMPOS_N];
	t_cframe 	campo_frame;
} t_uframe;

	
/****************************************************
 * 				Tipos de unión 						*
 ****************************************************/	

typedef union
{
	uint16_t	crc16_word;
	uint8_t		crc16_nibbles[2];
} U_CRC;


#define CMD_N				26														// Cantidad de comandos definidos

#define CMD_OFFSET			0x0F													// Offset inicial del orden de los comandos

typedef enum {
			ESTAS = CMD_OFFSET, 													// Inicio de la comunicación
			
			CONFI,																	// Modo de configuración														
			
			INIMA, 																	// Inicializo el equipo como master
			iNIMS, 																	// Inicializo la medición del master --> ES LA RESPUESTA OK DEL SLAVE A "inims"
			FINMM,																	// Finalizo la medición del master
			
			INIMS, 																	// Inicializo la medición del slave
			FINMS,																	// Finalizo la medición del slave
						
			INICK,																	// Inicio la calibración de conductividad
			FINCK,																	// Finalizo la calibración de conductividad
			CANCK, 																	// Cancelo la calibración de conductividad
			OKACK, 																	// La calibración de conductividad está OK
			FAICK,																	// La calibración de conductividad falló
	
			INICT, 																	// Inicio la calibración de temperatura
			FINCT, 																	// Finalizo la calibración de temperatura
			CANCT, 																	// Cancelo la calibración de temperatura
			OKACT, 																	// La calibración de temperatura está OK 
			FAICT, 																	// La calibración de temperatura falló
			
			ARCOK,																	// La PC confirmó haber recibido un dato
			
			INIDE,																	// Inicio la descarga de datos de archivo (logger) 
			SEFYH,																	// Seteo fecha y hora al equipo
			
			REUMB,																	// Recibo nivel de alarma 
			REALA,																	// Recibo nivel de umbral
			
			INIRE, 																	// Inicializo al Master como repetidor
			FINRE,																	// Finalizo al Master como repetidor
			
			DATOK,
			DATOT,

			DUMMY																	
} t_Byte_Cmd;

/****************************************************
 * 				Tipos de estructura					*
 ****************************************************/

typedef struct 
{
//	uint8_t				frame  [LARGO_BUFFER_FRAME];
	uint8_t				frame_temp[LARGO_BUFFER_FRAME];
	
	uint8_t 			ind;

	uint8_t				len_cadena;
	
	uint8_t				Direccion;
	uint8_t				Funcion;
	
	int32_t				Dato;	
	
	int16_t				dato_1;
	int16_t				dato_2;
	
	
	// uint8_t				comando;
	t_Byte_Cmd			comando;
	

	U_CRC				crc16;
	T_Estado_Buffer		estado_buffer;
	T_Estado_Buffer		estado_buffer_anterior;	
	T_Ret_Leer_Frame 	estado_frame_procesado;
	T_Estado_Dato		estado_dato;
	T_Responder			respuesta;

	// CAMPOS NUEVOS DESD TSK110
	t_bool				dato_pendiente;								// Flag de dato pendiente
	t_bool				envio_activo;								// Flag de envío
	uint8_t				bytes_no_procesados;

	uint8_t				frame [FRAME_N];							// Frame de procesamiento
	uint8_t				ind_fr;										// Indice del frame
	uint8_t*			fp;											// Puntero al frame

	uint8_t				buffer [BUFFER_N];							// Buffer de recepción
	// uint8_t				ind_bf;										// Indice del buffer
	uint8_t				i_fr;										// Indice del frente
	uint8_t				i_fo;										// Indice del fondo

	t_uframe			c_frame;									// Campos definidos en la estructura


} T_Modbus;

typedef struct
{
	T_Nro_Terminal 			Direccion_Equipo;
	T_Modos					Comando;
	T_estado_cmd			Estado_Comando;	
	T_Modos_Estados 		modos_estado;
	T_Terminal_Med			medicion;
	T_Terminal_Estado_Com	estado_com;
	
	uint8_t					alarma_activa;																	// Alarma inactiva si es cero, sino activa
	int16_t					nivel_alarma;																	// Nivel para señalizar la alarma
	
	uint8_t 				modo_diferencial;																// Si es cero, esta en absoluto, sino diferencial
	int16_t					nivel_umbral;																	// nivel para reconocer una diferencia
	
	uint8_t					Ts;																				// Intervalo entre adquisiciones
	
} T_Info_Terminal_Modbus;

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
// 								Tipos de dato nuevos:
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

typedef enum {
			DATO_ENVIO				= 0x00,						// Valor del dato al enviar un comando
			DATO_RECEPCION_OK		= 0xAA,						// Valor del dato al confirmar recepción correcta
			DATO_ERROR_CHK			= 0xFF,						// Valor del dato para señalar error de checksum
			DATO_ERROR_CTX			= 0x55,						// Valor del dato para señalar error de contexto
			DATO_ERROR_DES			= 0x11						// Error desconocido
}T_Modo_Dato;

//////////////////////////////////////////////////////////////////
//						ANALIZAR FRAME
//////////////////////////////////////////////////////////////////

#define ANALIZAR_FRAME_PAQUETE_OK				0
#define ANALIZAR_FRAME_ERROR_CHECKSUM 			-1
#define ANALIZAR_FRAME_ERROR_DES				-2

//////////////////////////////////////////////////////////////////
//						VALIDAR COMANDO
//////////////////////////////////////////////////////////////////

#define VALIDAR_CMD_RECIBIDO_VALIDO		0
#define VALIDAR_CMD_RECIBIDO_ERROR		-1

	
/****************************************************
 * 			Prototipos de funciones 				*
 ****************************************************/

extern int Inicializar_Canal_1 (void);
extern int Inicializar_Canal_2 (void);
extern int Inicializar_Terminal(void);

extern int Inicializar_Canal (T_Modbus*canal);

extern int Procesar_Frame_Recibido(T_Modbus* canal_rx, T_Modbus* canal_tx);									// Seteo el estado del frame recibido
extern int Frame_Respuesta(T_Modbus* ch_out, T_Modbus ch_in, T_Uart salida_UART);			
extern uint16_t CRC16 (const uint8_t *nData, uint16_t wLength);

extern int Tarea_Atender_Canal_Transmision (T_Modbus*canal);
extern int Tarea_Atender_Canal_Recepcion(T_Modbus*canal, T_Modbus*canal_envio);


/****************************************************
 * 				Variables globales 					*
 ****************************************************/

extern T_Modbus canal_rx_0;
extern T_Modbus canal_tx_0;

extern T_Modbus canal_rx_2;
extern T_Modbus canal_tx_2;

extern T_Info_Terminal_Modbus terminal;

#endif /*STRBUS_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
