/*******************************************************************************************
 *
 *  @file 	cadenas_comunicacion.h
 * 
 * 	@brief 	Definición de todas las cadenas de comunicación.
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
	
	"estas",						// Inicio de la comunicación				

	"confi",						// Modo de configuración

	"inima",						// Inicializo el equipo como master
	"Inims",						// Inicializo la medición del master --> ES LA RESPUESTA OK DEL SLAVE A "inims"
	"finmm",						// Finalizo la medición del master

	"inims",						// Inicializo la medición del slave
	"finms",						// Finalizo la medición del slave

	"inick",						// Inicio la calibración de conductividad
	"finck",						// Finalizo la calibración de conductividad
	"canck",						// Cancelo la calibración de conductividad
	"okack",						// La calibración de conductividad está OK
	"faick",						// La calibración de conductividad falló

	"inict",						// Inicio la calibración de temperatura
	"finct",						// Finalizo la calibración de temperatura
	"canct",						// Cancelo la calibración de temperatura
	"okact",						// La calibración de temperatura está OK
	"faict",						// La calibración de temperatura falló
	
	"arcok", 						// La PC confirmó haber recibido un dato
	
	"inide",						// Inicio la descarga de datos de archivo (logger) 
	"sefyh", 						// Seteo fecha y hora al equipo

	"reumb",						// Recibo nivel de alarma
	"reala",						// Recibo nivel de umbral
	
	"inire", 						// Inicializo al Master como repetidor
	"finre", 						// Finalizo al Master como repetidor
	
	"dummy",
	
};

const uint8_t cadena_estoy [CANT_BYTES_BUFFER] = "estoy";									// Comienzo la comunicación
const uint8_t cadena_error_crc [CANT_BYTES_BUFFER] = "ercrc";								// El frame traía un error de CRC
const uint8_t cadena_error_cmd [CANT_BYTES_BUFFER] = "ercmd";								// El frame no tenía errores pero el comando no es válido
const uint8_t cadena_error_ubq [CANT_BYTES_BUFFER] = "erubq";								// El frame no tenía errores, el comando es válido, pero está fuera de contexto
const uint8_t cadena_error_noi [CANT_BYTES_BUFFER] = "ernoi";								// Se envió un comando pero la terminal no está conectada
const uint8_t cadena_error_con [CANT_BYTES_BUFFER] = "ercon";								// Error de comunicación. Se intenta iniciar una comunicación ya inicializada
const uint8_t cadena_error_dat [CANT_BYTES_BUFFER] = "erdat";								// Dato erróneo
const uint8_t cadena_error_unk [CANT_BYTES_BUFFER] = "erunk";								// Error desconocido

uint8_t cadena_hora [CANT_BYTES_BUFFER] = "-hms-";											// Envío hora, minuto y segundo	

uint8_t cadena_temperatura[CANT_BYTES_BUFFER] = "T---T";									// Envío el valor de temperatura	
uint8_t cadena_resistencia[CANT_BYTES_BUFFER] = "R---R";									// Envío el valor de resistencia
uint8_t cadena_conductividad[CANT_BYTES_BUFFER] = "K---K";									// Envío el valor de Conductividad

uint8_t cadena_datos[CANT_BYTES_BUFFER] = "d----";											// Envío de datos en gral.

char cadena_archivos[LARGO_BUFFER_FRAME];													// Envío nombres de archivo a descargar

#endif /*CADENAS_COMUNICACION_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
