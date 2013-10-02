/********************************************************************************************************
 *																										*
 * 	Archivo : uart.c																					*
 * 																										*
 * 	Descripción: configuración del hardware de comunicación serie.										*
 * 																										*
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)													*
 * 																										*
 * 	NEROX 11/2011																						*
 *																										* 
 ********************************************************************************************************/

/********************************************************************************************************
 * 											Inclusión de archivos										*	
 ********************************************************************************************************/ 

#include <msp430f5529.h>
#include <stdint.h>

#include <hardware_constantes.h>
#include <hardware_macros.h>

#include <modos.h>
#include <strbus.h>
#include <Timer_A2.h>
#include <xbob_4.h>

#include <uart.h>
#include <HAL_RTC.h>

#include <HAL_Board.h>

/********************************************************************************************************
 * 										Prototipos de funciones											*
 ********************************************************************************************************/

int Inicializar_UART0(void);
int Inicializar_UART1(void);

int Iniciar_Transmision_Paquete_UART0(T_Modbus* ch_out);
int Iniciar_Transmision_Paquete_UART1(T_Modbus* ch_out);

int Frame_Timeout(T_Modbus* canal_rx, T_Delay timer_timeout);
int Frame_Timeout_UART1(void);
/********************************************************************************************************
 * 										Implementación de funciones										*
 ********************************************************************************************************/

int Leer_Dato_Buffer(T_Modbus*canal_rx)
{
	uint8_t dato_extraido;




}





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
	uint8_t i;
		
	switch(__even_in_range(UCA0IV,4))
	{
	case 0:		                             								// Vector 0 - no interrupt
		break;
	case 2:                                   								// Vector 2 - RXIFG
		
		rx = UCA0RXBUF;														// Leo el byte recibido
		UCA0IFG = 0;														// Limpio el flag de interrupción pendiente
		
		if(canal_rx_0.estado_buffer == BUFFER_LLENO)						// Pregunto si el buffer está lleno
		{
			return; 														// Si el buffer está lleno, salgo nomas
		}
		else
		{
			canal_rx_0.buffer[canal_rx_0.ind_bf] = rx;						// Guardo el dato recibido
			canal_rx_0.ind_bf++;											// incremento el índice guardado
			canal_rx_0.dato_pendiente = SI;									// Subo el flag de dato pendiente

			if(canal_rx_0.ind_bf >= BUFFER_N)								// Pregunto si el buffer se llenó
			{
				canal_rx_0.estado_buffer = BUFFER_LLENO;					// En ése caso, señalizo buffer lleno
				return;														// Salgo de la interrupción
			}
			else
			{
				canal_rx_0.estado_buffer = BUFFER_CARGANDO;					// Si no está lleno, señalizo buffer cargando
			}
		}

		/*
		canal_rx_0.estado_buffer_anterior = canal_rx_0.estado_buffer;					// Guardo el estado anterior por las dudas que se pierda el frame
		canal_rx_0.estado_buffer = BUFFER_CARGANDO;										// al estar escribiendo, pongo en estado libre
		
		if(canal_rx_0.ind > canal_rx_0.len_cadena) 
			canal_rx_0.ind = 0;															// Si me pase del largo del array, inicializo
						
		canal_rx_0.frame_temp[canal_rx_0.ind] = rx;										// Escribo en el buffer de recepcion

		canal_rx_0.ind++;																// Incremento el índice del buffer
		
		Timer_A2_Delay(1000, NO_BLOQUEANTE);											// Comienzo la cuenta del delay
				
		if(canal_rx_0.ind > canal_rx_0.len_cadena - 1)									// Llegué al final del array?
		{
			canal_rx_0.ind = 0;															// Inicializo el índice
			canal_rx_0.estado_buffer = BUFFER_LLENO;									// Queda en estado Leer
			for(i = 0;i < canal_rx_0.len_cadena; i++)									// Recorro el array temporal
			{
				canal_rx_0.frame[i] = canal_rx_0.frame_temp[i]; 						// descargo todo sobre el buffer definitivo
			} 		
		}
		*/
		break;

	case 4:                             												// Vector 4 - TXIFG
		/*
		if(canal_tx_0.estado_buffer == BUFFER_LLENO)									// En caso de que esté el buffer lleno
		{
			canal_tx_0.ind++;															// Incremento el índice del array
			
			if(canal_tx_0.ind >= canal_tx_0.len_cadena)									// Pregunto si llegué al final de la cadena
			{
				canal_tx_0.ind = 0;														// Inicializo el índice del buffer
				canal_tx_0.estado_buffer = BUFFER_VACIO;								// Señalizo como buffer vacío
			}
			else
			{			
				UCA0TXBUF = canal_tx_0.frame[canal_tx_0.ind];							// Pongo en el registro el byte a transmitir
				UCA0IFG = 0;															// Limpio el flag de interrupción pendiente
			}
		}*/

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
						
		canal_rx_2.frame_temp[canal_rx_2.ind] = rx;										// Escribo en el buffer de recepcion

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
				canal_rx_2.frame[i] = canal_rx_2.frame_temp[i]; 						// descargo todo sobre el buffer definitivo
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
