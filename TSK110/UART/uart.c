/********************************************************************************************************
 *																										*
 * 	@author 	uart.c																					*
 * 																										*
 * 	@brief 		configuración del hardware de comunicación serie.										*
 * 																										*
 * 	@author		Roux, Federico G. (rouxfederico@gmail.com)												*
 * 																										*
 * 	@date		11/2011																					*
 *																										* 
 ********************************************************************************************************/

/********************************************************************************************************
 * 											Inclusión de archivos										*	
 ********************************************************************************************************/ 

#include <msp430f5529.h>
#include <stdint.h>

#include "Hardware/hardware_constantes.h"
#include "Hardware/hardware_macros.h"

#include "Maq_Estados/modos.h"
#include "StrBus/strbus.h"
#include "Timer_A2/Timer_A2.h"
#include "XBOB_4/xbob_4.h"

#include "UART/uart.h"
#include "F5xx_F6xx_Core_Lib/HAL_RTC.h"

#include "MSP-EXP430F5529_HAL/HAL_Board.h"

/********************************************************************************************************
 * 										Prototipos de funciones											*
 ********************************************************************************************************/

int Inicializar_UART0(void);
int Inicializar_UART1(void);

int Iniciar_Transmision_Paquete_UART0(T_Modbus* ch_out);
int Iniciar_Transmision_Paquete_UART1(T_Modbus* ch_out);

int Frame_Timeout(T_Modbus* canal_rx, T_Delay timer_timeout);
int Frame_Timeout_UART1(void);

__inline int Leer_Dato_Buffer_INLINE(T_Modbus*canal, uint8_t*dato_leido);
__inline int Escribir_Dato_Buffer_INLINE (T_Modbus*canal, uint8_t dato_a_escribir);

/********************************************************************************************************
 * 										Implementación de funciones										*
 ********************************************************************************************************/


/********************************************************************************************************
 *	@brief 		Inicializo el periférico de la UART1, pines 4.5 RXD y 4.4 TXD							*
 * 																										*
 * 	@params 	none																					*
 * 																										* 
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/
 
int Inicializar_UART1(void)
{
	P4SEL = BIT4 + BIT5;						// P3.4,5 = USCI_A0 TXD/RXD

	UCA1CTL1 |= UCSWRST;                      	// **Put state machine in reset**
	UCA1CTL1 |= UCSSEL_1;                     	// CLK = ACLK
	UCA1BR0 = 0x03;                           	// 32kHz/9600=3.41 (see User's Guide)
	UCA1BR1 = 0x00;                           	//
	UCA1MCTL = UCBRS_3+UCBRF_0;               	// Modulation UCBRSx=3, UCBRFx=0
	UCA1CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**
		
	UCA1IFG = 0;								// Limpio los bits de interrupción pendiente	
	UCA1IE |= (UCRXIE + UCTXIE);				// Enable USCI_A0 RX interrupt
	
	return 0;
}

/********************************************************************************************************
 *	@brief 		Inicializo el periférico de la UART0, pines 3.4 RXD y 3.3 TXD							*
 * 																										*	
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/
 
int Inicializar_UART0(void)
{
	P3SEL = BIT3 + BIT4;						// P3.4,5 = USCI_A0 TXD/RXD

	UCA0CTL1 |= UCSWRST;                      	// **Put state machine in reset**
	UCA0CTL1 |= UCSSEL_1;                     	// CLK = ACLK
	UCA0BR0 = 0x03;                           	// 32kHz/9600=3.41 (see User's Guide)
	UCA0BR1 = 0x00;                           	//
	UCA0MCTL = UCBRS_3+UCBRF_0;               	// Modulation UCBRSx=3, UCBRFx=0
	UCA0CTL1 &= ~UCSWRST;                     	// **Initialize USCI state machine**	
	UCA0IFG = 0;								// Limpio los bits de interrupción pendiente 
	UCA0IE |= (UCRXIE + UCTXIE);				// Enable USCI_A0 RX interrupt
	
	return 0;
}

/********************************************************************************************************
 *	@brief 		Timeout para la recepción de datos de paquetes											*
 * 																										*
 * 	@params 	none																					*
 * 																										* 
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/

int Frame_Timeout(T_Modbus* canal_rx, T_Delay timer_timeout)
{
	if(canal_rx->estado_buffer == BUFFER_CARGANDO)
	{
		if(timer_timeout.estado_delay_Timer == FIN_DELAY)
		{
			canal_rx->ind = 0;
			canal_rx->estado_buffer = canal_rx->estado_buffer_anterior;
			// canal_rx->estado_buffer = BUFFER_VACIO;
		}			
	}
	
	return 0;
}

/********************************************************************************************************
 *	@brief 		Timeout para la recepción de datos de paquetes											*
 * 																										*
 * 	@params 	none																					*
 * 																										* 
 * 	@returns 	0 -> Reset																				*
 * 	@returns 	1 -> Nada																				*
 * 																										*
 ********************************************************************************************************/
 
int Frame_Timeout_UART1(void)
{
	if(timeout_UART1.estado_delay_Timer == LIMPIAR_BUFFER)
	{
		timeout_UART1.estado_delay_Timer = DETENIDO;
		canal_rx_2.ind = 0;
		canal_rx_2.estado_buffer = BUFFER_VACIO;
		return 0;
	}
	
	return 1;	
}

/********************************************************************************************************
 *																										*
 *  @brief		Comienzo la transmisión de un paquete por UART0, el resto es por ISR					*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/

int Iniciar_Transmision_Paquete_UART0(T_Modbus* ch_out)
{
	ch_out->ind = 0;
	ch_out->estado_buffer = BUFFER_LLENO;
	UCA0TXBUF = ch_out->frame[0];														// Envío el primer caracter para inicializar la transmisión del buffer	
	return 0;
}

/********************************************************************************************************
 *																										*
 *  @brief		Comienzo la transmisión de un paquete por UART1, el resto es por ISR					*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/

int Iniciar_Transmision_Paquete_UART1(T_Modbus* ch_out)
{
	ch_out->ind = 0;
	ch_out->estado_buffer = BUFFER_LLENO;
	UCA1TXBUF = ch_out->frame[0];														// Envío el primer caracter para inicializar la transmisión del buffer
	return 0;
}

/************************************************************************************************************
 *	@brief 		Escribo un dato en el buffer (versión inline)
 *
 * 	@params 	canal : Canal a escribir
 * 	@params		dato_a_escribir :
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline int Escribir_Dato_Buffer_INLINE (T_Modbus*canal, uint8_t dato_a_escribir)
{
	if(canal->estado_buffer == BUFFER_LLENO)					// Pregunto si el buffer ya está lleno
		return ESCRIBIR_DATO_BUFFER_LLENO;						// Retorno error de buffer lleno

	canal->buffer[canal->i_fr] = dato_a_escribir;				// Escribo el dato en el frente
	canal->i_fr++;												// Incremento el puntero del frente
	canal->dato_pendiente = SI;									// Levanto flag de dato pendiente
	canal->estado_buffer = BUFFER_CARGANDO;						// Señalizo buffer cargando

	if(canal->i_fr >= BUFFER_N)									// Pregunto si el puntero del frente llegó al final
		canal->i_fr = 0;										// Lo inicializo al comienzo

	if(canal->i_fo == canal->i_fr)								// Si los índices son iguales
		canal->estado_buffer = BUFFER_LLENO;					// es porque el buffer está lleno

	return ESCRIBIR_DATO_OK;

}

/************************************************************************************************************
 *	@brief 		Atiendo los datos recibidos por uno de los canales de transmisión(versión inline)
 *
 * 	@params 	canal : Canal a leer
 * 	@params		dato_leido : puntero al dato a leer
 *
 * 	@returns 	none
 *
 ************************************************************************************************************/

__inline int Leer_Dato_Buffer_INLINE(T_Modbus*canal, uint8_t*dato_leido)
{

	if(canal->estado_buffer == BUFFER_VACIO)					// Si el buffer está vacío
		return LEER_DATO_BUFFER_VACIO;							// Señalizo y salgo

	canal->i_fo++;												// Incremento el índice del fondo
	if(canal->i_fo >= BUFFER_N)									// Pregunto si llegué al final
		canal->i_fo = 0;										// Lo vuelvo a poner al principio

	*dato_leido = canal->buffer[canal->i_fo];					// Guardo el dato leido

	if(((canal->i_fo + 1)%BUFFER_N) >= canal->i_fr)				// Condición para buffer vacío
		canal->estado_buffer = BUFFER_VACIO;					// Caso verdadero: señalizo

	return LEER_DATO_BUFFER_OK;									// Retorno éxito en la lectura
}

/********************************************************************************************************
 *																										*
 *  @brief		ISR de la UART0																			*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	uint8_t rx;
			
	switch(__even_in_range(UCA0IV,4))
	{
	case 0:		                             								// Vector 0 - no interrupt
		break;

	case 2:                                   								// Vector 2 - RXIFG
		
		rx = UCA0RXBUF;														// Leo el byte recibido
		UCA0IFG = 0;														// Limpio el flag de interrupción pendiente

		if(!Escribir_Dato_Buffer_INLINE(&canal_rx_0, rx))					// Escribo dato en forma inline
			return;															// Si hubo error al escribir, salgo
		
		break;

	case 4:                             												// Vector 4 - TXIFG
		canal_tx_0.dato_pendiente = NO;												// Señalo que ya mande el dato
		UCA0IFG = 0;																// Limpio el flag de interrupción pendiente
		break;
		
	default: 
		break;  
	}	
}

/********************************************************************************************************
 *																										*
 *  @brief		ISR de la UART1																			*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/


#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
{
	uint8_t rx;
	uint8_t i;
		
	switch(__even_in_range(UCA1IV,4))
	{
	case 0:		                             											// Vector 0 - no interrupt
		break;
	case 2:                                   											// Vector 2 - RXIFG
		
		rx = UCA1RXBUF;																	// leo el byte recibido
		UCA1IFG = 0;																	// Limpio el flag de interrupción pendiente
		
		canal_rx_2.estado_buffer_anterior = canal_rx_2.estado_buffer;					// Guardo el estado anterior por las dudas que se pierda el frame
		canal_rx_2.estado_buffer = BUFFER_CARGANDO;										// al estar escribiendo, pongo en estado libre
		
		if(canal_rx_2.ind > canal_rx_2.len_cadena) 
			canal_rx_2.ind = 0;															// Si me pase del largo del array, inicializo
						
		//canal_rx_2.frame_temp[canal_rx_2.ind] = rx;										// Escribo en el buffer de recepcion

		canal_rx_2.ind++;																// Incremento el índice del buffer
		
		// Timer_A2_Delay(1000, NO_BLOQUEANTE);											// Comienzo la cuenta del delay
		timeout_UART1.cuenta_delay = 0;
		timeout_UART1.estado_delay_Timer = CUENTA;
		RTCCTL0 = RTCRDYIE + RTCTEVIE;	         										// Habilito la ISR del RTC
				
		if(canal_rx_2.ind > canal_rx_2.len_cadena - 1)									// Llegué al final del array?
		{
			canal_rx_2.ind = 0;															// Inicializo el índice
			canal_rx_2.estado_buffer = BUFFER_LLENO;									// Queda en estado Leer
			for(i = 0;i < canal_rx_2.len_cadena; i++)									// Recorro el array temporal
			{
				//canal_rx_2.frame[i] = canal_rx_2.frame_temp[i]; 						// descargo todo sobre el buffer definitivo
			} 			
		}
		break;

	case 4:                             												// Vector 4 - TXIFG
		
		if(canal_tx_2.estado_buffer == BUFFER_LLENO)									// En caso de que esté el buffer lleno
		{
			canal_tx_2.ind++;															// Incremento el índice del array
			
			if(canal_tx_2.ind >= canal_tx_2.len_cadena)									// Pregunto si llegué al final de la cadena
			{
				canal_tx_2.ind = 0;														// Inicializo el índice del buffer
				canal_tx_2.estado_buffer = BUFFER_VACIO;								// Señalizo como buffer vacío
			}
			else
			{						
				UCA1TXBUF = canal_tx_2.frame[canal_tx_2.ind];							// Pongo en el registro el byte a transmitir
			}						
		}		
		break;
		
	default: 
		break;  
	}	
}

/********************************************************************************************************
 * 											Final del Archivo											*
 ********************************************************************************************************/
