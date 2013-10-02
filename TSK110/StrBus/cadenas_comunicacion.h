/*******************************************************************************************
 *
 *  @file 	cadenas_comunicacion.h
 * 
 * 	@brief 	Definici�n de todas las cadenas de comunicaci�n.
 * 
 * 	@author Roux Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 		11/2011
 * 	@company	NEROX SRL
 * 
 *******************************************************************************************/
 
#ifndef CADENAS_COMUNICACION_H_
#define CADENAS_COMUNICACION_H_

#define N_CADENA_CMD		26



const uint8_t lista_de_comandos[N_CADENA_CMD][CANT_BYTES_BUFFER] = {
	
	"estas",						// Inicio de la comunicaci�n				

	"confi",						// Modo de configuraci�n

	"inima",						// Inicializo el equipo como master
	"Inims",						// Inicializo la medici�n del master --> ES LA RESPUESTA OK DEL SLAVE A "inims"
	"finmm",						// Finalizo la medici�n del master

	"inims",						// Inicializo la medici�n del slave
	"finms",						// Finalizo la medici�n del slave

	"inick",						// Inicio la calibraci�n de conductividad
	"finck",						// Finalizo la calibraci�n de conductividad
	"canck",						// Cancelo la calibraci�n de conductividad
	"okack",						// La calibraci�n de conductividad est� OK
	"faick",						// La calibraci�n de conductividad fall�

	"inict",						// Inicio la calibraci�n de temperatura
	"finct",						// Finalizo la calibraci�n de temperatura
	"canct",						// Cancelo la calibraci�n de temperatura
	"okact",						// La calibraci�n de temperatura est� OK
	"faict",						// La calibraci�n de temperatura fall�
	
	"arcok", 						// La PC confirm� haber recibido un dato
	
	"inide",						// Inicio la descarga de datos de archivo (logger) 
	"sefyh", 						// Seteo fecha y hora al equipo

	"reumb",						// Recibo nivel de alarma
	"reala",						// Recibo nivel de umbral
	
	"inire", 						// Inicializo al Master como repetidor
	"finre", 						// Finalizo al Master como repetidor
	
	"dummy",
	
};

const uint8_t cadena_estoy [CANT_BYTES_BUFFER] = "estoy";									// Comienzo la comunicaci�n
const uint8_t cadena_error_crc [CANT_BYTES_BUFFER] = "ercrc";								// El frame tra�a un error de CRC
const uint8_t cadena_error_cmd [CANT_BYTES_BUFFER] = "ercmd";								// El frame no ten�a errores pero el comando no es v�lido
const uint8_t cadena_error_ubq [CANT_BYTES_BUFFER] = "erubq";								// El frame no ten�a errores, el comando es v�lido, pero est� fuera de contexto
const uint8_t cadena_error_noi [CANT_BYTES_BUFFER] = "ernoi";								// Se envi� un comando pero la terminal no est� conectada
const uint8_t cadena_error_con [CANT_BYTES_BUFFER] = "ercon";								// Error de comunicaci�n. Se intenta iniciar una comunicaci�n ya inicializada
const uint8_t cadena_error_dat [CANT_BYTES_BUFFER] = "erdat";								// Dato err�neo
const uint8_t cadena_error_unk [CANT_BYTES_BUFFER] = "erunk";								// Error desconocido

uint8_t cadena_hora [CANT_BYTES_BUFFER] = "-hms-";											// Env�o hora, minuto y segundo	

uint8_t cadena_temperatura[CANT_BYTES_BUFFER] = "T---T";									// Env�o el valor de temperatura	
uint8_t cadena_resistencia[CANT_BYTES_BUFFER] = "R---R";									// Env�o el valor de resistencia
uint8_t cadena_conductividad[CANT_BYTES_BUFFER] = "K---K";									// Env�o el valor de Conductividad

uint8_t cadena_datos[CANT_BYTES_BUFFER] = "d----";											// Env�o de datos en gral.

char cadena_archivos[LARGO_BUFFER_FRAME];													// Env�o nombres de archivo a descargar

#endif /*CADENAS_COMUNICACION_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
