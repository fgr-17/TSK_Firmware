/********************************************************************************************************
 *																										*
 * 	Archivo : Timer_A2.h																				*
 *																										* 
 * 	Descripción: configuración del hardware del Timer_A2												*	
 *																										*
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)													*
 *																										*
 * 	NEROX 11/2011																						*
 *																										*
 ********************************************************************************************************/

/********************************************************************************************************
 *							 				Inclusión de archivos										*
 ********************************************************************************************************/ 

#include <msp430f5529.h>
#include <stdint.h>

#include "Hardware/hardware_constantes.h"
#include "Timer_A2/Timer_A2.h"

/********************************************************************************************************
 *							 				Prototipos de Funciones										*
 ********************************************************************************************************/
 
int Inicializar_Timer_A2_Delay(void);
int Timer_A2_Delay(uint16_t milisegundos, T_Modo_Delay Modo_Delay);

/********************************************************************************************************
 *							 				Variables Globales											*
 ********************************************************************************************************/

T_Delay delay_timer_A2;

/********************************************************************************************************
 *							 			Implementación de Funciones										*
 ********************************************************************************************************/

int Inicializar_Timer_A2_Delay(void)
{
	delay_timer_A2.cuenta_delay = 0;											// Inicializo el contador
	delay_timer_A2.cuenta_max = 0;												// Inicializo el período máximo
	delay_timer_A2.estado_delay_Timer = FIN_DELAY;								// Comienzo en estado "FIN_DELAY"
	TA2CCTL0 &= ~CCIE;															// Deshabilito la interrupción del timer
	return 0;
}

int Timer_A2_Delay(uint16_t milisegundos, T_Modo_Delay Modo_Delay)
{
	
	// Ajuste de la estructura del timer	
	delay_timer_A2.cuenta_delay = 0;  											// Inicializo la cuenta del contador
  	delay_timer_A2.cuenta_max   = milisegundos;									// Seteo la cuenta máxima
  	delay_timer_A2.estado_delay_Timer = CONTANDO;								// Seteo la estructura del timer en "ESPERANDO".
  
	// Seteo de los registros del timer A1    	
	TA2CCTL0 = CCIE;                          									// CCR0 interrupt enabled
	TA2CCR0 = (FRECUENCIA_SISTEMA - 1);											// Cuenta del contador para un milisegundo
	TA2CTL = TASSEL_2 + MC_2 + TACLR;         									// SMCLK, contmode, clear TAR
	
	
	switch(Modo_Delay)
	{
	case BLOQUEANTE:
		while(delay_timer_A2.estado_delay_Timer == CONTANDO);
		return 0;
	
	case NO_BLOQUEANTE:
		return 0;		
		
	default:
		return 1;
		
	}
}

// Timer2 interrupt service routine
#pragma vector=TIMER2_A0_VECTOR
__interrupt void TIMER2_A0_ISR(void)
{
		
	delay_timer_A2.cuenta_delay++;												// Incremento la cuenta del timer
	delay_timer_A2.estado_delay_Timer = CONTANDO;								// Si estoy contando, está en modo "Esperando" 
		
	if(delay_timer_A2.cuenta_delay >= delay_timer_A2.cuenta_max)				// Si llegué a la cuenta máxima
	{
		delay_timer_A2.cuenta_delay = 0;										// Reinicializo la cuenta
		delay_timer_A2.estado_delay_Timer = FIN_DELAY;							// Cambio al estado de "libre"
		TA2CCTL0 &= ~CCIE;														// Deshabilito la interrupción del timer
	}
	
	
	TA2CCR0 += (FRECUENCIA_SISTEMA  - 1);                         				// Add Offset to CCR0

}

/******************************************************************************************************** 	
 *		 									Final del Archivo											*
 ********************************************************************************************************/
