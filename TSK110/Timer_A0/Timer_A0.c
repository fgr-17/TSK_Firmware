/********************************************************************************************************************
 *	@file Timer_A0.h																								*
 * 																													*
 * 	@brief Configuración del Timer A1																				*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/ 

/********************************************************************************************************************
 * 												Inclusión de Archivos												*
 ********************************************************************************************************************/


#include <msp430f5529.h>
#include <stdint.h>

#include <hardware_constantes.h>

#include <dma.h>
#include <pwm.h> 
#include <dsp.h>

#include <Timer_A0.h>
 
/********************************************************************************************************************
 * 																													*
 * 	@brief Configuración del Timer_A0 para disparar el ADC al muestrear temperatura									*	
 * 																													*
 * 	@params	none																									*
 * 																													*
 * 	@returns 0 funcionamiento correcto																				*
 * 																													*
 ********************************************************************************************************************/
 
 void Timer_A0_Fs_Temperatura(void)
 {
 	
 	TA0CCR0 = (FRECUENCIA_SISTEMA / FRECUENCIA_MUESTREO_INIT) - 1;		// CCR0 es el divisor de frecuencia del SMCLK
	TA0CCR1 = TA0CCR0 - 100;											// Cuando TA0_CCR1 = 1, el ADC empieza a convertir
	TA0CCTL1 = OUTMOD_3;                       							// CCR1 set/reset mode
	TA0CTL = TASSEL_2 + MC_1 + TACLR;          
 
 }

/********************************************************************************************************************
 * 																													*
 * 	@brief Configuración del Timer_A0 para disparar el ADC al muestrear conductividad								*	
 * 																													*
 * 	@params	none																									*
 * 																													*
 * 	@returns 0 funcionamiento correcto																				*
 * 																													*
 ********************************************************************************************************************/ 

int Timer_A0_Fs_Conductividad(void)
{

	TA0CCR0 = (FRECUENCIA_SISTEMA / FRECUENCIA_CLOCK_ADC) - 1;		// CCR0 es el divisor de frecuencia del SMCLK
	TA0CCR1 = TA0CCR0 - 50;											// Cuando TA0_CCR1 = 1, el ADC empieza a convertir
	TA0CCTL1 = OUTMOD_3;                       						// CCR1 set/reset mode
	TA0CTL = TASSEL_2 + MC_1 + TACLR;              					// SMCLK, Up-Mode
	
	return 0;
} 

/********************************************************************************************************************
 * 																													*
 * 	@brief ISR del TimerA0. Se ejecuta en cada ciclo del PWM														*	
 * 																													*
 * 	@params	none																									*
 * 																													*
 * 	@returns 0 funcionamiento correcto																				*
 * 																													*
 ********************************************************************************************************************/

 #pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
	(*pwm1.registro_ccr_duty) = pwm1.sgn_a_modular.sgn_muestras[pwm1.sgn_a_modular.sgn_indice++];

	if(pwm1.sgn_a_modular.sgn_indice >= pwm1.sgn_a_modular.sgn_largo)
		pwm1.sgn_a_modular.sgn_indice = 0;
}
