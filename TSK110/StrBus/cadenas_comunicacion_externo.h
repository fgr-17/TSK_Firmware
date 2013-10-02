/*******************************************************************************************
 *
 * 	Archivo : cadenas_comunicacion_externo.h
 * 
 * 	Descripci�n: Inclusi�n de los archivos
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

extern const uint8_t cadena_estoy 	  [CANT_BYTES_BUFFER]; 								// Comienzo la comunicaci�n
extern const uint8_t cadena_error_crc [CANT_BYTES_BUFFER];								// El frame tra�a un error de CRC
extern const uint8_t cadena_error_cmd [CANT_BYTES_BUFFER];								// El frame no ten�a errores pero el comando no es v�lido
extern const uint8_t cadena_error_ubq [CANT_BYTES_BUFFER];								// El frame no ten�a errores, el comando es v�lido, pero est� fuera de contexto
extern const uint8_t cadena_error_noi [CANT_BYTES_BUFFER];								// Se envi� un comando pero la terminal no est� conectada
extern const uint8_t cadena_error_con [CANT_BYTES_BUFFER];								// Error de comunicaci�n. Se intenta iniciar una comunicaci�n ya inicializada
extern const uint8_t cadena_error_dat [CANT_BYTES_BUFFER];								// Dato err�neo
extern const uint8_t cadena_error_unk [CANT_BYTES_BUFFER];								// Error desconocido

extern uint8_t cadena_hora [CANT_BYTES_BUFFER];											// Env�o hora, minuto y segundo	

extern uint8_t cadena_temperatura[CANT_BYTES_BUFFER];									// Env�o el valor de temperatura 
extern uint8_t cadena_resistencia[CANT_BYTES_BUFFER];									// Env�o el valor de resistencia
extern uint8_t cadena_conductividad[CANT_BYTES_BUFFER];									// Env�o el valor de Conductividad

extern uint8_t cadena_datos[CANT_BYTES_BUFFER];											// Env�o de datos en gral.

#endif /*CADENAS_COMUNICACION_EXTERNO_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
