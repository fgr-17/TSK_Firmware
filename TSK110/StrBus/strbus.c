/***********************************************************************************************************
 * 
 * 	@file strbus.c
 * 
 * 	@brief Configuración del protocolo de comunicación
 * 
 * 	@version 1.0
 *	
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 11/2011
 * 
 ************************************************************************************************************/

/************************************************************************************************************
 * 											Inclusión de archivos											*
 ************************************************************************************************************/
  
#include <msp430f5529.h>
#include <stdint.h>

#include <modos.h>
#include <strbus.h>

#include <cadenas_comunicacion.h>
#include <uart.h>

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

int Inicializar_Canal_1 (void);																			// Inicializo el canal de comunicacion 1
int Inicializar_Terminal(void);																			// Inicializo la terminal del equipo

int Procesar_Frame_Recibido(T_Modbus* canal_rx, T_Modbus* canal_tx);									// Seteo el estado del frame recibido

int Concatenar_CRC(T_Modbus* ch);
int Decodificar_Comando(T_Modbus ch_in);
int Frame_Respuesta(T_Modbus* ch_out, T_Modbus ch_in, T_Uart salida_UART);								// Armo la respuesta en función de lo recibido

uint16_t CRC16 (const uint8_t *nData, uint16_t wLength);												// CRC de 16 bits de IBM usado en strbus
int Leer_Frame(T_Modbus* ch_in);																		// Leo el frame recibido

int CopiarArray_8_bits(uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);			// strcpy de 8 bits
int CopiarArray_16_bits(uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);			// strcpy de 16 bits

int CompararArray_8_bits(const uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo);	// strcmp de 8 bits
int CompararArray_16_bits(const uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo);	// strcmp de 16 bits


extern T_Modo_Envio_Datos modo_enviar_f_y_h;															// alterna entre envío de fecha y de hora
/************************************************************************************************************
 * 												Variables globales											*
 ************************************************************************************************************/
 
T_Modbus canal_rx_0;
T_Modbus canal_tx_0;

T_Modbus canal_rx_2;
T_Modbus canal_tx_2;

T_Info_Terminal_Modbus terminal;

/************************************************************************************************************
 * 											Implementación de funciones										*
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

int Escribir_Dato_Buffer (T_Modbus*canal, uint8_t dato_a_escribir)
{
	if(canal->estado_buffer == BUFFER_LLENO)									// Pregunto si el buffer ya está lleno
	{
		return ESCRIBIR_DATO_BUFFER_LLENO;										// Retorno error de buffer lleno
	}
	else
	{
		canal->buffer[canal->ind_bf] = dato_a_escribir;
		canal->ind_bf++;
		if(canal->ind_bf >= BUFFER_N)
		{
			canal->estado_buffer == BUFFER_LLENO;
		}
		return ESCRIBIR_DATO_OK;
	}
}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de transmisión
 *
 * 	@params 	canal : Canal a leer
 * 	@params		dato_leido : puntero al dato a leer
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Leer_Dato_Buffer(T_Modbus*canal, uint8_t*dato_leido)
{

	if(canal->estado_buffer == BUFFER_VACIO)									// Si el buffer está vacío
	{
		return LEER_DATO_BUFFER_VACIO;											// Señalizo y salgo
	}
	else if(canal->ind_bf > 0)													// Si hay datos disponibles
	{
		canal->ind_bf--;														// Decremento el índice (apunta a donde hay que escribir)

		*dato_leido = canal->buffer[canal->ind_bf];								// Anoto el dato leído
		return LEER_DATO_BUFFER_OK;												// Indico que está bien
	}
	else
	{
		Inicializar_Canal(canal);
		return LEER_DATO_BUFFER_ERROR;
	}
}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de transmisión
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Tarea_Atender_Canal_Transmision (T_Modbus*canal)
{

	uint8_t dato_a_enviar;

	if(canal->envio_activado == SI)											// Chequeo el flag de envío activado
	{
		if(canal->dato_pendiente == NO)										// Pregunto si el canal de salida está libre
		{
			if(!Leer_Dato_Buffer(canal, &dato_a_enviar))					// Extraigo un dato del buffer de salida
			{
				UCA0TXBUF = dato_a_enviar;									// Escribo el dato a enviar en reg.de salida
			}
			else
			{
				return ATENDER_CANAL_TX_NO_DATOS;
			}
		}
		else
		{
			return ATENDER_CANAL_TX_ENVIANDO;
		}
	}
	else
	{
		return ATENDER_CANAL_TX_DESACTIVADO;
	}
}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de recepción
 *
 * 	@params 	canal : Canal a analizar
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

int Tarea_Atender_Canal_Recepcion(T_Modbus*canal)
{
	if(canal->dato_pendiente == NO)
	{
		return ATENDER_CANAL_NO_DATOS;
	}
	else if (canal->dato_pendiente == SI)
	{
		Timer_A2_Delay(1000, NO_BLOQUEANTE);								// Comienzo la cuenta del timeout
		canal->bytes_no_procesados++;										// Incremento la cantidad de bytes a procesar
		canal->dato_pendiente = NO;											// Bajo el flag de dato pendiente

		if(!(*canal->fp = Leer_Dato_Buffer(canal)))							// Extraigo un dato del buffer hacia el frame
		{
			canal->fp++;													// Incremento el puntero

			if(canal->fp > canal->frame + FRAME_N)							// Si escribí el frame
				canal->fp = canal->frame;									// Vuelvo el puntero al inicio

			if(canal->bytes_no_procesados >= FRAME_N)						// Pregunto si ya ocupe un frame
			{
				if(!Procesar_Frame(canal))									// Proceso los campos del frame
				{
					return ATENDER_CANAL_OK;								// Retorno éxito
				}
				else
				{
					canal->bytes_no_procesados--;							// Descarto el dato más viejo
					return ATENDER_CANAL_FRAME_INCORRECTO;					// El frame no está bien
				}
			}
			else if(canal->estado_buffer != BUFFER_VACIO)					// Pregunto si el buffer quedó vacío
			{
				canal->dato_pendiente = SI;									// Vuelvo a poner el flag en alto
				return ATENDER_CANAL_BYTES_PENDIENE;						// Quedan datos por procesar
			}
		}
		else
		{
			return ATENDER_CANAL_ERROR_LEER_DATO;							// Error al leer datos
		}
	}
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
__inline uint8_t Escribir_Frame(T_Modbus*canal, uint8_t dato_a_escribir)
{


	if(canal->fp > canal->frame + FRAME_N)
	{
		canal->fp = canal->frame;
	}
	*(canal->fp) = dato_a_escribir;

	canal->fp++;

	if (canal->fp >= canal->frame + FRAME_N)
	{
		canal->fp = canal->frame;
	}
}


__inline uint8_t Leer_Frame(T_Modbus*canal)
{
	canal->fp--;

	if(canal->fp < canal->frame)
		canal->fp = canal->frame + (FRAME_N - 1);

	return *(canal->fp);
}


int Procesar_Frame(T_Modbus*canal)
{
	t_uframe frame_extraido;

	frame_extraido.campo_frame.crc = Leer_Frame (canal);
	frame_extraido.campo_frame.d0  = Leer_Frame (canal);
	frame_extraido.campo_frame.d1  = Leer_Frame (canal);
	frame_extraido.campo_frame.cmd = Leer_Frame (canal);







}

/************************************************************************************************************
 *	@brief 		Inicializo los campos de la estructura "terminal", que contiene
 * 	@brief		todos los datos de conexión del equipo.				
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
 *	@brief		Inicializo los campos de la estructura de transmisión y recepción			
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Inicializar_Canal_1 (void)
{	
	//////////////////////////////////////////////
	// 	Inicialización del canal de recepción	//
	//////////////////////////////////////////////
	
	canal_rx_0.estado_buffer 	= BUFFER_VACIO;	
	canal_rx_0.Dato 			= 0;
	canal_rx_0.Direccion 		= TERMINAL_DUMMY;
	canal_rx_0.Funcion 			= STRBUS_FUN_DUMMY;
	canal_rx_0.ind 				= 0;
	canal_rx_0.len_cadena		= LARGO_FRAME;

	//////////////////////////////////////////////
	// 	Inicialización del canal de transmisión	//
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
 *	@brief		Inicializo los campos de la estructura de transmisión y recepción 2			
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Inicializar_Canal_2 (void)
{	
	//////////////////////////////////////////////
	// 	Inicialización del canal de recepción	//
	//////////////////////////////////////////////
	
	canal_rx_2.estado_buffer 	= BUFFER_VACIO;	
	canal_rx_2.Dato 			= 0;
	canal_rx_2.Direccion 		= TERMINAL_DUMMY;
	canal_rx_2.Funcion 			= STRBUS_FUN_DUMMY;
	canal_rx_2.ind 				= 0;
	canal_rx_2.len_cadena		= LARGO_FRAME;

	//////////////////////////////////////////////
	// 	Inicialización del canal de transmisión	//
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
 *	@brief 		En caso de estar el buffer de recepción, lo proceso y analizo la
 * 	@brief 		validez del mismo, para luego hacer la respuesta.				
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

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
		
		switch(canal_rx->estado_frame_procesado)										// Evalúo el resultado del frame procesado
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
				terminal.Estado_Comando = COMANDO_LIBRE;								// No hay ningún comando para procesar.
				terminal.estado_com = TERMINAL_CONECTADA;								// Desconecto para no mandar hora
			}
			else if(terminal.Comando == MODOS_CMD_INIT_MEDICION_S)						// Si quiero inicializar el esclavo, no hace falta conectar
			{
				terminal.modos_estado = CONFIGURACION;									// Salto automáticamente al estado previo a cambiar
				terminal.estado_com = TERMINAL_DESCONECTADA;							// Desconecto para no mandar hora
				terminal.Estado_Comando = COMANDO_PENDIENTE;							// Señalizo comando pendiente.
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

/************************************************************************************************************
 *	@brief		Chequeo la validez del CRC y del comando recibido.					
 * 
 * 	@params 	ch_in : Canal a leer.
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int Leer_Frame(T_Modbus* ch_in)
{
	ch_in->crc16.crc16_word = CRC16(ch_in->frame, ch_in->len_cadena);			// Hago el calculo de CRC sobre el frame recibido. Si es cero está OK

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

/************************************************************************************************************
 *	@brief 		Arma la cadena de respuesta al comando recibido.					
 * 
 * 	@params 	ch_out				Canal de salida de la respuesta. 
 *  @params		ch_in				Canal de entrada del mensaje recibido
 *  @params		ret_Maq_Estados		Respuesta de la máquina de estados a la cadena
 * 
 * 	@returns 	0	responde por la UART seleccionada.
 * 
 ************************************************************************************************************/

int Frame_Respuesta(T_Modbus* ch_out, T_Modbus ch_in, T_Uart salida_UART)
{	
	if(ch_out->estado_buffer == BUFFER_VACIO)												// Si el buffer está libre listo para enviar datos
	{
		switch(ch_out->respuesta)
		{
		case RESPONDER_OK:
			CopiarArray_8_bits(ch_out->frame, ch_in.frame, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC
			ch_out->frame[0] -= 'a' - 'A';													// Convierto la primer letra a mayúscula para armar la respuesta
			break;
			
		case RESPONDER_ESTOY:
			CopiarArray_8_bits(ch_out->frame, cadena_estoy, CANT_BYTES_BUFFER);				// Copio la respuesta sin CRC, comienzo comunicación.
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
			CopiarArray_8_bits(ch_out->frame, cadena_resistencia, CANT_BYTES_BUFFER);		// Copio la respuesta sin CRC, respondo con medición de R
			break;
			
		case RESPONDER_K:
			CopiarArray_8_bits(ch_out->frame, cadena_conductividad, CANT_BYTES_BUFFER);		// Copio la respuesta sin CRC, respondo medición de K
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
			CopiarArray_8_bits(ch_out->frame, canal_rx_0.frame , CANT_BYTES_BUFFER);		// Copio al canal de salida (PC) lo que recibí del Slave
			break;
			
		case RESPONDER_SLAVE_A_PC:
			CopiarArray_8_bits(ch_out->frame, canal_rx_2.frame , CANT_BYTES_BUFFER);		// Copio al canal de salida (Slave) lo que recibí de la PC
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
			Concatenar_CRC(ch_out);															// Concateno el CRC al final de la cadena transmitida
			ch_out->len_cadena = LARGO_FRAME;												// El paquete a transmitir es el largo del frame
		}
		
		ch_out->ind = 0;																	// Inicializo el buffer de salida																
		ch_out->estado_buffer = BUFFER_LLENO;												// Lo indico como BUFFER_LLENO
		ch_out->respuesta = NO_RESPONDER;													// Como ya atendí la llamada, no emito más respuestas
		
		switch(salida_UART)																	// de acuerdo a la salida seleccionada
		{
			case UART0:
				Iniciar_Transmision_Paquete_UART0(ch_out);									// Inicio la transmisión del paquete por UART0
				break;
			case UART1:														
				Iniciar_Transmision_Paquete_UART1(ch_out);									// Inicio la transmisión del paquete por UART1
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

/************************************************************************************************************
 *	@brief 		Evalúo si el comando es válido.					
 * 
 * 	@params 	ch_in	Canal de entrada a evaluar
 * 
 * 	@returns 	MODOS_RET_ERROR		No se encontró el comando
 *  @returns	Si encontró el comando, devuelvo el valor.
 * 
 ************************************************************************************************************/

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
			return (i + MODOS_CMD_OFFSET);															// Devuelvo el comando que se envió		
		}	
	}	

	return MODOS_RET_ERROR;																			// Si no encontró, devuelvo -1
}

/************************************************************************************************************
 *	@brief 		Agrega el CRC en una cierta cadena.		
 * 
 * 	@params 	ch		Canal a elaborar la respuesta
 * 
 * 	@returns 	none
 * 
 ************************************************************************************************************/

int Concatenar_CRC(T_Modbus* ch)
{
	ch->crc16.crc16_word = CRC16(ch->frame, CANT_BYTES_BUFFER);
	
	ch->frame[CANT_BYTES_BUFFER] = ch->crc16.crc16_nibbles[0];
	ch->frame[CANT_BYTES_BUFFER + 1] = ch->crc16.crc16_nibbles[1];
	
	// CopiarArray_8_bits(ch->frame + CANT_BYTES_BUFFER, ch->crc16.crc16_nibbles, CANT_BYTES_CRC);
	return 0;
	
}

/************************************************************************************************************
 *	@brief 		Copia un array de 8 bits a otro de 8 bits. 			
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CopiarArray_8_bits(uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
	{
		array_destino[i] = array_origen[i];
	}
	
	return 0;
}

/************************************************************************************************************
 *	@brief 		Copia un array de 16 bits a otro de 8 bits. 			
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CopiarArray_16_bits(uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
	{
		array_destino[i] = array_origen[i];
	}
	
	return 0;	
}

/************************************************************************************************************
 *	@brief 		Compara dos array de 8 bits y devuelve la diferencia del primer
 *  @brief		elemento no coincidente entre los 2
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CompararArray_8_bits(const uint8_t* array_destino, const uint8_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
		if(array_destino[i] != array_origen[i])
			return (array_destino[i] - array_origen[i]);

	return 0;
}

/************************************************************************************************************
 *	@brief 		Compara dos array de 16 bits y devuelve la diferencia del primer
 *  @brief		elemento no coincidente entre los 2
 * 
 * 	@params 	array_destino
 *  @params		array_origen
 *  @params		largo			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

int CompararArray_16_bits(const uint16_t* array_destino, const uint16_t* array_origen, uint16_t largo)
{
	uint16_t i;
	
	for(i = 0;i < largo;i++)
		if(array_destino[i] != array_origen[i])
			return (array_destino[i] - array_origen[i]);

	return 0;
}

/************************************************************************************************************
 *	@brief 		Realiza el calculo de CRC16 bits Modbus sobre un array
 * 
 * 	@params 	nData			Array de datos sobre los que se calcula el CRC16
 *  @params		wLength			Longitud del array
 * 
 * 	@returns 	0 success
 * 
 ************************************************************************************************************/

uint16_t CRC16 (const uint8_t *nData, uint16_t wLength)
{
static const uint16_t wCRCTable[] = {
0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

uint8_t nTemp;
uint16_t wCRCWord = 0xFFFF;

   while (wLength--)
   {
      nTemp = *nData++ ^ wCRCWord;
      wCRCWord >>= 8;
      wCRCWord ^= wCRCTable[nTemp];
   }
   return wCRCWord;
}




/************************************************************************************************************ 	
 * 											Final del Archivo												*
 ************************************************************************************************************/
