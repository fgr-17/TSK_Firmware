/*******************************************************************************************
 *
 * 	Archivo : cadenas_comunicacion_externo.h
 * 
 * 	Descripción: Inclusión de los archivos
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 11/2011
 * 
 *******************************************************************************************/
 
#ifndef CADENAS_COMUNICACION_EXTERNO_H_
#define CADENAS_COMUNICACION_EXTERNO_H_

#define N_CADENA_CMD		12

// extern const uint8_t lista_de_comandos[N_CADENA_CMD][CANT_BYTES_BUFFER];				// Array de strings con los comandos posibles

extern const uint8_t cadena_estoy 	  [CANT_BYTES_BUFFER]; 								// Comienzo la comunicación
extern const uint8_t cadena_error_crc [CANT_BYTES_BUFFER];								// El frame traía un error de CRC
extern const uint8_t cadena_error_cmd [CANT_BYTES_BUFFER];								// El frame no tenía errores pero el comando no es válido
extern const uint8_t cadena_error_ubq [CANT_BYTES_BUFFER];								// El frame no tenía errores, el comando es válido, pero está fuera de contexto
extern const uint8_t cadena_error_noi [CANT_BYTES_BUFFER];								// Se envió un comando pero la terminal no está conectada
extern const uint8_t cadena_error_con [CANT_BYTES_BUFFER];								// Error de comunicación. Se intenta iniciar una comunicación ya inicializada
extern const uint8_t cadena_error_dat [CANT_BYTES_BUFFER];								// Dato erróneo
extern const uint8_t cadena_error_unk [CANT_BYTES_BUFFER];								// Error desconocido

extern uint8_t cadena_hora [CANT_BYTES_BUFFER];											// Envío hora, minuto y segundo	

extern uint8_t cadena_temperatura[CANT_BYTES_BUFFER];									// Envío el valor de temperatura 
extern uint8_t cadena_resistencia[CANT_BYTES_BUFFER];									// Envío el valor de resistencia
extern uint8_t cadena_conductividad[CANT_BYTES_BUFFER];									// Envío el valor de Conductividad

extern uint8_t cadena_datos[CANT_BYTES_BUFFER];											// Envío de datos en gral.

#endif /*CADENAS_COMUNICACION_EXTERNO_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
