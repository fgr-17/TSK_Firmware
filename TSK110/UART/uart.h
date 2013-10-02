/*******************************************************************************************
 *
 * 	@file uart.h
 * 
 * 	@brief header del archivo "uart.c"
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@company NEROX 11/2011
 * 
 *******************************************************************************************/


#ifndef UART_H_
#define UART_H_

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/

#include "StrBus/strbus.h"
#include "Timer_A2/Timer_A2.h"


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
