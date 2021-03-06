/************************************************************************************************************
 *																											*
 * 	Archivo : modos.c																						*
 *																											*
 * 	Descripci�n: m�quina de estados para el cambio de modos de funcionamiento del TSK100					
 *
 * 				 (ver archivo "TSK100-DiagramadeEstados.pdf")												*
 *																											*
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)														*
 *																											* 
 * 	NEROX 11/2011																							*
 *																											* 
 ************************************************************************************************************/

/************************************************************************************************************
 * 						Inclusi�n de archivos						*
 ************************************************************************************************************/

#include <msp430f5529.h>

#include <stdint.h>
#include "Maq_Estados/modos.h"
#include "StrBus/strbus.h"
 
#include "Flash/flash_mapeo_variables.h"

#include "F5xx_F6xx_Core_Lib/HAL_RTC.h"

#include "StrBus/cadenas_comunicacion_externo.h"

#include "Medicion_Sensores/Sensores/Temperatura.h"
#include "Medicion_Sensores/Sensores/Conductividad.h"

#include "SD_Card/SD_Card.h"
#include "Log/log.h"

#include "UART/uart.h"
#include "Timer_A2/Timer_A2.h"

#include "inc/funciones_arrays.h"

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

///////////////////////////////////////////// Funciones Privadas ////////////////////////////////////////////
inline int Leer_y_Enviar_Hora_y_Fecha(void);
inline int Leer_y_Enviar_Conductividad(void);
inline int Leer_y_Enviar_ADC_Temperatura(void);
inline int Leer_y_Enviar_Mediciones(void);
inline int Enviar_Lista_Archivos_Log(void);
inline int Init_Enviar_Lista_Archivos_Log(void);
inline int Leer_DWord_Entrante(T_Modbus *ch_in);
inline int Leer_Words_Entrantes(T_Modbus* ch_in);
///////////////////////////////////////////// Funciones globales ////////////////////////////////////////////
int Extraer_Nombre_de_Archivo(int8_t ind_arch, char* array_dst, const uint8_t* path);
int Modos_Inicializar(void);
int Modos_Maq_Estados(void);
//////////////////////////////////////////// Precalibraci�n Master //////////////////////////////////////////
int Precalibracion_Master(void); 
int Precalibracion_Slave(void);

/************************************************************************************************************
 * 						Variables globales						*
 ************************************************************************************************************/

const char variables_validas[CANT_VARIABLES_VALIDAS] = {'T', 'K'};					// Recibo conductividad o temperatura
const char parametros_validos[CANT_PARAMETROS_VALIDOS] = {'P', 'O', 'V'};				// Recibo pendiente, offset o valor

int32_t Pendiente_Resistencia_tmp;									// Valor de la pendiente temporal, antes de confirmar la calibraci�n
int32_t Offset_Resistencia_tmp;									// Valor del offset de la conversi�n, antes de confirmar la calibraci�n

int32_t Pendiente_Conductividad_tmp;									// Valor temporal de la pendiente de conductividad
int32_t Offset_Conductividad_tmp;																	// Valor temporal del offset de conductividad

T_Modo_Envio_Datos modo_envio_T;									// variable que env�o al mandar los datos
T_Modo_Envio_Datos modo_envio_K;									// variable que env�o al mandar los datos

int8_t ind_archivo = 0;											// Indice del nombre de archivo que estoy enviando
uint8_t ind_acumulado = 0;										// Indice para recorrer el array de nombres

uint8_t hour_tmp = 0;											// Variable temporal para guardar la hora recibida del sistema	
uint8_t min_tmp = 0;											// idem minuto
uint8_t sec_tmp = 0;											// idem segundo

uint8_t day_tmp = 0;											// idem d�a
uint8_t mon_tmp = 0;											// idem mes
uint16_t year_tmp = 0;											// idem a�o

T_Modo_Envio_Datos modo_enviar_f_y_h = ENVIAR_FECHA;							// alterna entre env�o de fecha y de hora

////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////				VARIABLES NUEVAS				////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////

T_Modbus*canal_ppal = &canal_rx_0;								// Asigno el canal que se usa como canal principal
T_Modbus*canal_sec = &canal_rx_2;								// Asigno el canal que se conecta con el slave (en caso de que sea el master)


/************************************************************************************************************
 * 											Implementaci�n de funciones										*
 ************************************************************************************************************/

/************************************************************************************************************
 * @brief  	Inicializaci�n de la m�quina de estados Modos													*
 * 																											*
 * @param 	none																							*
 * 																											*
 * @return	0 -> success																					*
 ************************************************************************************************************/

int Modos_Inicializar(void)
{
	// RecuperarVariablesFlash();
	
	// if(Pendiente_Resistencia < 0)
	// 	Precalibracion_Master();
	
	terminal.modos_estado = CONFIGURACION;
	
	return 0;	
}

/************************************************************************************************************
 * @brief  	M�quina de Estados Modos.																		*
 * 																											*
 * @param 	none																							*
 * 																											*
 * @return	0 -> success																					*
 ************************************************************************************************************/

#define FLAG_AUX_ESPERAR_ANTES_DE_DESCARGAR		0x55
 
int Modos_Maq_Estados(void)
{
	uint8_t flag_aux;
	
	switch(terminal.modos_estado)
	{
	case CONFIGURACION:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )					// Para cambiar de estado:
		{
			// if(terminal.Comando == MODOS_CMD_INIT_MASTER)
			if(canal_ppal->comando == INIMA)								// CMD : Inicializo master?
			{						
				terminal.modos_estado = MASTER_ESPERA_SLAVE;				// Seteo el estado correspondiente
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero flag de comando
				Activar_Timeout_Slave();									// Empieza la cuenta de timeout, se�alizo Slave = vivo
				// canal_tx_2.respuesta = RESPONDER_INIT_MEDICION_S;		// todo : respuesta
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_MEDICION_S)		// Recibo �ste comando por R1 desde el Master. "q haces, master! ;)"
			else if(canal_ppal->comando == INIMS)							// Inicializo medici�n del esclavo
			{
				terminal.Ts = TS_MODO_MEDIR;								// Seteo frecuencia de muestreo para medir
				terminal.modos_estado = SLAVE_MIDE;							// Paso al estado de SLAVE_MIDE
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero flag de comando
				RTCCTL0 = RTCRDYIE + RTCTEVIE;	        					// Habilito la ISR del RTC
				terminal.medicion = TERMINAL_LIBRE;							// La terminal est� libre para medir
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_DESCARGA)
			else if(canal_ppal->comando == INIDE)							// CMD : Inicializo descarga?
			{
				terminal.modos_estado = DATOS_DISPONIBLES;					// Paso al estado DATOS_DISPONIBLES
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero flag de comando
				GetDirectory(root);											// Llamo a GetDirectory() que me de lo que hay en Log_Datos, as� lo puedo mandar como respuesta.
				Init_Enviar_Lista_Archivos_Log();							// Inicializo el �ndice para mandar los nombres
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_CALIBRACION_K)
			else if(canal_ppal->comando == INICK)							// CMD : Inicializo calibraci�n de conductividad
			{
				terminal.Ts	= TS_MODO_CALIBRACION_K;						// Seteo el tiempo de adquisici�n
				modo_envio_K = ENVIAR_PENDIENTE; 							// todo : Empiezo mandando la pendiente, offset y despu�s datos
				terminal.modos_estado = CALIBRACION_K;						// Paso al estado de CALIBRACION_K
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero flag de comando
				terminal.medicion = TERMINAL_LIBRE;							// La terminal est� libre para medir
				RTCCTL0 |= RTCRDYIE + RTCTEVIE;         					// Habilito la interrupci�n del RTC
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_CALIBRACION_T)
			else if(canal_ppal->comando == INICT)							// CMD : Inicializo calibraci�n de Temperatura
			{
				terminal.Ts	= TS_MODO_CALIBRACION_T;						// Seteo el tiempo de adquisici�n
				modo_envio_T = ENVIAR_PENDIENTE; 							// Empiezo mandando la pendiente, offset y despu�s datos
				terminal.modos_estado = CALIBRACION_T;						// paso al estado de CALIBRACION_T
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero flag de comando
				terminal.medicion = TERMINAL_LIBRE;							// La terminal est� libre para medir
				RTCCTL0 |= RTCRDYIE + RTCTEVIE;         					// Enable interrupt
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_SETEAR_F_Y_H)
			else if (canal_ppal->comando == SEFYH)							// CMD : Inicializo configuraci�n de fecha y hora
			{
				terminal.modos_estado = RECIBE_HORA;						// La terminal esta lista para recibir seteo de hora y fecha
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
			}
			// else if(terminal.Comando == MODOS_CMD_RECIBIR_ALARMA)
			else if (canal_ppal->comando == REALA)							// CMD : Modo recibir valor de alarma
			{
				terminal.modos_estado = RECIBE_ALARMA;						// La terminal esta lista para recibir seteo de hora y fecha
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
			}
			// else if(terminal.Comando == MODOS_CMD_RECIBIR_UMBRAL)
			else if (canal_ppal->comando == REUMB)							// CMD : Inicializo modo de recibir umbral
			{
				terminal.modos_estado = RECIBE_UMBRAL;						// La terminal esta lista para recibir seteo de hora y fecha
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
			}
			// else if(terminal.Comando == MODOS_CMD_INI_REPETIDOR)
			else if (canal_ppal->comando == INIRE)							// CMD : Inicializo modo de repetidor
			{	
				terminal.modos_estado = MASTER_REPETIDOR;					// La terminal esta lista para recibir seteo de hora y fecha
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
			}
			// else if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			else if (canal_ppal->comando == CONFI)							// CMD : Inicializo modo de configuraci�n
			{
				terminal.modos_estado = CONFIGURACION;						// Estado principal de configuraci�n
				modo_enviar_f_y_h = ENVIAR_FECHA;							// Seteo modo de enviar fecha y hora
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
			}
			else															// Modo por default:
			{						
				terminal.modos_estado = CONFIGURACION;						// Vuelvo al estado ppal de configuraci�n
				modo_enviar_f_y_h = ENVIAR_FECHA;							// Env�o fecha y hora
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
				// canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;				// todo : Respondo error
				return MODOS_RET_UBIC;										// retorno se�alizaci�n
			}
		}
		else																// En caso de que no tenga ning�n comando pendiente
		{			// pregunto si la terminal est� conectada y si no tiene nada que responder
			if((terminal.estado_com == TERMINAL_CONECTADA)&&(canal_tx_0.respuesta == NO_RESPONDER))	
				Leer_y_Enviar_Hora_y_Fecha();								// Env�o hora y fecha e info del equipo.
		}		
		break;
		
	case MASTER_ESPERA_SLAVE:												// Estado : MASTER_ESPERA_SLAVE
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )					// Para cambiar de estado:
		{
			//if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			if (canal_ppal->comando == CONFI)								// CMD : volver a configuraci�n?
			{
				terminal.modos_estado = CONFIGURACION;						// Vuelvo al estado anterior
				// modo_enviar_f_y_h = ENVIAR_FECHA;						// todo : Env�o fecha
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero el comando
				// Escribir_Word_en_Flash(terminal.modos_estado, MODO_ESTABLECIDO_ADDRESS);	// El cambio al modo "CONFIGURACION" lo guardo en flash			
			}
			// else if(terminal.Comando == MODOS_CMD_INIT_MEDICION_M)		// Este comando lo recibo por R2, desde el slave.
			else if (canal_sec->comando == INIMS)							// Si recibo inicializaci�n de medici�n de Slave por el
			{																// canal secundario en realidad es una respuesta confirmatoria del slave
				Activar_Timeout_Slave();									// Empieza la cuenta de timeout, se�alizo Slave = vivo
				terminal.modos_estado = MASTER_MIDE;						// Paso al estado de master mide
				terminal.Estado_Comando = COMANDO_LIBRE;					// Libero comando
			}
			else
			{
				terminal.modos_estado = MASTER_ESPERA_SLAVE;
				terminal.Estado_Comando = COMANDO_LIBRE;
				Escribir_Word_en_Flash(terminal.modos_estado, MODO_ESTABLECIDO_ADDRESS);	// El cambio al modo "CONFIGURACION" lo guardo en flash
				// canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;				// todo : respondo se�alizaci�n
				return MODOS_RET_UBIC;										// Se�alizo hacia afuera
			}	
		}
		else if(timeout_Slave.slave_vivo == FALSE)							// Si paso el Timeout y no recib� nada,
		{
			terminal.modos_estado = MASTER_MIDE;							// Paso autom�ticamente a Medir autodisparado
			terminal.Ts = TS_MODO_MEDIR;
			terminal.Estado_Comando = COMANDO_LIBRE;						// Por las dudas se�alizo comando libre
		}
		break;
		
	case MASTER_MIDE:														// Estado : Master midiendo

		if(terminal.Estado_Comando == COMANDO_PENDIENTE )					// Para cambiar de estado:
		{
			// if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			if (canal_ppal->comando == CONFI)								// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;			
			}
			// else if(terminal.Comando ==  MODOS_CMD_FIN_MEDICION_M)		// Este comando lo recibo por R2
			else if (canal_ppal->comando == FINMM)
			{
				terminal.modos_estado = MASTER_ESPERA_SLAVE;
				terminal.Estado_Comando = COMANDO_LIBRE;	
			}
			// else if(terminal.Comando ==  MODOS_CMD_DATO)					// Recibo datos por R2
			else if ((canal_sec->comando == DATOK)||(canal_sec->comando == DATOT))		// Pregunto si recib� un dato del slave
			{
				//////////////////////////////////////////////////////////////////////////
				//				* Hago una medici�n de T y K							//
				// 				* Armo cadena con datos recibidos y medido				//
				//				* Escribo SD con cadena armada							//
				//				* Mando datos (M) y (S) por (T1)						//
				//////////////////////////////////////////////////////////////////////////				
				if(Leer_Words_Entrantes(&canal_rx_2) == 0)
				{
					Activar_Timeout_Slave();														// Empieza la cuenta de timeout, se�alizo Slave = vivo
					
					terminal.medicion = TERMINAL_MEDIR;												// La llegada del dato del esclavo dispara la medici�n del master
										
					conductividadProcesada_recibida_prev = conductividadProcesada_recibida;			// Guardo la conductividad recibida anterior
					temperaturaProcesada_recibida_prev = temperaturaProcesada_recibida;				// Guardo la temperatura recibida anterior
					
					conductividadProcesada_recibida = canal_rx_2.dato_1;							// Guardo la conductividad recibida
					temperaturaProcesada_recibida = canal_rx_2.dato_2;								// Guardo la temperatura recibida
					
					terminal.Estado_Comando = COMANDO_LIBRE;										// Libero la terminal
				
					// El resto del trabajo lo hago desde Sensores.c	
					// Comienzo una cuenta de Timeout
				}
				else
				{
					// Recib� un dato incorrecto, no hago nada.
				}
			}			
			else
			{
				
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
				/*
				terminal.modos_estado = MASTER_MIDE;
				terminal.Estado_Comando = COMANDO_LIBRE;	
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				*/
				return MODOS_RET_UBIC;
						
			}	
		}
		else if(timeout_Slave.slave_vivo == FALSE)											// Si paso el Timeout y no recib� nada,
		{
			// El estado de TERMINAL_MEDIR para disparar la medici�n se setea en el RTC
			// terminal.medicion = TERMINAL_MEDIR;											// La llegada del dato del esclavo dispara la medici�n del master
					
			conductividadProcesada_recibida = 0;											// Seteo en cero la medici�n del slave
			temperaturaProcesada_recibida = 0;											
			
		}		
		break;
		
	case SLAVE_MIDE:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )									// Para cambiar de estado:
		{
		
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				// modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_FIN_MEDICION_S)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando == MODOS_CMD_INIT_MEDICION_S)							// Recibo �ste comando por R1 desde el Master. "q haces, master! ;)"
			{
				//////////////////////////////////////////////////////////////////////////////
				// 			* Inicializo el RTC como trigger para medici�n					//
				//			* Respondo al master que se empez� la medici�n					//
				//////////////////////////////////////////////////////////////////////////////
				
				terminal.modos_estado = SLAVE_MIDE;
				terminal.Estado_Comando = COMANDO_LIBRE;
				RTCCTL0 = RTCRDYIE + RTCTEVIE;	        									// Habilito la ISR del RTC
				terminal.medicion = TERMINAL_LIBRE;											// La terminal est� libre para medir
			}
			else
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				/*
				terminal.modos_estado = SLAVE_MIDE;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;*/
				return MODOS_RET_UBIC;			
			}		
		}
		else
		{
			Leer_y_Enviar_Mediciones();
		}
		break;
	
	case DATOS_DISPONIBLES:	
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;			
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else
			{
				terminal.modos_estado = DATOS_DISPONIBLES;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}		
		}
		else
		{
			flag_aux = Enviar_Lista_Archivos_Log();										// Env�o la lista de archivos encontrado,
			if(flag_aux == 1)															// quedan m�s??	
			{	
				terminal.modos_estado = PC_RECIBIO_NARCH;								// Paso a esperar la confirmaci�n de la PC
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando
			}
			else if (flag_aux == 0)														// Mand� todos los nombres de archivo.
			{
				terminal.modos_estado = SELECCION_ARCHIVO;								// Paso a esperar la confirmaci�n de la PC
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando
			}
		}
		break;
	
	case PC_RECIBIO_NARCH:
		
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;			
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando == MODOS_CMD_ARCHIVO_OK)
			{
				terminal.modos_estado = DATOS_DISPONIBLES;
				terminal.Estado_Comando = COMANDO_LIBRE;
				// canal_tx_0.respuesta = NO_RESPONDER;									// Como es una confirmaci�n de la PC, no mando respuesta
			}
			else
			{
				terminal.modos_estado = DATOS_DISPONIBLES;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}		
		}
		break;
	
	case SELECCION_ARCHIVO:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )															// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;		
				modo_enviar_f_y_h = ENVIAR_FECHA;	
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)
			{
				Leer_DWord_Entrante(&canal_rx_0);																	// Si el dato es v�lido, paso a descargarlo		
				
				if(canal_rx_0.Dato > 0)																				// Si el dato es positivo, es para extraer
				{
					canal_rx_0.Dato--;																				// Lo vuelvo a referenciar al cero
				
					if(Extraer_Nombre_de_Archivo(canal_rx_0.Dato, stream_archivo.fileName,(const uint8_t*)root) == 0)	// Pruebo de extraer el nombre de archivo
					{
						Init_Leer_Stream_Archivo();
						terminal.modos_estado = DESCARGO_DATOS;
						terminal.Estado_Comando = COMANDO_LIBRE;
						canal_tx_0.respuesta = RESPONDER_OK;
						flag_aux = FLAG_AUX_ESPERAR_ANTES_DE_DESCARGAR;
					}
					else																								// sino, vuelvo a configuraci�n y mando ERROR
					{
						canal_tx_0.respuesta = RESPONDER_ERROR_CMD;														// Error de comando, 
					}
				}
				else if(canal_rx_0.Dato < 0)
				{
					canal_rx_0.Dato++;																					// Referencio a cero
					canal_rx_0.Dato *= -1;
					
					if(Extraer_Nombre_de_Archivo(canal_rx_0.Dato, stream_archivo.fileName,(const uint8_t*)root) == 0)	// Pruebo de extraer el nombre de archivo
					{
						f_mount(0, &(stream_archivo.fatfs));															// Monto el dispositivo en memoria
						f_unlink(stream_archivo.fileName);
						terminal.modos_estado = CONFIGURACION;
						terminal.Estado_Comando = COMANDO_LIBRE;
						canal_tx_0.respuesta = RESPONDER_OK;
					}
					else																								// sino, vuelvo a configuraci�n y mando ERROR
					{
						canal_tx_0.respuesta = RESPONDER_ERROR_CMD;														// Error de comando, 
					}
				}
			}
			else
			{
				terminal.modos_estado = DATOS_DISPONIBLES;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}		
		}
		break;
	
	case DESCARGO_DATOS:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )											// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;			
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else
			{
				terminal.modos_estado = DESCARGO_DATOS;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}	
		}	
		else
		{
			if(flag_aux == FLAG_AUX_ESPERAR_ANTES_DE_DESCARGAR)										// La 1ra vez espero 1 segundo
				Timer_A2_Delay(1000, BLOQUEANTE);													// Comienzo la cuenta del delay
				
			flag_aux = Leer_Stream_Archivo();														// Guardo la respuesta de la funci�n
			if(flag_aux == 0)																		// Mand� un frame y falta terminar
			{
				terminal.modos_estado = DESCARGO_DATOS;												// Termin� de leer el archivo
				terminal.Estado_Comando = COMANDO_LIBRE;											// Vuelvo al estado configuraci�n
				canal_tx_0.respuesta = RESPONDER_FRAME_ARCHIVO;
			}
			else if(flag_aux == 2)
			{
				terminal.modos_estado = CONFIGURACION;												// Termin� de leer el archivo
				terminal.Estado_Comando = COMANDO_LIBRE;											// Vuelvo al estado configuraci�n
				modo_enviar_f_y_h = ENVIAR_FECHA;
				canal_tx_0.respuesta = RESPONDER_FIN_ARCHIVO;
			}			
		}
		break;
	
	case CALIBRACION_K:

		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)							// Vuelvo a modo configuraci�n
			{
				terminal.modos_estado = CONFIGURACION;							
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando de la terminal
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_OK_CALIBRACION_K)					// Finaliz� la calibraci�n, espero si hubo �xito o no
			{
				terminal.modos_estado = ESPERO_PENDIENTE_K;							
				terminal.Estado_Comando = COMANDO_LIBRE;							
				// RTCCTL0 &= ~(RTCRDYIE + RTCTEVIE);         								// Deshabilito la ISR del RTC
			}	
			else if(terminal.Comando ==  MODOS_CMD_CANCEL_CALIBRACION_K)				// Calibraci�n cancelada
			{
				terminal.modos_estado = CONFIGURACION;									// Vuelvo a configuraci�n
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando de la terminal
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else
			{
				terminal.Ts	= TS_MODO_CALIBRACION_K;
				terminal.modos_estado = CALIBRACION_K;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				RTCCTL0 = RTCRDYIE + RTCTEVIE;	         								// Habilito la ISR del RTC
				return MODOS_RET_UBIC;			
			}	
		}
		else
		{				
			RTCCTL0 = RTCRDYIE + RTCTEVIE;	         									// Habilito la ISR del RTC
			Leer_y_Enviar_Conductividad();												// Mandar medici�n por puerto serie!!										 			
		}	
		break;
	
	case ESPERO_PENDIENTE_K:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;			
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))
				{
					Pendiente_Conductividad_tmp = canal_rx_0.Dato;
					terminal.modos_estado = ESPERO_OFFSET_K;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					canal_tx_0.respuesta = RESPONDER_OK;				
				}
				else
				{
					Pendiente_Conductividad_tmp = 0;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;
				}				
			}
			else
			{
				terminal.modos_estado = ESPERO_PENDIENTE_K;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}										
		}		
		break;		
	
	case ESPERO_OFFSET_K:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;		
				modo_enviar_f_y_h = ENVIAR_FECHA;	
			}
			
			else if(terminal.Comando ==  MODOS_CMD_DATO)
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))
				{
					Offset_Conductividad_tmp = canal_rx_0.Dato;
					terminal.modos_estado = CALIBRACION_K;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					canal_tx_0.respuesta = RESPONDER_OK;	
																
					Pendiente_Conductividad = Pendiente_Conductividad_tmp;
					Offset_Conductividad = Offset_Conductividad_tmp;
				
					Escribir_DWord_en_Flash(Pendiente_Conductividad, PENDIENTE_CONDUCTIVIDAD_ADDRESS);
					Escribir_DWord_en_Flash(Offset_Conductividad, OFFSET_CONDUCTIVIDAD_ADDRESS);
					 
				}
				else
				{
					Offset_Resistencia_tmp = 0;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;
				}				
			}
			else
			{
				terminal.modos_estado = ESPERO_PENDIENTE_T;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}										
		}
			
		
		break;
		
	case CALIBRACION_T:

		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Para cambiar de estado:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)							// Vuelvo a modo configuraci�n
			{
				terminal.modos_estado = CONFIGURACION;							
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando de la terminal
				modo_enviar_f_y_h = ENVIAR_FECHA;

			}
			else if(terminal.Comando ==  MODOS_CMD_OK_CALIBRACION_T)					// Finaliz� la calibraci�n, espero si hubo �xito o no
			{
				terminal.modos_estado = ESPERO_PENDIENTE_T;							
				terminal.Estado_Comando = COMANDO_LIBRE;							
				// RTCCTL0 &= ~(RTCRDYIE + RTCTEVIE);         								// Deshabilito la ISR del RTC
			}
			else if(terminal.Comando ==  MODOS_CMD_CANCEL_CALIBRACION_T)				// Calibraci�n cancelada
			{
				terminal.modos_estado = CONFIGURACION;									// Vuelvo a configuraci�n
				terminal.Estado_Comando = COMANDO_LIBRE;								// Libero el comando de la terminal
				modo_enviar_f_y_h = ENVIAR_FECHA;

			}
			else
			{
				terminal.Ts	= TS_MODO_CALIBRACION_T;
				terminal.modos_estado = CALIBRACION_T;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				RTCCTL0 = RTCRDYIE + RTCTEVIE;	         								// Habilito la ISR del RTC
				return MODOS_RET_UBIC;			
			}	
		}
		else
		{				
			RTCCTL0 = RTCRDYIE + RTCTEVIE;	         									// Habilito la ISR del RTC
			Leer_y_Enviar_ADC_Temperatura();											// Mandar medici�n por puerto serie!!										 			
		}	
		break;
	
	case ESPERO_PENDIENTE_T:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )								// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;	
				modo_enviar_f_y_h = ENVIAR_FECHA;		
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))
				{
					Pendiente_Resistencia_tmp = canal_rx_0.Dato;
					terminal.modos_estado = ESPERO_OFFSET_T;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					canal_tx_0.respuesta = RESPONDER_OK;				
				}
				else
				{
					Pendiente_Resistencia_tmp = 0;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;
				}				
			}
			else
			{
				terminal.modos_estado = ESPERO_PENDIENTE_T;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}										
		}
			
		
		break;		
	
	case ESPERO_OFFSET_T:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)											// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)												// Recib� un dato
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))													// Leo un dato
				{
					Offset_Resistencia_tmp = canal_rx_0.Dato;											// Guardo el dato de forma temporal
					
					terminal.modos_estado = CALIBRACION_T;												// Vuelvo al estado de calibraci�n
					terminal.Estado_Comando = COMANDO_LIBRE;											// Despejo el flag de comando
					canal_tx_0.respuesta = RESPONDER_OK;													// Respondo OK
					
					Pendiente_Resistencia  = Pendiente_Resistencia_tmp;									// Guardo los valores temporales en las
					Offset_Resistencia     = Offset_Resistencia_tmp;									// variables definitivas de calibraci�n

					Escribir_DWord_en_Flash(Pendiente_Resistencia, PENDIENTE_RESISTENCIA_ADDRESS);		// Guardo pendiente en flash	
					Escribir_DWord_en_Flash(Offset_Resistencia, OFFSET_RESISTENCIA_ADDRESS);			// Guardo offset en flash
					
					terminal.Ts	= TS_MODO_CALIBRACION_T;												// Seteo el tiempo de adquisici�n
				
				}
				else
				{
					Offset_Resistencia_tmp = 0;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;	
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;
				}				
			}
			else
			{
				terminal.modos_estado = ESPERO_PENDIENTE_T;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}										
		}
			
		
		break;
		
	case RECIBE_HORA:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)											// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)												// Recib� un dato
			{
				/*
				hour_tmp = canal_rx_0.frame[1];															// Guardo temporalmente la hora
				min_tmp = canal_rx_0.frame[2];															// idem minuto
				sec_tmp = canal_rx_0.frame[3]; 															// idem segundo
				
				terminal.modos_estado = RECIBE_FECHA;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_OK;
				*/
			}
			else
			{
				terminal.modos_estado = RECIBE_HORA;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}
		}
		break;

	case RECIBE_FECHA:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)											// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)												// Recib� un dato
			{
				day_tmp = canal_rx_0.frame[1];															// Guardo temporalmente la hora
				mon_tmp = canal_rx_0.frame[2];															// idem minuto
				
				year_tmp = canal_rx_0.frame[3]; 														// guardo los 2 d�gitos m�s sign. del a�o
				year_tmp <<= 8;																			// los corro 2 d�gitos hacia la izquierda
//				year_tmp += canal_rx_0.frame[4];														// guardo los dos d�gitos m�s bajos
				
				if(!Establecer_Registros_RTC(hour_tmp, min_tmp, sec_tmp, day_tmp, mon_tmp, year_tmp))	// Valido y seteo el valor de los registros
				{
					terminal.modos_estado = CONFIGURACION;												// Termin� de setear fecha y hora
					terminal.Estado_Comando = COMANDO_LIBRE;											// libero el comando
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_OK;
				}
				else
				{
					terminal.modos_estado = CONFIGURACION;												// Termin� de setear fecha y hora
					terminal.Estado_Comando = COMANDO_LIBRE;											// libero el comando
					modo_enviar_f_y_h = ENVIAR_FECHA;
					canal_tx_0.respuesta = RESPONDER_ERROR_CMD;											// Devuelvo "ERROR DE COMANDO"
				}
			}
			else
			{
				terminal.modos_estado = RECIBE_HORA;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}
		}
		break;
		
	case RECIBE_ALARMA:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE)												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)											// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)												// Recib� un dato
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))													// Leo un dato
				{
					if(canal_rx_0.Dato <= 0)															// Pregunto si es un valor de alarma v�lido
					{
						terminal.alarma_activa = 0;														// Alarma inactiva
						terminal.nivel_alarma = 0;
						
					}
					else
					{
						terminal.alarma_activa = 1;														// Activo la alarma
						terminal.nivel_alarma = (int16_t) canal_rx_0.Dato;								// Seteo el nivel de alarma						
					}
					
					Escribir_Byte_en_Flash(terminal.alarma_activa, ALARMA_ACTIVA_ADDRESS);
					Escribir_Word_en_Flash(terminal.nivel_alarma, NIVEL_ALARMA_ADDRESS);					
					
					canal_tx_0.respuesta = RESPONDER_OK;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;
				}
				else
				{
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;											// Se�alizo dato err�neo
				}
			}
			else
			{
				terminal.modos_estado = RECIBE_HORA;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}
		}
		break;
		
	case RECIBE_UMBRAL:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_CONFIGURACION)											// Vuelvo al estado ppal.
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando ==  MODOS_CMD_DATO)												// Recib� un dato
			{
				if(!Leer_DWord_Entrante(&canal_rx_0))													// Leo un dato
				{
					if(canal_rx_0.Dato <= 0)															// Pregunto si es un valor de alarma v�lido
					{
						terminal.modo_diferencial = 0;													// Alarma inactiva
						terminal.nivel_umbral = 0;														// limpio la variable del nivel de umbral
						
					}
					else
					{
						terminal.modo_diferencial = 1;													// Activo el modo diferencial
						terminal.nivel_umbral = (int16_t)canal_rx_0.Dato;										// Seteo el nivel de alarma						
					}
					canal_tx_0.respuesta = RESPONDER_OK;
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;
				}
				else
				{
					terminal.modos_estado = CONFIGURACION;
					terminal.Estado_Comando = COMANDO_LIBRE;
					canal_tx_0.respuesta = RESPONDER_ERROR_DAT;											// Se�alizo dato err�neo
				}
			}
			else
			{
				terminal.modos_estado = RECIBE_HORA;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_ERROR_UBQ;
				return MODOS_RET_UBIC;			
			}
		}
		break;
	
	case MASTER_REPETIDOR:
	
		if(terminal.Estado_Comando == COMANDO_PENDIENTE )												// Procesar comandos:
		{
			if(terminal.Comando ==  MODOS_CMD_FIN_REPETIDOR)											// Vuelvo al estado ppal. la �nica manera de salir de este modo
			{
				terminal.modos_estado = CONFIGURACION;
				terminal.Estado_Comando = COMANDO_LIBRE;
				modo_enviar_f_y_h = ENVIAR_FECHA;
			}
			else if(terminal.Comando == MODOS_CMD_PC_A_SLAVE) 											// Recib� un comando de la PC	
			{
				terminal.modos_estado = MASTER_REPETIDOR;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_2.respuesta = RESPONDER_PC_A_SLAVE;
			}
			else if(terminal.Comando == MODOS_CMD_SLAVE_A_PC) 											// Recib� un comando del Slave
			{
				terminal.modos_estado = MASTER_REPETIDOR;
				terminal.Estado_Comando = COMANDO_LIBRE;
				canal_tx_0.respuesta = RESPONDER_SLAVE_A_PC;
			}
			
		}
		break;
	}
	
	return MODOS_RET_OK;
	
}

/************************************************************************************************************
 * @brief  	Leo la hora y la mando por puerto serie															*
 *																											* 
 * @param 	none																							*
 *																											*   
 * @return	0 -> no hizo nada																				*
 * @return  1 -> mand� los datos!																			*
 ************************************************************************************************************/

inline int Leer_y_Enviar_Hora_y_Fecha(void)
{
	T_Bytes_a_Word anio;
	
	if((lectura_registros_RTC == FALSE)&&(canal_tx_0.estado_buffer == BUFFER_VACIO))// Espero a que el flag est� en FALSE para poder leer los registros
	{
		Leer_Registros_RTC(FALSE);													// Vuelvo a preguntar por los registros
		
		if (modo_enviar_f_y_h == ENVIAR_FECHA)
		{
			anio.word = RTCYEAR;
			
			modo_enviar_f_y_h = ENVIAR_HORA;	
			
			cadena_hora[0] = '/';
			cadena_hora[1] = RTCDAY;												// cargo la hora en el buffer
			cadena_hora[2] = RTCMON;												// cargo el minuto en el buffer		
			cadena_hora[3] = anio.bytes[0];											// cargo los segundos en el buffer
			cadena_hora[4] = anio.bytes[1];											// cargo los segundos en el buffer
		}
		else if(modo_enviar_f_y_h == ENVIAR_HORA)
		{
			cadena_hora[0] = '-';
			cadena_hora[1] = hour_BCD;												// cargo la hora en el buffer
			cadena_hora[2] = min_BCD;												// cargo el minuto en el buffer		
			cadena_hora[3] = sec_BCD;												// cargo los segundos en el buffer
			if((hour_BCD == 0x00)&&(min_BCD == 0x00)&&(sec_BCD == 0x00))			// si cambio de dia
				modo_enviar_f_y_h = ENVIAR_FECHA;									// vuelvo a mandar la fecha
		}
		else
		{
			modo_enviar_f_y_h = ENVIAR_FECHA;									// vuelvo a mandar la fecha
		}
		
		canal_tx_0.respuesta = RESPONDER_HORA;										// Levanto el flag para responder
		return 1;
	}
	else
	{
		return 0;
	}	
} 

/************************************************************************************************************
 * @brief  	Leo la Temperatura y la mando por puerto serie													*
 *																											* 
 * @param 	none																							*
 *																											*   
 * @return	0 -> no hizo nada																				*
 * @return  1 -> mand� los datos!																			*
 ************************************************************************************************************/
/*
inline int Leer_y_Enviar_Temperatura(void)
{
	T_Bytes_a_Word Temperatura_a_Enviar;
	
	if(terminal.medicion == TERMINAL_MIDIENDO)						// Termin� de hacer una medici�n pero no baj� el flag porque est� en modo calibracion T
	{				
		terminal.medicion = TERMINAL_LIBRE;							// Seteo la terminal libre para un nuevo conteo del RTC.
		
		Temperatura_a_Enviar.word = temperaturaProcesada;
		
		cadena_temperatura [CADENA_TEMPERATURA_IND_PARTE_ALTA] = Temperatura_a_Enviar.bytes[1];
		cadena_temperatura [CADENA_TEMPERATURA_IND_PARTE_BAJA] = Temperatura_a_Enviar.bytes[0];
		
		canal_tx_0.respuesta  = RESPONDER_T;
		return 1;
	}
	else
	{
		return 0;
	}
}
*/
/************************************************************************************************************
 * @brief  	Leo Resistencia por la entrada de temperatura 													*
 * @brief 	y la mando por puerto serie																		*
 * 																											*
 * @param 	none																							*
 * 																											*   
 * @return	0 -> no hizo nada																				*
 * @return  1 -> mand� los datos!																			*
 ************************************************************************************************************/
/*
inline int Leer_y_Enviar_Resistencia(void)
{
	T_Bytes_a_Word Resistencia_a_Enviar;
	
	if(terminal.medicion == TERMINAL_MIDIENDO)															// Termin� de hacer una medici�n pero no baj� el flag porque est� en modo calibracion T
	{				
		terminal.medicion = TERMINAL_LIBRE;																// Seteo la terminal libre para un nuevo conteo del RTC.
		Resistencia_a_Enviar.word = Resistencia_Medida;													// Guardo el valor a enviar
		cadena_resistencia [CADENA_TEMPERATURA_IND_PARTE_ALTA] = Resistencia_a_Enviar.bytes[1];			// Separo la parte baja del dato
		cadena_resistencia [CADENA_TEMPERATURA_IND_PARTE_BAJA] = Resistencia_a_Enviar.bytes[0];			// de la parte alta (en 2 bytes)
		canal_tx_0.respuesta  = RESPONDER_R;																// Habilito a mandar valor de resistencia
		return 1;																						// Aviso que mand� un dato
	}
	else
	{
		return 0;																						// La funci�n no hizo nada
	}
}
*/
/************************************************************************************************************
 * @brief  	Leo y mando el valor de la medici�n del ADC														*
 * 																											* 
 * @param 	none																							*
 * 																											*   
 * @return	0 -> no hizo nada																				*
 * @return  1 -> mand� los datos!																			*
 ************************************************************************************************************/

inline int Leer_y_Enviar_ADC_Temperatura(void)
{
	T_Bytes_a_DWord Dato_a_Enviar;
	
	if(terminal.medicion == TERMINAL_MIDIENDO)															// Termin� de hacer una medici�n pero no baj� el flag porque est� en modo calibracion T
	{				
		terminal.medicion = TERMINAL_LIBRE;																// Seteo la terminal libre para un nuevo conteo del RTC.

		switch(modo_envio_T)
		{
			case ENVIAR_PENDIENTE:
				cadena_datos[0] = 'p';																	// aclaro que mando la pendiente
				Dato_a_Enviar.dword = Pendiente_Resistencia;											// Primero mando la pendiente usada para medir resistencia
				modo_envio_T = ENVIAR_OFFSET;															// Cambio a env�o de offset
				break;
			case ENVIAR_OFFSET:
				cadena_datos[0] = 'o';																	// mando el offset
				Dato_a_Enviar.dword = Offset_Resistencia;												// Segundo, mando el offset
				modo_envio_T = ENVIAR_TEMPERATURA;														// Por �ltimo, mando datos
				break;
			case ENVIAR_TEMPERATURA:
				cadena_datos[0] = 'd';																	// Byte m�s significativo
				Dato_a_Enviar.dword = (int32_t)PT100PromedioObtenido;									// Guardo el valor a enviar
				modo_envio_T = ENVIAR_TEMPERATURA;														// Env�o datos hasta que vuelva a inicializarse
				break;
			default:
				break;
		}
	
		cadena_datos[1] = Dato_a_Enviar.bytes[3];														// Byte m�s significativo
		cadena_datos[2] = Dato_a_Enviar.bytes[2];														// Byte 2
		cadena_datos[3] = Dato_a_Enviar.bytes[1];														// Byte 3
		cadena_datos[4] = Dato_a_Enviar.bytes[0];														// Byte menos significativo
		canal_tx_0.respuesta  = RESPONDER_DATO;															// Habilito a mandar valor de resistencia
		return 1;																						// Aviso que mand� un dato
	}
	else
	{
		return 0;																						// La funci�n no hizo nada
	}
}

/************************************************************************************************************
 * @brief  	Leo conductividad y preparo cadena para enviar por serie										*
 * 																											* 
 * @param 	none																							*
 * 																											*   
 * @return	0 -> no hizo nada																				*
 * @return  1 -> mand� los datos!																			*
 ************************************************************************************************************/

inline int Leer_y_Enviar_Conductividad(void)
{
	T_Bytes_a_DWord Calibracion_a_Enviar;																// Para enviar pendiente u offset de calibraci�n
	
	if(terminal.medicion == TERMINAL_MIDIENDO)															// Termin� de hacer una medici�n pero no baj� el flag porque est� en modo calibracion T
	{				

		switch(modo_envio_K)
		{
			case ENVIAR_PENDIENTE:
			
				cadena_datos[0] = 'p';																	// aclaro que mando la pendiente
				Calibracion_a_Enviar.dword = Pendiente_Conductividad;									// Primero mando la pendiente usada para medir resistencia
				
				cadena_datos[1] = Calibracion_a_Enviar.bytes[3];										// Byte m�s significativo
				cadena_datos[2] = Calibracion_a_Enviar.bytes[2];										// Byte 2
				cadena_datos[3] = Calibracion_a_Enviar.bytes[1];										// Byte 3
				cadena_datos[4] = Calibracion_a_Enviar.bytes[0];										// Byte menos significativo
				
				modo_envio_K = ENVIAR_OFFSET;															// Cambio a env�o de offset
				terminal.medicion = TERMINAL_LIBRE;														// Seteo la terminal libre para un nuevo conteo del RTC.
				canal_tx_0.respuesta  = RESPONDER_DATO;													// Habilito a mandar valor de resistencia
				break;
				
			case ENVIAR_OFFSET:
			
				cadena_datos[0] = 'o';																	// mando el offset
				Calibracion_a_Enviar.dword = Offset_Conductividad;										// Segundo, mando el offset
								
				cadena_datos[1] = Calibracion_a_Enviar.bytes[3];										// Byte m�s significativo
				cadena_datos[2] = Calibracion_a_Enviar.bytes[2];										// Byte 2
				cadena_datos[3] = Calibracion_a_Enviar.bytes[1];										// Byte 3
				cadena_datos[4] = Calibracion_a_Enviar.bytes[0];										// Byte menos significativo
				
				modo_envio_K = ENVIAR_CORRIENTE;														// Por �ltimo, mando datos
				terminal.medicion = TERMINAL_LIBRE;														// Seteo la terminal libre para un nuevo conteo del RTC.
				canal_tx_0.respuesta  = RESPONDER_DATO;													// Habilito a mandar valor de resistencia
				break;
			
			case ENVIAR_CORRIENTE:
			
				if(canal_tx_0.estado_buffer == BUFFER_VACIO)											// Pregunto si la terminal no tiene una respuesta pendiente
				{
					cadena_datos[0] = 'i';																// guardo el identificador para enviar un valor de corriente
					Calibracion_a_Enviar.dword = I_Integral_Media_Onda_Promediado;						// Guardo la integral de la senoidal de media onda
					
					cadena_datos[1] = Calibracion_a_Enviar.bytes[3];									// Byte m�s significativo
					cadena_datos[2] = Calibracion_a_Enviar.bytes[2];									// Byte 2
					cadena_datos[3] = Calibracion_a_Enviar.bytes[1];									// Byte 3
					cadena_datos[4] = Calibracion_a_Enviar.bytes[0];									// Byte menos significativo
					
					modo_envio_K = ENVIAR_TENSION;														// Por �ltimo, mando datos
					canal_tx_0.respuesta  = RESPONDER_DATO;												// Habilito a mandar valor de resistencia
					terminal.medicion = TERMINAL_LIBRE;													// Seteo la terminal libre para un nuevo conteo del RTC.
				}
				break;
			
			case ENVIAR_TENSION:
			
				if(canal_tx_0.estado_buffer == BUFFER_VACIO)											// Pregunto si la terminal no tiene una respuesta pendiente
				{																			
					cadena_datos[0] = 'v';																// Guardo el tag para enviar tensi�n
					Calibracion_a_Enviar.dword = V_Integral_Media_Onda_Promediado;						// Guardo la integral de la senoidal de media onda
					
					cadena_datos[1] = Calibracion_a_Enviar.bytes[3];									// Byte m�s significativo
					cadena_datos[2] = Calibracion_a_Enviar.bytes[2];									// Byte 2
					cadena_datos[3] = Calibracion_a_Enviar.bytes[1];									// Byte 3
					cadena_datos[4] = Calibracion_a_Enviar.bytes[0];									// Byte menos significativo				
					
					modo_envio_K = ENVIAR_TEMPERATURA;													// Por �ltimo, mando datos
					canal_tx_0.respuesta  = RESPONDER_DATO;												// Habilito a mandar valor de resistencia
					terminal.medicion = TERMINAL_LIBRE;													// Seteo la terminal libre para un nuevo conteo del RTC.
				}
				break;
				
			case ENVIAR_TEMPERATURA:
			
				if(canal_tx_0.estado_buffer == BUFFER_VACIO)											// Pregunto si la terminal no tiene una respuesta pendiente
				{
					cadena_datos[0] = 't';																// Guardo el tag para mandar temperatura calibrada
					Calibracion_a_Enviar.dword = temperaturaProcesada;									// guardo el valor de 32bit de temp. calibrada
					
					cadena_datos[1] = Calibracion_a_Enviar.bytes[3];									// Byte m�s significativo
					cadena_datos[2] = Calibracion_a_Enviar.bytes[2];									// Byte 2
					cadena_datos[3] = Calibracion_a_Enviar.bytes[1];									// Byte 3
					cadena_datos[4] = Calibracion_a_Enviar.bytes[0];									// Byte menos significativo				
					
					modo_envio_K = ENVIAR_CORRIENTE;													// Por �ltimo, mando datos				
					canal_tx_0.respuesta  = RESPONDER_DATO;												// Habilito a mandar valor de resistencia
					terminal.medicion = TERMINAL_LIBRE;													// Seteo la terminal libre para un nuevo conteo del RTC.
				}
				break;
			
			default:
				break;
		}		
		
		return 1;																						// Aviso que mand� un dato
	}
	else
	{
		return 0;																						// La funci�n no hizo nada
	}
	
}
 
/************************************************************************************************************
 * @brief  	Leo una cadena entrante y extraigo un dato de 32 bits											*	
 * 																											*
 * @param 	none 																							*
 * 																											*   
 * @return	0 -> extrajo el dato! 																			*	
 * @return  1 -> error 																						*
 ************************************************************************************************************/

inline int Leer_DWord_Entrante(T_Modbus *ch_in)
{
	T_Bytes_a_DWord dato_a_leer;
	
	if(ch_in->frame[LEER_DATO_IDENTIFICADOR_IND] != LEER_DATO_IDENTIFICADOR)		// Chequeo que la cadena empiece con 'd'
	{
		ch_in->Dato = 0;															// Sino elimino el dato
		ch_in->estado_dato = DATO_INVALIDO;											// Se�alizo como inv�lido
		return 1;																	// Y salgo de la funci�n
	}

	dato_a_leer.bytes[3] = ch_in->frame[LEER_DATO_BYTE3];							// Si llegu� hasta ac�, est� todo OK. Copio la parte alta del dato
	dato_a_leer.bytes[2] = ch_in->frame[LEER_DATO_BYTE2];							// Copio la parte alta del dato
	dato_a_leer.bytes[1] = ch_in->frame[LEER_DATO_BYTE1];							// Copio la parte alta del dato
//	dato_a_leer.bytes[0] = ch_in->frame[LEER_DATO_BYTE0];							// Copio la parte alta del dato
	
	ch_in->Dato = (int32_t)dato_a_leer.dword; 										// paso a la variable del canal de recepci�n.
	
	return 0;																		// devuelvo �sito! ;)
	
}

/************************************************************************************************************
 * @brief  	Leo una cadena entrante y extraigo 2 datos de 16 bits c/u										*	
 * 																											*
 * @param 	none 																							*
 * 																											*   
 * @return	0 -> extrajo el dato! 																			*	
 * @return  1 -> error 																						*
 ************************************************************************************************************/

inline int Leer_Words_Entrantes(T_Modbus* ch_in)
{
	
	T_Bytes_a_Word dato_a_leer_1;																	// Primer dato que extraigo
	T_Bytes_a_Word dato_a_leer_2;																	// Segundo dato que extraigo
	
	if(ch_in->frame[LEER_DATO_IDENTIFICADOR_IND] != LEER_DATO_IDENTIFICADOR)						// Chequeo que la cadena empiece con 'd'
	{
		ch_in->Dato = 0;																			// Sino elimino el dato
		ch_in->estado_dato = DATO_INVALIDO;															// Se�alizo como inv�lido
		return 1;																					// Y salgo de la funci�n
	}

	dato_a_leer_1.bytes[0] = ch_in->frame[1];														// Si llegu� hasta ac�, est� todo OK. Copio la parte alta del dato 1
	dato_a_leer_1.bytes[1] = ch_in->frame[2];														// Copio la parte baja del dato 1
	
	dato_a_leer_2.bytes[0] = ch_in->frame[3];														// Copio la parte alta del dato 2
//	dato_a_leer_2.bytes[1] = ch_in->frame[4];														// Copio la parte baja del dato 2
	
	ch_in->dato_1 = (int16_t)dato_a_leer_1.word; 													// paso a la variable del canal de recepci�n.
	ch_in->dato_2 = (int16_t)dato_a_leer_2.word; 													// paso a la variable del canal de recepci�n.
	
	return 0;																						// devuelvo �sito! ;)
	
}

/************************************************************************************************************
 * @brief  	Armo la cadena a enviar a partir de las mediciones.  											*
 * 																											* 
 * @param 	none 																							*
 * 																											*
 * @return	0 -> extrajo el dato! 																			*
 * @return  1 -> error 																						*
 ************************************************************************************************************/

inline int Leer_y_Enviar_Mediciones(void)
{
	T_Bytes_a_Word conductividad_a_enviar;
	T_Bytes_a_Word temperatura_a_enviar;
	
	if(terminal.medicion == TERMINAL_MIDIENDO)															// Termin� de hacer una medici�n pero no baj� el flag porque est� en modo calibracion T
	{

		cadena_datos[0] = 'd';																			// Guardo el tag para enviar tensi�n
		
		conductividad_a_enviar.word = conductividadProcesada;											// Valor calibrado de conductividad
		temperatura_a_enviar.word = temperaturaProcesada;												// Valor calibrado de temperatura
		
		cadena_datos[1] = conductividad_a_enviar.bytes[0];												// Byte m�s significativo de conductividad
		cadena_datos[2] = conductividad_a_enviar.bytes[1];												// Byte menos significativo de conductividad
		
		cadena_datos[3] = temperatura_a_enviar.bytes[0];												// Byte m�s significativo de temperatura
		cadena_datos[4] = temperatura_a_enviar.bytes[1];												// Byte menos significativo de temperatura	
		
		canal_tx_0.respuesta = RESPONDER_DATO;
		terminal.medicion = TERMINAL_LIBRE;
	}
	else
	{
		return 1;
	}	
	
	return 0;	
}

/************************************************************************************************************
 * @brief  	Inicializa los contadores de la funci�n Enviar_Lista_Archivos_Log()								*
 * 																											* 
 * @param 	none 																							*
 * 																											*
 * @return	0 -> mando todo bien! 																			*
 * @return  1 -> error 																						*
 ************************************************************************************************************/

inline int Init_Enviar_Lista_Archivos_Log(void)
{
	ind_acumulado = 0;
	ind_archivo = 0;
	return 0;
}
 
/************************************************************************************************************
 * @brief  	Mando la lista de archivos que se pueden descargar  											*
 * 																											* 
 * @param 	none 																							*
 * 																											*
 * @return	0 -> mando todo bien! 																			*
 * @return  1 -> error 																						*
 ************************************************************************************************************/

inline int Enviar_Lista_Archivos_Log(void)
{
	uint8_t* nombre_ptr;																				// Puntero al inicio del nombre de archivo actual
	uint8_t long_nombre = 0;																			// Longitud del nombre de archivo actual
	uint8_t i;

	if(canal_tx_0.estado_buffer == BUFFER_VACIO)														// El canal de salida est� libre, mand� todos los datos
	{
		if(ind_archivo < numNames)
		{
			ind_acumulado +=  LONG_SEPARADOR_NOMBRES;													// Posiciono 4 adelante de la �ltima lectura
			nombre_ptr = ((uint8_t*) &names[ind_acumulado]);											// Puntero donde arranca el frame
			long_nombre = nameLengths[ind_archivo] - LONG_SEPARADOR_NOMBRES; 							// Longitud del nombre del archivo
			ind_acumulado += long_nombre;																// Le agrego el largo de la cadena actual para la pr�x.
			
			canal_tx_0.frame[0] = '*';																	// Se�alizo que estoy mandando una cadena
			
			CopiarArray_8_bits(canal_tx_0.frame + 1, nombre_ptr, long_nombre);							// Copio el nombre del archivo
			
			canal_tx_0.respuesta = NO_RESPONDER;														// Se�alizo para la funci�n que devuelve los datos
			canal_tx_0.len_cadena = long_nombre + 1;													// Es un caracter m�s porque tiene el '*' para se�alizar
			canal_tx_0.ind = 0;																			// Inicializo el �ndice para recorrer el frame
			canal_tx_0.estado_buffer = BUFFER_LLENO;													// Se�alizo BUFFER_LLENO
			
			Iniciar_Transmision_Paquete_UART0(&canal_tx_0);												// Inicio la transmisi�n del paquete por (T1)
			ind_archivo++;																				// Incremento para pasar al pr�ximo archivo
			return 1;
		}
		else if(ind_archivo == numNames)
		{
			for(i = 0;i < 10;i++)
				canal_tx_0.frame[i] = '!';																// Se�alizo que termin� de mandar cadenas
			
			canal_tx_0.respuesta = NO_RESPONDER;														// Se�alizo para la funci�n que devuelve los datos
			canal_tx_0.len_cadena = 10;																	// Es un caracter m�s porque tiene el '*' para se�alizar
			canal_tx_0.ind = 0;																			// Inicializo el �ndice para recorrer el frame
			canal_tx_0.estado_buffer = BUFFER_LLENO;													// Se�alizo BUFFER_LLENO
			
			Iniciar_Transmision_Paquete_UART0(&canal_tx_0);												// Inicio la transmisi�n del paquete por (T1)
			
			return 0;
		}
		else if(ind_archivo > numNames)																	// Pregunto si llegu� al final.
		{
			return 0;
		}
	}	
	else
	{
		
		return 2;
	}
																				 
	return 0;
}

/************************************************************************************************************
 * @brief  	Extraigo un nombre de archivo de la lista general	  											*
 * 																											* 
 * @param 	none 																							*
 * 																											*
 * @return	0 -> extrajo todo bien! 																		*
 * @return  1 -> error 																						*
 ************************************************************************************************************/
 
int Extraer_Nombre_de_Archivo(int8_t ind_arch, char* array_dst, const uint8_t* path)
{
	uint8_t* nombre_ptr;																				// Puntero al inicio del nombre de archivo actual
	uint8_t long_nombre = 0;																			// Longitud del nombre de archivo actual
	uint8_t len_path = 0;																				// Longitud del nombre de archivo actual	
	uint8_t i;
	uint8_t ind_acumulado;
	
	if(ind_arch < 0) 																					// Si el �ndice est� fuera de rango
		return 1;																						// Respondo con error
	else if (ind_arch > (int8_t) numNames)
		return 1;
	else
	{
		ind_acumulado =  LONG_SEPARADOR_NOMBRES;														// Posiciono 4 adelante de la �ltima lectura		
		
		for(i = 0;i < ind_arch; i++)																	// Recorro los nombres de archivo
			ind_acumulado +=  nameLengths[ind_arch];													// sumo el largo del nombre actual mas el separador sgte.	
		
		nombre_ptr = ((uint8_t*) &names[ind_acumulado]);												// Puntero donde arranca el nombre del archivo
		long_nombre = nameLengths[ind_arch] - LONG_SEPARADOR_NOMBRES; 									// Longitud del nombre del archivo		
		len_path = strlen((const char*)path);
		
		CopiarArray_8_bits((uint8_t*)array_dst, path, len_path);										// Copio el nombre del archivo
		array_dst[len_path] = '\\';																		// separo el directorio
		
		CopiarArray_8_bits((uint8_t*)array_dst + len_path + 1, nombre_ptr, long_nombre);				// Copio el nombre del archivo
		array_dst[len_path + long_nombre + 1] = '\0';													// Final de la cadena
		
		return 0;	
	}	
}

/************************************************************************************************************
 *	@brief 		Variables de calibraci�n medidas el 09.04.2012, en la placa Master							*
 * 																											*
 * 	@params 	none																						*
 * 																											* 		
 * 	@returns 	none																						*
 * 																											*
 ************************************************************************************************************/
 
int Precalibracion_Master(void)
{
	Pendiente_Resistencia = 10524;
	Offset_Resistencia = 975;
	Pendiente_Conductividad = 6032;
	Offset_Conductividad = -2;
	return 0;
} 

/************************************************************************************************************
 *	@brief 		Variables de calibraci�n medidas el 09.04.2012, en la placa Master							*
 * 																											*
 * 	@params 	none																						*
 * 																											* 		
 * 	@returns 	none																						*
 * 																											*
 ************************************************************************************************************/

int Precalibracion_Slave(void)
{
	Pendiente_Resistencia = 11453;
	Offset_Resistencia = 959;
	Pendiente_Conductividad = 5931;
	Offset_Conductividad = -2;
	return 0;
}

/************************************************************************************************************
 *		 									Final del Archivo												*
 ************************************************************************************************************/
