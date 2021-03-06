/***********************************************************************************************************
 * 
 * 	@file strbus.c
 * 
 * 	@brief Configuraci�n del protocolo de comunicaci�n
 * 
 * 	@version 1.0
 *	
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 11/2011
 * 
 ************************************************************************************************************/

/************************************************************************************************************
 * 											Inclusi�n de archivos											*
 ************************************************************************************************************/
  
#include <msp430f5529.h>
#include <stdint.h>

#include "Maq_Estados/modos.h"
#include "StrBus/strbus.h"

#include "StrBus/cadenas_comunicacion.h"
// #include "StrBus/cadenas_comunicacion_externo.h"
#include "UART/uart.h"

#include "inc/funciones_arrays.h"

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

int Inicializar_Canal_1 (void);																			// Inicializo el canal de comunicacion 1
int Inicializar_Terminal(void);																			// Inicializo la terminal del equipo

int Procesar_Frame_Recibido(T_Modbus* canal_rx, T_Modbus* canal_tx);									// Seteo el estado del frame recibido

int Decodificar_Comando(T_Modbus ch_in);
int Frame_Respuesta(T_Modbus* ch_out, T_Modbus ch_in, T_Uart salida_UART);								// Armo la respuesta en funci�n de lo recibido

int Leer_Frame(T_Modbus* ch_in);																		// Leo el frame recibido

extern T_Modo_Envio_Datos modo_enviar_f_y_h;															// alterna entre env�o de fecha y de hora


// FUNCIONES NUEVAS:
__inline uint8_t Leer_Byte_Frame(T_Modbus*canal);
__inline void Escribir_Byte_Frame(T_Modbus*canal, uint8_t dato_a_escribir);

int Tarea_Atender_Canal_Transmision (T_Modbus*canal);
int Tarea_Atender_Canal_Recepcion(T_Modbus*canal, T_Modbus*canal_envio);

int Leer_Dato_Buffer(T_Modbus*canal, uint8_t*dato_leido);
int Escribir_Dato_Buffer (T_Modbus*canal, uint8_t dato_a_escribir);
int Inicializar_Canal (T_Modbus*canal);
int Cargar_Respuesta (T_Modbus*canal, T_Modo_Dato modo_dato);
int Analizar_Frame(T_Modbus*canal, T_Modbus*canal_envio);

uint8_t Checksum (uint8_t *ptr, uint8_t sz);

/************************************************************************************************************
 * 												Variables globales											*
 ************************************************************************************************************/
 
T_Modbus canal_rx_0;
T_Modbus canal_tx_0;

T_Modbus canal_rx_2;
T_Modbus canal_tx_2;

T_Info_Terminal_Modbus terminal;

/************************************************************************************************************
 * 											Implementaci�n de funciones										*
 ************************************************************************************************************/

/************************************************************************************************************
 *	@brief 		Escribo un dato en el buffer
 *
 * 	@params 	canal : Canal a escribir
 * 	@params		dato_a_escribir :
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Inicializar_Canal (T_Modbus*canal)
{
	//////////////////////////////////////////////
	// 	Inicializaci�n del canal de recepci�n	//
	//////////////////////////////////////////////
	
	canal->estado_buffer 	= BUFFER_VACIO;	
	canal->Dato 			= 0;
	canal->Direccion 		= TERMINAL_DUMMY;
	canal->Funcion 			= STRBUS_FUN_DUMMY;
	canal->ind 				= 0;
	canal->len_cadena		= LARGO_FRAME;

	canal->dato_pendiente = NO;
	canal->envio_activo = NO;
	canal->bytes_no_procesados = 0;
	
	// canal->ind_bf = 0;
	canal->ind_fr = 0;
	canal->fp= canal->frame;
	
	canal->i_fr = 0;								// El puntero del frente se inicializa al inicio
	canal->i_fo = FRAME_N - 1;						// El puntero del fondo se inicializa al final del buffer
	
	return 0;
}

/************************************************************************************************************
 *	@brief 		Escribo un dato en el buffer
 *
 * 	@params 	canal : Canal a escribir
 * 	@params		dato_a_escribir :
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Escribir_Dato_Buffer (T_Modbus*canal, uint8_t dato_a_escribir)
{
	if(canal->estado_buffer == BUFFER_LLENO)					// Pregunto si el buffer ya est� lleno
		return ESCRIBIR_DATO_BUFFER_LLENO;						// Retorno error de buffer lleno

	canal->buffer[canal->i_fr] = dato_a_escribir;				// Escribo el dato en el frente
	canal->i_fr++;												// Incremento el puntero del frente

	if(canal->i_fr >= BUFFER_N)									// Pregunto si el puntero del frente lleg� al final
		canal->i_fr = 0;										// Lo inicializo al comienzo

	if(canal->i_fo >= canal->i_fr)								// Si los �ndices son iguales
		canal->estado_buffer = BUFFER_LLENO;					// es porque el buffer est� lleno

	return ESCRIBIR_DATO_OK;

}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de transmisi�n
 *
 * 	@params 	canal : Canal a leer
 * 	@params		dato_leido : puntero al dato a leer
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Leer_Dato_Buffer(T_Modbus*canal, uint8_t*dato_leido)
{

	if(canal->estado_buffer == BUFFER_VACIO)					// Si el buffer est� vac�o
		return LEER_DATO_BUFFER_VACIO;							// Se�alizo y salgo

	canal->i_fo++;												// Incremento el �ndice del fondo
	if(canal->i_fo >= BUFFER_N)									// Pregunto si llegu� al final
		canal->i_fo = 0;										// Lo vuelvo a poner al principio

	*dato_leido = canal->buffer[canal->i_fo];					// Guardo el dato leido

	if(((canal->i_fo + 1)%BUFFER_N) >= canal->i_fr)				// Condici�n para buffer vac�o
		canal->estado_buffer = BUFFER_VACIO;					// Caso verdadero: se�alizo

	return LEER_DATO_BUFFER_OK;									// Retorno �xito en la lectura
}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de transmisi�n
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Tarea_Atender_Canal_Transmision (T_Modbus*canal)
{
	uint8_t dato_a_enviar;

	if(canal->envio_activo == SI)									// Chequeo el flag de env�o activado
	{
		if(canal->dato_pendiente == NO)								// Pregunto si el canal de salida est� libre
		{															// Si el flag todav�a est� en alto:
			if(!Leer_Dato_Buffer(canal, &dato_a_enviar))			// Extraigo un dato del buffer de salida
			{														// Si pude sacar un dato:
				UCA0TXBUF = dato_a_enviar;							// Escribo el dato a enviar en reg.de salida
			}
			else													// En caso que no haya podido sacar un dato:
			{
				canal->envio_activo = NO;							// Bajo el flag de env�o activo
				return ATENDER_CANAL_TX_NO_DATOS;					// Se�alizo que no tengo m�s datos a enviar
			}
		}
		else
		{															// En caso de no se haya terminado de enviar el dato
			return ATENDER_CANAL_TX_ENVIANDO;						// Se�alizo que no pude enviar el dato
		}
	}
	else															// En caso de que el env�o no est� activado
	{																// Salgo directamente
		return ATENDER_CANAL_TX_DESACTIVADO;						// Y se�alizo
	}
	return 0;
}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de recepci�n
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Tarea_Atender_Canal_Recepcion(T_Modbus*canal, T_Modbus*canal_envio)
{
	if(canal->dato_pendiente == NO)
	{
		return ATENDER_CANAL_RX_NO_DATOS;
	}
	else if (canal->dato_pendiente == SI)
	{
		Timer_A2_Delay(1000, NO_BLOQUEANTE);								// Comienzo la cuenta del timeout
		canal->bytes_no_procesados++;										// Incremento la cantidad de bytes a procesar
		canal->dato_pendiente = NO;											// Bajo el flag de dato pendiente

		if(!Leer_Dato_Buffer(canal, canal->fp ))							// Extraigo un dato del buffer hacia el frame
		{
			canal->fp++;													// Incremento el puntero

			if(canal->fp > canal->frame + FRAME_N)							// Si escrib� el frame
				canal->fp = canal->frame;									// Vuelvo el puntero al inicio

			if(canal->bytes_no_procesados >= FRAME_N)						// Pregunto si ya ocupe un frame
			{
				if(!Analizar_Frame(canal, canal_envio)) 					// Proceso los campos del frame
				{
					return ATENDER_CANAL_RX_OK;								// Retorno �xito
				}
				else
				{
					canal->bytes_no_procesados--;							// Descarto el dato m�s viejo
					return ATENDER_CANAL_RX_FRAME_INCORRECTO;				// El frame no est� bien
				}
			}
			else if(canal->estado_buffer != BUFFER_VACIO)					// Pregunto si el buffer qued� vac�o
			{
				canal->dato_pendiente = SI;									// Vuelvo a poner el flag en alto
				return ATENDER_CANAL_RX_BYTES_PENDIENTE;					// Quedan datos por procesar
			}
		}
		else
		{
			return ATENDER_CANAL_RX_ERROR_LEER_DATO;						// Error al leer datos
		}
	}
	return 0;
}

/************************************************************************************************************
 *	@brief 		Comprueba si el puntero de escritura del frame est� dentro del rango o no. Si est� fuera de
 * 	@brief		rango por arriba, lo pongo en el origen. Si est� fuera de rango por debajo, lo pongo en el 
 * 	@brief		valor final.
 *
 * 	@params 	canal 	Canal correspondiente al frame.
 *
 * 	@returns 	COMPROBAR_PUNTERO_FRAME_OK			El puntero est� en rango, no lo modifico
 * 	@returns	COMPROBAR_PUNTERO_FRAME_ARRIBA		El puntero est� fuera de rango por arriba, lo pongo en valor inicial
 * 	@returns	COMPROBAR_PUNTERO_FRAME_ABAJO		El puntero est� fuera de rango por debajo, lo seteo en el mayor valor
 *
 ************************************************************************************************************/

__inline int8_t Comprobar_Puntero_Frame (T_Modbus*canal)
{
	if(canal->fp >= canal->frame + FRAME_N)									// Pregunto si el puntero no est� por arriba del valor m�ximo
	{
		canal->fp = canal->frame;											// Inicializo el frame en el inicio
		return COMPROBAR_PUNTERO_FRAME_ARRIBA;								// Retorno el error (igualmente ya est� corregido).														
	}
	
	if(canal->fp < canal->frame)											// Pregunto si el puntero est� por debajo
	{
		canal->fp = canal->frame + FRAME_N - 1;								// Seteo en el mayor valor
		return COMPROBAR_PUNTERO_FRAME_ABAJO;								// El puntero estaba por debajo del valor permitido, lo seteo en el mayor.
	}
	
	return COMPROBAR_PUNTERO_FRAME_OK;										// El puntero estaba en rango, no hago ninguna modificaci�n	
}

/************************************************************************************************************
 *	@brief 		Incremento el valor del puntero al frame en forma circular.
 *
 * 	@params 	canal Canal correspondiente al frame.
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline void Inc_Circular_Puntero_Frame (T_Modbus*canal)
{
	canal->fp++;															// Incremento el puntero

	if (canal->fp >= canal->frame + FRAME_N)								// Pregunto si llegu� al final del frame
		canal->fp = canal->frame;											// En ese caso, vuelvo al comienzo

	return;																	// Salgo sin se�alizar nada
}

/************************************************************************************************************
 *	@brief 		Decremento el valor del puntero al frame en forma circular.
 *
 * 	@params 	canal Canal correspondiente al frame.
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline void Dec_Circular_Puntero_Frame (T_Modbus*canal)
{
	canal->fp--;															// Decremento el puntero

	if (canal->fp < canal->frame)											// Pregunto si llegu� al inicio del frame
		canal->fp = canal->frame + FRAME_N - 1;								// En ese caso lo seteo en el final

	return;																	// Salgo sin se�alizar	
}

/************************************************************************************************************
 *	@brief 		Escribe un dato en el frame, despu�s incremento el puntero en forma circular.
 *
 * 	@params 	canal 				Canal correspondiente al frame.
 * 	@params		dato_a_escribir		Dato a escribir en el frame
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline void Escribir_Byte_Frame(T_Modbus*canal, uint8_t dato_a_escribir)
{
	Comprobar_Puntero_Frame(canal);											// Compruebo si el puntero est� en rango
	*(canal->fp) = dato_a_escribir;											// Escribo el dato recibido
	Inc_Circular_Puntero_Frame(canal);										// Incremento el puntero circularmente				
	return;
}

/************************************************************************************************************
 *	@brief 		Procesa el frame recibido: chequea CRC, comprueba el valor del comando y extrae datos si
 *	@brief		si hace falta.
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline uint8_t Leer_Byte_Frame(T_Modbus*canal)
{
	Comprobar_Puntero_Frame(canal);											// Compruebo si el puntero est� en rango
	Dec_Circular_Puntero_Frame(canal);										// Incremento el puntero circularmente
	return *(canal->fp);													// Retorno el dato le�do
}

/************************************************************************************************************
 *	@brief 		Valida un comando recibido
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline int Validar_Cmd_Recibido(uint8_t cmd)
{
	if((cmd >= CMD_OFFSET) && (cmd <= CMD_OFFSET + CMD_N))
		return VALIDAR_CMD_RECIBIDO_VALIDO;
	else
		return VALIDAR_CMD_RECIBIDO_ERROR;
}

/************************************************************************************************************
 *	@brief 		Carga una secuencia de respuesta en el array de salida
 *
 * 	@params 	canal : Canal a escribir
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Cargar_Respuesta (T_Modbus*canal, T_Modo_Dato modo_dato)
{
	t_uframe frame_respuesta;

	frame_respuesta.campo_frame.d0 = (uint8_t) modo_dato;		// Cargo el dato correspondiente para se�alar tipo de respuesta
	frame_respuesta.campo_frame.cmd = canal->comando;			// Vuelvo a cargar el comando recibido
	frame_respuesta.campo_frame.chk = Checksum(frame_respuesta.byte_frame, FRAME_N - 1);	// Calculo y guardo el checksum

	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.chk);	// Cargo checksum
	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.d0);		// Cargo dato
	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.cmd);	// Cargo comando

	canal->envio_activo = SI;									// Pongo en alto el flag de env�o activo
	canal->dato_pendiente = SI;									// Se�alizo que el canal de env�o est� libre

	return 0;
}

/************************************************************************************************************
 *	@brief 		Carga una secuencia de respuesta en el array de salida
 *
 * 	@params 	canal : Canal a escribir
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

typedef enum																				// Encabezado de dato (se�ala qu� dato se est� enviando)
{
	DATO_K,
	DATO_T
}t_Enc_Dato;

int Enviar_Dato (T_Modbus*canal, uint8_t dato)
{
	t_uframe frame_respuesta;

	frame_respuesta.campo_frame.d0 = dato;													// Cargo el dato correspondiente para se�alar tipo de respuesta
	frame_respuesta.campo_frame.cmd = canal->comando;										// Vuelvo a cargar el comando recibido
	frame_respuesta.campo_frame.chk = Checksum(frame_respuesta.byte_frame, FRAME_N - 1);	// Calculo y guardo el checksum

	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.chk);							// Cargo checksum
	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.d0);								// Cargo dato
	Escribir_Byte_Frame(canal, frame_respuesta.campo_frame.cmd);							// Cargo comando

	canal->envio_activo = SI;																// Pongo en alto el flag de env�o activo
	canal->dato_pendiente = SI;																// Se�alizo que el canal de env�o est� libre

	return 0;
}

/************************************************************************************************************
 *	@brief 		Procesa el frame recibido: chequea CRC, comprueba el valor del comando y extrae datos si
 *	@brief		si hace falta
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Analizar_Frame(T_Modbus*canal, T_Modbus*canal_envio)
{
	t_uframe frame_extraido;

	frame_extraido.campo_frame.chk = Leer_Byte_Frame (canal);				// Leo el byte de CRC
	frame_extraido.campo_frame.d0  = Leer_Byte_Frame (canal);				// Leo el byte de D0
	frame_extraido.campo_frame.d1  = Leer_Byte_Frame (canal);				// Leo el byte de D1
	frame_extraido.campo_frame.cmd = Leer_Byte_Frame (canal);				// Leo el byte de CMD

	if(!(Checksum(frame_extraido.byte_frame, FRAME_N)))						// Pregunto si el checksum de la secuencia es v�lido
	{																		// En caso que el frame sea correcto
		if(!Validar_Cmd_Recibido(frame_extraido.campo_frame.cmd))			// Pregunto si el comando es v�lido
		{																	// En caso verdadero
			terminal.Estado_Comando = COMANDO_PENDIENTE;					// Levanto flag de comando pendiente
			
			////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////
			////////////			ENVIO SOBRE EL CANAL DE TRANSMISI�N				////////////			
			////////////////////////////////////////////////////////////////////////////////////
			////////////////////////////////////////////////////////////////////////////////////

			canal_envio->comando = (t_Byte_Cmd) frame_extraido.campo_frame.cmd;				// Copio el frame recibido al canal
			Cargar_Respuesta(canal_envio, DATO_RECEPCION_OK);						// Cargo la respuesta para que sea enviada
			
			
			return ANALIZAR_FRAME_PAQUETE_OK;								// Se�alizo que recib� OK
		}
		else																// En caso que el comando no sea v�lido
		{																	//
			Cargar_Respuesta(canal, DATO_ERROR_DES);						// Error de comando desconocido
			return ANALIZAR_FRAME_ERROR_DES;								// Se�alizo el error
		}
	}
	else
	{
		Cargar_Respuesta(canal, DATO_ERROR_CHK);
		return ANALIZAR_FRAME_ERROR_CHECKSUM;
	}

}

/************************************************************************************************************
 *	@brief 		Calculo el checksum de una secuencia de bytes.
 *
 * 	@params 	ptr		Puntero al inicio de la secuencia de bytes
 * 	@params		sz		Longitud de la secuencia de bytes
 *
 * 	@returns 	Valor del checksum de la secuencia
 *
 ************************************************************************************************************/

uint8_t Checksum (uint8_t *ptr, uint8_t sz)
{
    uint8_t chk = 0;
    while (sz-- != 0)
        chk -= *ptr++;
    return chk;
}

/************************************************************************************************************
 *	@brief 		Inicializo los campos de la estructura "terminal", que contiene
 * 	@brief		todos los datos de conexi�n del equipo.				
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Inicializar_Terminal(void)
{
	terminal.Direccion_Equipo 	= TERMINAL_PC;	
	terminal.Comando 			= MODOS_CMD_INICIAL;
	terminal.Estado_Comando 	= COMANDO_LIBRE;	
	terminal.medicion			= TERMINAL_LIBRE;
	terminal.estado_com 		= TERMINAL_DESCONECTADA;
	terminal.Ts					= TS_MODO_MEDIR;
	
	terminal.alarma_activa 		= 0;
	terminal.modo_diferencial	= 0;
	terminal.nivel_alarma		= 0;
	terminal.nivel_umbral 		= 0;
	terminal.modos_estado 		= CONFIGURACION;
	
	modo_enviar_f_y_h = ENVIAR_FECHA;
	// Modos_Inicializar();
	return 0;
}

/************************************************************************************************************
 *	@brief		Inicializo los campos de la estructura de transmisi�n y recepci�n			
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Inicializar_Canal_1 (void)
{	
	//////////////////////////////////////////////
	// 	Inicializaci�n del canal de recepci�n	//
	//////////////////////////////////////////////
	
	canal_rx_0.estado_buffer 	= BUFFER_VACIO;	
	canal_rx_0.Dato 			= 0;
	canal_rx_0.Direccion 		= TERMINAL_DUMMY;
	canal_rx_0.Funcion 			= STRBUS_FUN_DUMMY;
	canal_rx_0.ind 				= 0;
	canal_rx_0.len_cadena		= LARGO_FRAME;

	//////////////////////////////////////////////
	// 	Inicializaci�n del canal de transmisi�n	//
	//////////////////////////////////////////////
	
	canal_tx_0.estado_buffer 	= BUFFER_VACIO;
	canal_tx_0.Dato				= 0;
	canal_tx_0.Direccion 		= terminal.Direccion_Equipo;
	canal_tx_0.Funcion			= STRBUS_FUN_DUMMY;
	canal_tx_0.ind				= 0;
	canal_tx_0.len_cadena		= 0;
	canal_tx_0.respuesta 		= NO_RESPONDER;
		
	return 0;	
}

/************************************************************************************************************
 *	@brief		Inicializo los campos de la estructura de transmisi�n y recepci�n 2			
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Inicializar_Canal_2 (void)
{	
	//////////////////////////////////////////////
	// 	Inicializaci�n del canal de recepci�n	//
	//////////////////////////////////////////////
	
	canal_rx_2.estado_buffer 	= BUFFER_VACIO;	
	canal_rx_2.Dato 			= 0;
	canal_rx_2.Direccion 		= TERMINAL_DUMMY;
	canal_rx_2.Funcion 			= STRBUS_FUN_DUMMY;
	canal_rx_2.ind 				= 0;
	canal_rx_2.len_cadena		= LARGO_FRAME;

	//////////////////////////////////////////////
	// 	Inicializaci�n del canal de transmisi�n	//
	//////////////////////////////////////////////
	
	canal_tx_2.estado_buffer 	= BUFFER_VACIO;
	canal_tx_2.Dato				= 0;
	canal_tx_2.Direccion 		= terminal.Direccion_Equipo;
	canal_tx_2.Funcion			= STRBUS_FUN_DUMMY;
	canal_tx_2.ind				= 0;
	canal_tx_2.len_cadena		= 0;
	
	canal_tx_2.respuesta 		= NO_RESPONDER;
		
	return 0;	
}

/************************************************************************************************************
 *	@brief 		En caso de estar el buffer de recepci�n, lo proceso y analizo la
 * 	@brief 		validez del mismo, para luego hacer la respuesta.				
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/
/*
int Procesar_Frame_Recibido(T_Modbus* canal_rx, T_Modbus* canal_tx)
{
	if(canal_rx->estado_buffer == BUFFER_LLENO)
	{		
		canal_rx->estado_buffer = BUFFER_VACIO;											// Libero el canal para no volver a procesarlo	
		Leer_Frame(canal_rx);															// Proceso el frame recibido
		
		if((terminal.modos_estado == MASTER_REPETIDOR)&&(canal_rx->comando != MODOS_CMD_FIN_REPETIDOR))//  && (canal_rx->comando != MODOS_CMD_CONFIGURACION))
		{
			terminal.estado_com = TERMINAL_CONECTADA;									// Desconecto para no mandar hora
			terminal.Estado_Comando = COMANDO_PENDIENTE;								// El comando recibido queda pendiente para procesar.
			
			if(canal_rx == &canal_rx_0)
				terminal.Comando = MODOS_CMD_PC_A_SLAVE;
			else if(canal_rx == &canal_rx_2)
				terminal.Comando = MODOS_CMD_SLAVE_A_PC;
				
			return 0;
		}
		
		switch(canal_rx->estado_frame_procesado)										// Eval�o el resultado del frame procesado
		{
		case FRAME_OK:																	// Todos los campos fueron correctos
			
			terminal.Comando = (T_Modos) canal_rx->comando;								// Actualizo el comando a procesar			
			
			if(terminal.Comando == MODOS_CMD_DATO)										// Pregunto si es un dato
			{
				if(canal_rx == &canal_rx_0)
					terminal.Estado_Comando = COMANDO_PENDIENTE;						// El comando recibido queda pendiente para procesar.
				else if(terminal.modos_estado == MASTER_MIDE)
					terminal.Estado_Comando = COMANDO_PENDIENTE;
			}
			else if(terminal.Comando == MODOS_CMD_ESTAS)								// Inicializaron el Software de PC
			{
				Inicializar_Terminal();													// Inicializo la estructura de la terminal			
				terminal.modos_estado = CONFIGURACION;									// Lo tomo como si fuera un reset									
				canal_tx->respuesta = RESPONDER_OK;										// Respondo OK desde el Slave al Master
				terminal.Estado_Comando = COMANDO_LIBRE;								// No hay ning�n comando para procesar.
				terminal.estado_com = TERMINAL_CONECTADA;								// Desconecto para no mandar hora
			}
			else if(terminal.Comando == MODOS_CMD_INIT_MEDICION_S)						// Si quiero inicializar el esclavo, no hace falta conectar
			{
				terminal.modos_estado = CONFIGURACION;									// Salto autom�ticamente al estado previo a cambiar
				terminal.estado_com = TERMINAL_DESCONECTADA;							// Desconecto para no mandar hora
				terminal.Estado_Comando = COMANDO_PENDIENTE;							// Se�alizo comando pendiente.
				canal_tx->respuesta = RESPONDER_OK;										// Respondo OK desde el Slave al Master
				canal_tx->estado_buffer = BUFFER_VACIO;									// Le doy prioridad a este comando, borro lo que estaba mandando		
			}
			else
			{
				terminal.estado_com = TERMINAL_CONECTADA;								// Desconecto para no mandar hora
				terminal.Estado_Comando = COMANDO_PENDIENTE;							// El comando recibido queda pendiente para procesar.	
				canal_tx->respuesta = RESPONDER_OK;										// Respondo comando OK a la PC									
			}
			
			if(terminal.Comando == MODOS_CMD_CONFIGURACION) 		
			{
				Inicializar_Terminal();
				terminal.estado_com = TERMINAL_CONECTADA;
				
				if(canal_rx == &canal_rx_0)
					canal_tx_2.respuesta = RESPONDER_CONFIGURACION_SLAVE;	
			}
			
			break;
			
		case ERROR_CRC:						
			terminal.Estado_Comando = COMANDO_LIBRE;								// Al haber error, no proceso nada
			canal_tx->respuesta 	= RESPONDER_ERROR_CRC;							// Respondo con la cadena de "ercrc" 				
			break;
			
		case ERROR_CMD:
			terminal.Estado_Comando = COMANDO_LIBRE;								// Comando no reconocido, no se procesa
			canal_tx->respuesta 	= RESPONDER_ERROR_CMD;							// Respondo con la cadena de "ercmd"
			break;
					
		default:	
			terminal.Estado_Comando = COMANDO_LIBRE;								// Comando no reconocido, no se procesa
			canal_tx->respuesta 	= RESPODNER_ERROR_UNK;							// Respondo con la cadena de "ercmd"
			break;
		}
				
	}
	
	return 0;
}
*/

/************************************************************************************************************
 *	@brief		Chequeo la validez del CRC y del comando recibido.					
 * 
 * 	@params 	ch_in : Canal a leer.
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

/*
int Leer_Frame(T_Modbus* ch_in)
{
	// ch_in->crc16.crc16_word = CRC16(ch_in->frame, ch_in->len_cadena);			// Hago el calculo de CRC sobre el frame recibido. Si es cero est� OK

	if(ch_in->crc16.crc16_word != 0)											// Pregunto si el frame recibido tiene el CRC ok
	{
		ch_in->estado_frame_procesado = ERROR_CRC;								// Si el resultado no es cero, devuelvo "ERROR_CRC"
		return 1;
	}
	
	if((ch_in->comando = Decodificar_Comando(*ch_in)) == MODOS_RET_ERROR)
	{
		ch_in->estado_frame_procesado = ERROR_CMD;								// Si el resultado no es cero, devuelvo "ERROR_CRC"
		return 1;
	}
	
	ch_in->estado_frame_procesado = FRAME_OK;
	return 0;
	
}
*/

/************************************************************************************************************
 *	@brief 		Arma la cadena de respuesta al comando recibido.					
 * 
 * 	@params 	ch_out				Canal de salida de la respuesta. 
 *  @params		ch_in				Canal de entrada del mensaje recibido
 *  @params		ret_Maq_Estados		Respuesta de la m�quina de estados a la cadena
 * 
 * 	@returns 	0	responde por la UART seleccionada.
 * 
 ************************************************************************************************************/
/*
int Frame_Respuesta(T_Modbus* ch_out, T_Modbus ch_in, T_Uart salida_UART)
{	
	if(ch_out->estado_buffer == BUFFER_VACIO)												// Si el buffer est� libre listo para enviar datos
	{
		switch(ch_out->respuesta)
		{
		case RESPONDER_OK:
			CopiarArray_8_bits(ch_out->frame, ch_in.frame, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC
			ch_out->frame[0] -= 'a' - 'A';													// Convierto la primer letra a may�scula para armar la respuesta
			break;
			
		case RESPONDER_ESTOY:
			CopiarArray_8_bits(ch_out->frame, cadena_estoy, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC, comienzo comunicaci�n.
			break;
			
		case RESPONDER_ERROR_CRC:
			CopiarArray_8_bits(ch_out->frame, cadena_error_crc, CANT_BYTES_BUFFER);			// Copio la respuesta sin CRC, error en el CRC
			break;
			
		case RESPONDER_ERROR_CMD:
			// CopiarArray_8_bits(ch_out->frame, cadena_error_cmd, CANT_BYTES_BUFFER);			// Copio la respuesta sin CRC, comando no reconocido
			// break;
			return 0;
	
		case RESPONDER_ERROR_UBQ:
			CopiarArray_8_bits(ch_out->frame, cadena_error_ubq, CANT_BYTES_BUFFER);			// Comando fuera de contexto
			break;
	
		case RESPONDER_ERROR_NOI:
			CopiarArray_8_bits(ch_out->frame, cadena_error_noi, CANT_BYTES_BUFFER);			// Copio la respuesta sin CRC, no inicializado	
			break;
	
		case RESPONDER_ERROR_CON:
			CopiarArray_8_bits(ch_out->frame, cadena_error_con, CANT_BYTES_BUFFER);			// Error de conectividad		
			break;
	
		case RESPONDER_ERROR_DAT:
			CopiarArray_8_bits(ch_out->frame, cadena_error_dat, CANT_BYTES_BUFFER);			// Error de dato recibido		
			break;
		
		case RESPODNER_ERROR_UNK:
			CopiarArray_8_bits(ch_out->frame, cadena_error_unk, CANT_BYTES_BUFFER);			// Copio la respuesta sin CRC, error desconocido
			break;
			
		case RESPONDER_HORA:
			CopiarArray_8_bits(ch_out->frame, cadena_hora, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC, respondo con la hora
			break;
			
		case RESPONDER_T:		
			CopiarArray_8_bits(ch_out->frame, cadena_temperatura, CANT_BYTES_BUFFER);		// Copio la respuesta sin CRC, respondo con la temperatura
			break;
		
		case RESPONDER_R:
			CopiarArray_8_bits(ch_out->frame, cadena_resistencia, CANT_BYTES_BUFFER);		// Copio la respuesta sin CRC, respondo con medici�n de R
			break;
			
		case RESPONDER_K:
			CopiarArray_8_bits(ch_out->frame, cadena_conductividad, CANT_BYTES_BUFFER);		// Copio la respuesta sin CRC, respondo medici�n de K
			break;
			
		case RESPONDER_K_T:
			// mando los 2 datos del slave al master
			break;		
			
		case RESPONDER_DATO:
			CopiarArray_8_bits(ch_out->frame, cadena_datos, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC, respondo un dato
			break;
					
		case RESPONDER_INIT_MEDICION_S:														// Copio la respuesta sin CRC, respondo con la temperatura
			CopiarArray_8_bits(ch_out->frame, lista_de_comandos[MODOS_CMD_INIT_MEDICION_S - MODOS_CMD_OFFSET], CANT_BYTES_BUFFER);		
			break;
			
		case RESPONDER_FRAME_XBOB:															// El frame ya lo arme antes. Solo tengo que mandar dato
			break;		
			
		case RESPONDER_FRAME_ARCHIVO:
			// ya tengo el frame armado
			break;
			
		case RESPONDER_FIN_ARCHIVO:
			cadena_archivos[0] = 'f';
			CopiarArray_8_bits(ch_out->frame, (const uint8_t*)cadena_archivos, LARGO_BUFFER_FRAME);
			break;			
					
		case RESPONDER_CONFIGURACION_SLAVE:
			CopiarArray_8_bits(ch_out->frame, lista_de_comandos[MODOS_CMD_CONFIGURACION - MODOS_CMD_OFFSET], CANT_BYTES_BUFFER);
			break;
			
		case RESPONDER_PC_A_SLAVE:
			CopiarArray_8_bits(ch_out->frame, canal_rx_0.frame , CANT_BYTES_BUFFER);		// Copio al canal de salida (PC) lo que recib� del Slave
			break;
			
		case RESPONDER_SLAVE_A_PC:
			CopiarArray_8_bits(ch_out->frame, canal_rx_2.frame , CANT_BYTES_BUFFER);		// Copio al canal de salida (Slave) lo que recib� de la PC
			break;
		
		case NO_RESPONDER:
			return 0;		
	
		default:
			ch_out->respuesta = NO_RESPONDER;
			return 0;				
		}
		
		if((ch_out->respuesta == RESPONDER_FRAME_ARCHIVO)||(ch_out->respuesta == RESPONDER_FIN_ARCHIVO))
		{
			ch_out->len_cadena = BYTES_STREAM_ARCHIVO + CANT_BYTES_TAG;
		}
		else
		{	
			// Concatenar_CRC(ch_out);															// Concateno el CRC al final de la cadena transmitida
			ch_out->len_cadena = LARGO_FRAME;												// El paquete a transmitir es el largo del frame
		}
		
		ch_out->ind = 0;																	// Inicializo el buffer de salida																
		ch_out->estado_buffer = BUFFER_LLENO;												// Lo indico como BUFFER_LLENO
		ch_out->respuesta = NO_RESPONDER;													// Como ya atend� la llamada, no emito m�s respuestas
		
		switch(salida_UART)																	// de acuerdo a la salida seleccionada
		{
			case UART0:
				Iniciar_Transmision_Paquete_UART0(ch_out);									// Inicio la transmisi�n del paquete por UART0
				break;
			case UART1:														
				Iniciar_Transmision_Paquete_UART1(ch_out);									// Inicio la transmisi�n del paquete por UART1
				break;
			default:
				return 1;
		}
		
		return 0;
	}
	else
	{
		return 1;
	}	
}
*/
/************************************************************************************************************
 *	@brief 		Eval�o si el comando es v�lido.					
 * 
 * 	@params 	ch_in	Canal de entrada a evaluar
 * 
 * 	@returns 	MODOS_RET_ERROR		No se encontr� el comando
 *  @returns	Si encontr� el comando, devuelvo el valor.
 * 
 ************************************************************************************************************/
/*
int Decodificar_Comando(T_Modbus ch_in)
{	
	uint8_t i;
	const uint8_t* cadena_i;
	int diferencia_cadenas;
	
	for (i = 0;i < N_CADENA_CMD;i++)
	{
		cadena_i = lista_de_comandos[i];															// Extraigo un comando de la lista
				
		switch(ch_in.frame[0])																		// Comandos especiales
		{
		case LEER_DATO_IDENTIFICADOR:
			return MODOS_CMD_DATO;
		}
		
		diferencia_cadenas = CompararArray_8_bits(cadena_i, ch_in.frame, ch_in.len_cadena - CANT_BYTES_CRC); 	// Comparo las cadenas contra comandos conocidos
		
		if(diferencia_cadenas == 0)																	// Comparo los bits de comando
		{
			return (i + MODOS_CMD_OFFSET);															// Devuelvo el comando que se envi�		
		}	
	}	

	return MODOS_RET_ERROR;																			// Si no encontr�, devuelvo -1
}
*/
/************************************************************************************************************ 	
 * 											Final del Archivo												*
 ************************************************************************************************************/
