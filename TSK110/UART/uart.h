/*******************************************************************************************
 *
 * 	Archivo : uart.h
 * 
 * 	Descripción: header del archivo "uart.c"
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 11/2011
 * 
 *******************************************************************************************/


#ifndef UART_H_
#define UART_H_

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/

#include <strbus.h>
#include <Timer_A2.h>


typedef enum {SI, NO} t_bool;

/********************************************************************************************************
 * 									Prototipos de funciones externas									*
 ********************************************************************************************************/

extern int Inicializar_UART0(void);
extern int Inicializar_UART1(void);

extern int Iniciar_Transmision_Paquete_UART0(T_Modbus* ch_out);
extern int Iniciar_Transmision_Paquete_UART1(T_Modbus* ch_out);

extern int Frame_Timeout(T_Modbus* canal_rx, T_Delay timer_timeout);
extern int Frame_Timeout_UART1(void);

#endif /*UART_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
