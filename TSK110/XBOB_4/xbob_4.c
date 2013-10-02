/********************************************************************************************************
 *
 * 	Archivo : xbob_4.c
 * 
 * 	Descripción: software de manejo de la placa controladora de TV XBOB-4
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 12/2011
 * 
 ********************************************************************************************************/
/********************************************************************************************************
 * 											Inclusión de archivos										*	
 ********************************************************************************************************/ 

#include <msp430f5529.h>
#include <stdint.h>

#include <strbus.h>
#include <xbob_4.h>

#include <uart.h>

#include <xbob_4_cadenas.h>
#include <adc.h>

#include <Temperatura.h>
#include <Conductividad.h>

#include <strbus.h>
#include <HAL_RTC.h>
#include <modos.h>

/********************************************************************************************************
 * 											Prototipos de funciones										*
 ********************************************************************************************************/

int Inicializar_XBOB(void);

void Test_Comunicacion_XBOB_4(void);
void Cartel_Inicio(void);

int Enviar_Medicion_Binario(void);
int Mostrar_Mediciones(void);
int Rotulos_Mediciones(void);

void Enviar_Comando_Simple(T_Comandos_Simples comando);
void Enviar_CSI(void);

int my_itoa(int numero, char* cadena, uint8_t decimales);
int Mover_Cursor(char* fila, char* columna);

/********************************************************************************************************
 * 											Variables Globales											*
 ********************************************************************************************************/

volatile uint8_t caracteres_xbob_4 [LONGITUD_BUFFER_CARACTERES];											// buffer de caracteres a mostrar
const uint8_t comandos_simples [CANTIDAD_COMANDOS_SIMPLES] = {XBOB4_CR, XBOB4_LF, XBOB4_BS, XBOB4_FF};		// lista de comandos a enviar
T_Modbus* canal_salida_xbob;																				// canal que uso para mandar datos

/********************************************************************************************************
 * 											Implementación de funciones									*
 ********************************************************************************************************/

/********************************************************************************************************
 * 
 *	@brief 		Inicializo el puntero por donde envío los datos.		
 * 
 * 	@params 	F	Fila deseada
 *  @params		C	Columna deseada
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

int Inicializar_XBOB(void)
{
	canal_salida_xbob = &canal_tx_0;												// Salgo por el canal TX1.
	return 0;	
} 
 
/********************************************************************************************************
 *	@brief 		Muevo el cursor a una determinada posición (F,C)		
 * 
 * 	@params 	F	Fila deseada
 *  @params		C	Columna deseada
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

int Mover_Cursor(char* fila, char* columna)
{
	#define OFFSET_CSI	2
	
	uint8_t i, j;
	
	canal_salida_xbob->frame[0] = 0x1B;
	canal_salida_xbob->frame[1] = '[';
	
	i = 0;
	while(fila[i] != '*')
	{
		canal_salida_xbob->frame[OFFSET_CSI + i] = fila[i];
		i++;
	}
	
	canal_salida_xbob->frame[OFFSET_CSI + i] = ';';
	
	i++;
	j = 0;
	
	while(columna[j] != '*')
	{
		canal_salida_xbob->frame[OFFSET_CSI + i] = columna[j];
		i++;
		j++;
	}
	
	canal_salida_xbob->frame[OFFSET_CSI + i] = 'H';
	i++;
	
	canal_salida_xbob->len_cadena = OFFSET_CSI + i;
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;
	canal_salida_xbob->ind = 0;
	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);		
	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);
	
	return 0;	
}

/********************************************************************************************************
 *	@brief 		Muestro los rótulos de las mediciones		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

int Rotulos_Mediciones(void)
{
	Mover_Cursor(CADENA_COND_FILA, CADENA_COND_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame, cadena_cond, CADENA_COND_LEN);				// Copio la cadena a transmitir
	canal_salida_xbob->len_cadena = CADENA_COND_LEN;										// Cargo el largo del comando CSI		
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI	
	
	Mover_Cursor(CADENA_TEMP_FILA, CADENA_TEMP_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame, cadena_temp, CADENA_TEMP_LEN);				// Copio la cadena a transmitir
	canal_salida_xbob->len_cadena = CADENA_TEMP_LEN; 										// Cargo el largo del comando CSI		
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	
	return 0;
	
}

/********************************************************************************************************
 *	@brief 		Envío las mediciones
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

int Enviar_Medicion_Binario(void)
{
	uint8_t i;
	
	uint8_t alarma_master = 0;																// Determina si se activa o no la alarma del master
	uint8_t alarma_slave = 0;																// Determina si se activa o no la alarma del slave
	
	T_Bytes_a_Word k1_frame;																// Convierto los bytes de k1 al valor final de 16 bits
	T_Bytes_a_Word t1_frame;																// Convierto los bytes de t1 al valor final de 16 bits
	T_Bytes_a_Word k2_frame;																// Convierto los bytes de k2 al valor final de 16 bits
	T_Bytes_a_Word t2_frame;																// Convierto los bytes de t2 al valor final de 16 bits	
	
	//////////////////////////////////////////////////////////////////////////////////////////
	//									SELECCIONO ALARMAS									//
	//////////////////////////////////////////////////////////////////////////////////////////	
	
	if(terminal.alarma_activa)																// Pregunto si la alarma está activa
	{
		if(conductividadProcesada >= terminal.nivel_alarma)				alarma_master = 1;	// Activo la alarma del master
		if(conductividadProcesada_recibida >= terminal.nivel_alarma)	alarma_slave = 1;	// Activo la alarma del slave
	}
	
	k1_frame.word = conductividadProcesada;													// Guardo K1 como variable de 32 bits
	t1_frame.word = temperaturaProcesada;													// Guardo T1 como variable de 32 bits
	k2_frame.word = conductividadProcesada_recibida;										// Guardo K2 como variable de 32 bits
	t2_frame.word = temperaturaProcesada_recibida;											// Guardo T2 como variable de 32 bits
	
	for(i = 0; i < MEDICIONES_SIZE; i++)													// Recorro los 4 bytes de las mediciones de K y T del TSK100
	{
		canal_salida_xbob->frame[i + K1_BYTE_INI] = k1_frame.bytes[i];						// Extraigo los bytes de la medición de K1
		canal_salida_xbob->frame[i + T1_BYTE_INI] = t1_frame.bytes[i];						// Extraigo los bytes de la medición de T1
		canal_salida_xbob->frame[i + K2_BYTE_INI] = k2_frame.bytes[i];						// Extraigo los bytes de la medición de K2
		canal_salida_xbob->frame[i + T2_BYTE_INI] = t2_frame.bytes[i];						// Extraigo los bytes de la medición de T2
	}
	
	if(alarma_master)	
		canal_salida_xbob->frame[AM_BYTE_INI] = 0x01;
	else 
		canal_salida_xbob->frame[AM_BYTE_INI] = 0x00;
	
	if(alarma_slave)	
		canal_salida_xbob->frame[AS_BYTE_INI] = 0x01;
	else
		canal_salida_xbob->frame[AS_BYTE_INI] = 0x00;
	
	if(timeout_Slave.slave_vivo == FALSE)	
		canal_salida_xbob->frame[AS_BYTE_INI] |= 0x02;										// Si el slave está muerto, lo indico con un bit
	
	canal_salida_xbob->len_cadena = FRAME_MEDICIONES_LEN;									// Guardo el largo de la cadena a enviar
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI

	return 0;
}

/********************************************************************************************************
 *	@brief 		Muestro los resultados de las mediciones
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

int Mostrar_Mediciones(void)
{
	uint8_t largo_med_temporal;
	char med[8];
	const char blanco_K [9] = "     uS ";
	const char blanco_T [9] = "     C "; 
	const char no_conectado [15] = "-No conectado- ";
	
	const char alarma[3] = "AL";
	const char borra_alarma[3] = "  ";
	uint8_t alarma_master = 0;																// Determina si se activa o no la alarma del master
	uint8_t alarma_slave = 0;																// Determina si se activa o no la alarma del slave
	
	//////////////////////////////////////////////////////////////////////////////////////////
	//									SELECCIONO ALARMAS									//
	//////////////////////////////////////////////////////////////////////////////////////////	
	
	if(terminal.alarma_activa)																// Pregunto si la alarma está activa
		if(conductividadProcesada >= terminal.nivel_alarma)				alarma_master = 1;	// Activo la alarma del master
	
	//////////////////////////////////////////////////////////////////////////////////////////
	//									MUESTRO ALARMAS										//
	//////////////////////////////////////////////////////////////////////////////////////////
	
	Mover_Cursor(ALARMA_FILA, CADENA_COND_COL);												// Ubico el cursor
	if(alarma_master)																		// si tengo que disparar la alarma
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) alarma, 3);			// copio cartel de alarma
	else
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) borra_alarma, 3);		// sino borro el cartel		
	canal_salida_xbob->len_cadena = 3; 														// Cargo el largo del comando CSI
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	
	//////////////////////////////////////////////////////////////////////////////////////////
	//							MUESTRO MEDICIONES DEL MASTER								//
	//////////////////////////////////////////////////////////////////////////////////////////
	
	Mover_Cursor(CADENA_COND_FILA, CADENA_COND_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) blanco_K, 8);				// Copio la cadena a transmitir
	canal_salida_xbob->len_cadena = 8; 														// Cargo el largo del comando CSI
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI

	largo_med_temporal = my_itoa(conductividadProcesada, med, 0);
	Mover_Cursor(CADENA_COND_FILA, CADENA_COND_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) med, 4);					// Copio la cadena a transmitir
 	canal_salida_xbob->len_cadena = largo_med_temporal; 									// Cargo el largo del comando CSI		
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI

	Mover_Cursor(CADENA_TEMP_FILA, CADENA_TEMP_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) blanco_T, 8);				// Copio la cadena a transmitir
	canal_salida_xbob->len_cadena = 8; 														// Cargo el largo del comando CSI
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
		
	largo_med_temporal = my_itoa(temperaturaProcesada, med, 1);
	Mover_Cursor(CADENA_TEMP_FILA, CADENA_TEMP_COL);										// Ubico el cursor
	CopiarArray_8_bits(canal_salida_xbob->frame, (const uint8_t*)med, 4);					// Copio la cadena a transmitir
	canal_salida_xbob->len_cadena = largo_med_temporal; 									// Cargo el largo del comando CSI		
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
		
	//////////////////////////////////////////////////////////////////////////////////////////
	//							MUESTRO MEDICIONES DEL SLAVE								//
	//////////////////////////////////////////////////////////////////////////////////////////
			
	if(timeout_Slave.slave_vivo == TRUE)
	{
		
		if((terminal.alarma_activa) && (conductividadProcesada_recibida >= terminal.nivel_alarma))	alarma_slave = 1;	// Activo la alarma del slave
		
		Mover_Cursor(ALARMA_FILA, CADENA_COND2_COL);											// Ubico el cursor	
		
		if(alarma_slave	)
			CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) alarma, 3);			// Copio la cadena a transmitir
		else
			CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) borra_alarma, 3);		// Copio la cadena a transmitir
		
		canal_salida_xbob->len_cadena = 3; 														// Cargo el largo del comando CSI
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
		
		Mover_Cursor(CADENA_COND2_FILA, CADENA_COND2_COL);										// Ubico el cursor
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) blanco_K, 8);				// Copio la cadena a transmitir
		canal_salida_xbob->len_cadena = 8; 														// Cargo el largo del comando CSI
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	
		Mover_Cursor(CADENA_TEMP2_FILA, CADENA_TEMP2_COL);										// Ubico el cursor
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) blanco_T, 8);				// Copio la cadena a transmitir
		canal_salida_xbob->len_cadena = 8; 														// Cargo el largo del comando CSI
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
		
		largo_med_temporal = my_itoa(conductividadProcesada_recibida, med, 0);	
		Mover_Cursor(CADENA_COND2_FILA, CADENA_COND2_COL);										// Ubico el cursor
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) med, 4);					// Copio la cadena a transmitir
	 	canal_salida_xbob->len_cadena = largo_med_temporal; 									// Cargo el largo del comando CSI		
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI	
		
		largo_med_temporal = my_itoa(temperaturaProcesada_recibida, med, 1);
		Mover_Cursor(CADENA_TEMP2_FILA, CADENA_TEMP2_COL);										// Ubico el cursor
		CopiarArray_8_bits(canal_salida_xbob->frame, (const uint8_t*)med, 4);					// Copio la cadena a transmitir
		canal_salida_xbob->len_cadena = largo_med_temporal; 									// Cargo el largo del comando CSI		
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	}
	else											
	{
		Mover_Cursor(CADENA_COND2_FILA, CADENA_COND2_COL);										// Ubico el cursor
		CopiarArray_8_bits(canal_salida_xbob->frame,(const uint8_t*) no_conectado, 15);			// Copio la cadena a transmitir
		canal_salida_xbob->len_cadena = 15; 													// Cargo el largo del comando CSI
		Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);									// Mando el primer caracter
		while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	}	
	
	return 0;
}

/********************************************************************************************************
 *	@brief 		Envio el comando CSI para comenzar una transmisión		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

void Enviar_CSI(void)
{
	canal_salida_xbob->frame[0] = XBOB4_CSI_0;													// Cargo el primer byte del comando CSI
	canal_salida_xbob->frame[1] = XBOB4_CSI_1;													// Cargo el segundo byte del comando CSI
	canal_salida_xbob->frame[2] = XBOB4_CSI_2;													// byte dummy
	
	canal_salida_xbob->ind = 0;																	// Inicializo el índice
	canal_salida_xbob->len_cadena = LEN_XBOB4_CMD_CSI;											// Cargo el largo del comando CSI
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;											// Señalizo como buffer lleno
	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);
}

/********************************************************************************************************
 *	@brief 		Envio el comando CSI para comenzar una transmisión		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

void Enviar_Comando_Simple(T_Comandos_Simples comando)
{
	if((uint8_t)comando > CANTIDAD_COMANDOS_SIMPLES)
		return;
		
	canal_salida_xbob->frame[0] = comandos_simples[(uint8_t)comando];						// Cargo el primer byte del comando CSI
	
	canal_salida_xbob->ind = 0;																// Inicializo el índice
	canal_salida_xbob->len_cadena = 1;														// Cargo el largo del comando CSI
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;										// Señalizo como buffer lleno
	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);
}

/********************************************************************************************************
 *	@brief 		Prueba de la pantalla		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/
 
void Test_Comunicacion_XBOB_4(void)
{
	Enviar_CSI();
	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
		
	CopiarArray_8_bits(canal_salida_xbob->frame, cadena_test, LEN_XBOB4_CADENA_TEST);		// Copio la cadena a transmitir
	
	canal_salida_xbob->ind = 0;																// Inicializo el índice
	canal_salida_xbob->len_cadena = LEN_XBOB4_CADENA_TEST;									// Cargo el largo del comando CSI
	
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;										// Señalizo como buffer lleno	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI

	canal_salida_xbob->estado_buffer = BUFFER_LLENO;										// Señalizo como buffer lleno
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI

	Enviar_Comando_Simple(CR);	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);								// Espero que se haya transmitido el CSI
	Enviar_Comando_Simple(LF);
}

/********************************************************************************************************
 *	@brief 		Cartel de Inicio		
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************************************/

void Cartel_Inicio(void)
{
	Enviar_CSI(); while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);						// Envío comienzo de transmisión
	Enviar_Comando_Simple(FF); while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);			// Limpio la pantalla
		
	CopiarArray_8_bits(canal_salida_xbob->frame, cadena_test, LEN_XBOB4_CADENA_TEST);			// Copio la cadena a transmitir
	
	canal_salida_xbob->ind = 0;																	// Inicializo el índice
	canal_salida_xbob->len_cadena = LEN_XBOB4_CADENA_TEST;										// Cargo el largo del comando CSI
	
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;											// Señalizo como buffer lleno	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);									// Espero que se haya transmitido el CSI

	Enviar_Comando_Simple(CR); while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);			// Carriage Return								
	Enviar_Comando_Simple(LF); while(canal_salida_xbob->estado_buffer != BUFFER_VACIO); 		// Line Feed
								
	CopiarArray_8_bits(canal_salida_xbob->frame, cadena_nerox, LEN_XBOB4_CADENA_NEROX);			// Copio la cadena a transmitir
	
	canal_salida_xbob->ind = 0;																	// Inicializo el índice
	canal_salida_xbob->len_cadena = LEN_XBOB4_CADENA_NEROX;										// Cargo el largo del comando CSI
	
	canal_salida_xbob->estado_buffer = BUFFER_LLENO;											// Señalizo como buffer lleno	
	Iniciar_Transmision_Paquete_UART0(canal_salida_xbob);	
	while(canal_salida_xbob->estado_buffer != BUFFER_VACIO);									// Espero que se haya transmitido el CSI	
		
	return;
}

/********************************************************************************************************
 * 
 *	@brief entero a ascii
 * 
 *  @returns char*
 * 
 ********************************************************************************************************/

int my_itoa(int numero, char* cadena, uint8_t decimales)
{
	uint8_t i, j;
	uint8_t offset_signo = 0;
	uint8_t cant_cifras = 0;
	uint8_t digito;	
	uint8_t largo_cadena = 0;
	
	char cad_aux[10];
	
	int num_aux;	
	
	if(numero == 0)
	{
		cadena[0] = '0';
		largo_cadena = 1;
		return largo_cadena;
	}
	
	if(numero < 0)										// Si el número es menor que cero
	{
		numero = numero * (-1);							// Lo hago positivo
		cadena[0] = '-';								// pero agrego un cero a la cadena
		largo_cadena++;									// Incremento en uno el largo por el signo
	}
	
	num_aux = numero;									// Copio el número a convertir a una variable auxiliar
		
	while(num_aux > 0)									// Mientras siga teniendo cifras
	{
		cant_cifras++;									// cuento la cifra
		num_aux /= 10;									// divido el número por diez
	}
	
	largo_cadena += cant_cifras;
		
	for(i = 0; i < cant_cifras; i++)
	{
		digito = numero % 10;
		cadena [cant_cifras - (i + 1) + offset_signo] = (char) ('0' + digito);
				
		numero /= 10;
	}
	
	
	if(decimales > 0)	largo_cadena++;					// si tiene decimales agrego la coma
	
	j = 0;
	
	if(decimales < cant_cifras)
	{
		for(i = 0; i < decimales; i++)
		{
			cadena[cant_cifras - i] = cadena[cant_cifras - 1 - i]; 		
		}
		cadena[cant_cifras - decimales] = ',';
	}
	else if(decimales >= cant_cifras)
	{
		for(i = 0; i < (decimales + 2); i++)
		{
			if(i < ((decimales + 2) - cant_cifras))
				cad_aux[i] = '0';
			else
			{
				cad_aux[i] = cadena[j + offset_signo];
				j++;
			}							 			
		}		
		cad_aux[1] = ',';
		
		for(i = 0; i < (decimales + 2); i++)
			cadena[i] = cad_aux[i];
		
		largo_cadena = (decimales + 2);
				
	}
		
	return largo_cadena;
}
 
/******************************************************************************************************** 	
 * 											Final del Archivo											*
 ********************************************************************************************************/
