/********************************************************************************************************************
 *	@file pwm.c																										*
 * 																													*
 * 	@brief manejo del modo PWM del timer																			*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 											INCLUSION DE ARCHIVOS													*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>
		
#include <hardware_constantes.h>
#include <hardware_macros.h>

#include <dma.h>
#include <pwm.h>

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

int PWM_Init(PWM_Var* PWM_Sel, char timer, uint8_t salida);											// Inicialización de la salida PWM
int PWM_Setear_Fsw (PWM_Var* PWM_Sel, uint16_t frec_sw);											// Establece una frecuencia de sw 		
int PWM_Setear_Duty(PWM_Var* PWM_Sel, uint16_t duty_refresco);										// Establece un ciclo de trabajo determinado

void PWM_Activar_Salida(PWM_Var* PWM_Sel);															// Activa la salida PWM

int PWM_Desactivar_Salida(PWM_Var* PWM_Sel); 														// Desactiva la salida PWM
int PWM_Reset_Campos(PWM_Var* PWM_Sel);																// Reinicializa los contadores asociados al PWM
int PWM_Init_TA1_1(PWM_Var* PWM_Sel);																// Inicializa la salida PWM en TA1_1, P2.0
int PWM_Convertir_DAC(PWM_Var* PWM_Sel, Sgn_Res muestra_a_escribir);								// Setea el duty a partir de la muestra a convertir
int PWM_Setear_Resolucion_DAC(PWM_Var* PWM_Sel, PWM_Resolucion PWM_Res);							// Setea la fsw a partir de la resolucion
int PWM_Cargar_Sgn (PWM_Var* PWM_Sel, Sgn_Res* sgn_a_cargar, int16_t sgn_largo, uint16_t fs);		// Carga una señal a modular en memoria: 					

/********************************************************************************************************************
 * 											VARIABLES GLOBALES EXTERNAS												*
 ********************************************************************************************************************/

PWM_Var pwm1;

/********************************************************************************************************************
 * 											IMPLEMENTACION DE FUNCIONES												*
 ********************************************************************************************************************/

/********************************************************************************
 * 
 * 	@brief Inicializa la salida de PWM sobre el TimerA
 * 
 *	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/ 

int PWM_Init(PWM_Var* PWM_Sel, char timer, uint8_t salida)
{

	unsigned short bit_clock_source;
	unsigned short bit_clock_clear;

	// WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	
	// A partir del timer y la salida seleccionada, defino los registros que se van a usar:	
	switch(timer)
	{
		case '0':
		// Timer A0:
			
			bit_clock_source = TASSEL_2;				// Selecciono SMCLK
			bit_clock_clear = TACLR;					// Bit de clear del registro del Timer A
			
			PWM_Sel->registro_ccr_sw = &TA0CCR0;		// Registro para la frecuencia de switching
			PWM_Sel->registro_timer_ctl = &TA0CTL;	// Registro para configurar el timer
			PWM_Sel->registro_hab_int = &TA0CCTL0;	// Regostro para habilitación de interrupción
			
			if(salida == 1)
			{	
				PWM_Sel->registro_ccr_duty = &TA0CCR1;
				PWM_Sel->registro_ccr_ctl = &TA0CCTL1;
				PWM_Sel->port_salida = 1;
				PWM_Sel->bit_salida = 2;			
			}
			else if(salida == 2)
			{	
				PWM_Sel->registro_ccr_duty = &TA0CCR2;
				PWM_Sel->registro_ccr_ctl = &TA0CCTL2;
				PWM_Sel->port_salida = 1;
				PWM_Sel->bit_salida = 3;						
			}
			else if(salida == 3)
			{	
				PWM_Sel->registro_ccr_duty = &TA0CCR3;
				PWM_Sel->registro_ccr_ctl = &TA0CCTL3;
				PWM_Sel->port_salida = 1;
				PWM_Sel->bit_salida = 4;						
			}
			else if(salida == 4)
			{	
				PWM_Sel->registro_ccr_duty = &TA0CCR4;
				PWM_Sel->registro_ccr_ctl = &TA0CCTL4;
				PWM_Sel->port_salida = 1;
				PWM_Sel->bit_salida = 5;						
			}
			else
			{
				return 1;			// El timer A0 tiene solo 4 salidas PWM (se utiliza la 0 para generar el pulso)
			}	
			break;
			
		case '1':
		// Timer A1:
		
			bit_clock_source = TASSEL_2;				// Selecciono SMCLK
			bit_clock_clear = TACLR;					// Bit de clear del registro del Timer A
			
			PWM_Sel->registro_ccr_sw = &TA1CCR0;		// Registro para la frecuencia de switching
			PWM_Sel->registro_timer_ctl = &TA1CTL;	// Registro para configurar el timer
			PWM_Sel->registro_hab_int = &TA1CCTL0;	// Regostro para habilitación de interrupción
						
			if(salida == 1)
			{	
				PWM_Sel->registro_ccr_duty = &TA1CCR1;
				PWM_Sel->registro_ccr_ctl = &TA1CCTL1;
				PWM_Sel->port_salida = 2;
				PWM_Sel->bit_salida = 0;			
			}
			else if(salida == 2)
			{	
				PWM_Sel->registro_ccr_duty = &TA1CCR2;
				PWM_Sel->registro_ccr_ctl = &TA1CCTL2;
				PWM_Sel->port_salida = 2;
				PWM_Sel->bit_salida = 1;						
			}
			else
			{
				return 1;
			}
			
			break;
		
		case '2':
		// Timer A2:
			
			bit_clock_source = TASSEL_2;				// Selecciono SMCLK
			bit_clock_clear = TACLR;					// Bit de clear del registro del Timer A
			
			PWM_Sel->registro_ccr_sw = &TA2CCR0;		// Registro para la frecuencia de switching
			PWM_Sel->registro_timer_ctl = &TA2CTL;	// Registro para configurar el timer
			PWM_Sel->registro_hab_int = &TA2CCTL0;	// Regostro para habilitación de interrupción
						
			if(salida == 1)
			{	
				PWM_Sel->registro_ccr_duty = &TA2CCR1;
				PWM_Sel->registro_ccr_ctl = &TA2CCTL1;
				PWM_Sel->port_salida = 2;
				PWM_Sel->bit_salida = 4;			
			}
			else if(salida == 2)
			{	
				PWM_Sel->registro_ccr_duty = &TA2CCR2;
				PWM_Sel->registro_ccr_ctl = &TA2CCTL2;
				PWM_Sel->port_salida = 2;
				PWM_Sel->bit_salida = 5;						
			}
			else
			{
				return 1;
			}
			
			break;	
		
		case 'B':
		// Timer B:
			
			bit_clock_source = TBSSEL_2;				// Selecciono SMCLK
			bit_clock_clear = TBCLR;					// Bit de clear del registro del Timer A
			
			PWM_Sel->registro_ccr_sw = &TBCCR0;			// Registro para setear el contador de la fsw
			PWM_Sel->registro_timer_ctl = &TBCTL;		// Registro para configurar el timer
			PWM_Sel->registro_hab_int = &TBCCTL0;		// Regostro para habilitación de interrupción
			
			if(salida == 1)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR1;
				PWM_Sel->registro_ccr_ctl = &TBCCTL1;
				PWM_Sel->port_salida = 5;
				PWM_Sel->bit_salida = 7;
			}
			else if(salida == 2)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR2;
				PWM_Sel->registro_ccr_ctl = &TBCCTL2;
				PWM_Sel->port_salida = 7;
				PWM_Sel->bit_salida = 4;
			}
			else if(salida == 3)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR3;
				PWM_Sel->registro_ccr_ctl = &TBCCTL3;
				PWM_Sel->port_salida = 7;
				PWM_Sel->bit_salida = 5;
			}
			else if(salida == 4)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR4;
				PWM_Sel->registro_ccr_ctl = &TBCCTL4;
				PWM_Sel->port_salida = 7;
				PWM_Sel->bit_salida = 6;
			}
			else if(salida == 5)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR5;
				PWM_Sel->registro_ccr_ctl = &TBCCTL5;
				PWM_Sel->port_salida = 3;
				PWM_Sel->bit_salida = 5;
			}
			else if(salida == 6)
			{
				PWM_Sel->registro_ccr_duty = &TBCCR6;
				PWM_Sel->registro_ccr_ctl = &TBCCTL6;
				PWM_Sel->port_salida = 3;
				PWM_Sel->bit_salida = 6;
			}
			else
				return 1;
					
			break;
			
		default:
		
			return 1;			// Error en la elección del timer de salida 
	}
	
	// Detengo el timer para realizar la configuración	
	(*PWM_Sel->registro_timer_ctl) &= MC_0;
		
	// Seteo la frecuencia del cristal y el conteo máximo por default:
	PWM_Sel->frec_ref = FRECUENCIA_XTAL;
	PWM_Sel->duty_max = DUTY_MAX_INICIAL; 		
	
	// Seteo la frecuencia de switching y establezco el duty inicial por default
	if(PWM_Setear_Fsw(PWM_Sel, FSW_INICIAL)) 	return 1;
	if(PWM_Setear_Duty(PWM_Sel, DUTY_INICIAL)) 	return 1;
			
	// Seteo los registros para inicializar los pines y el timer
	(*PWM_Sel->registro_timer_ctl) = bit_clock_source + MC_1 + bit_clock_clear;	// Registro de Control: cuenta arriba
	if(Dir_Pin(PWM_Sel->port_salida, PWM_Sel->bit_salida, DIR_SALIDA, FUNCION_PERIF)) return 1;
	
	// Deshabilito la interrupción del timer cada vez que se cumple un período Tsw:
	(*PWM_Sel->registro_hab_int) &= ~CCIE; 		
	
	return 0;	// Terminó OK!
}

/********************************************************************************
 * 
 * 	@brief Setea el ciclo de trabajo de la señal PWM
 * 
 *	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/ 

int PWM_Setear_Duty(PWM_Var* PWM_Sel, uint16_t duty_refresco)
{
	
	// HACER : Apagar timer
	
	if(duty_refresco >= PWM_Sel->duty_max)
		return 1;
	
	PWM_Sel->duty = duty_refresco;
	PWM_Sel->cuenta_t_on = (PWM_Sel->cuenta_periodo * PWM_Sel->duty)/ PWM_Sel->duty_max;		
	(*PWM_Sel->registro_ccr_duty) = PWM_Sel->cuenta_t_on;
	
	return 0;		// Terminó OK!
	
	// HACER : Encender timer
}

/********************************************************************************
 * 
 *	@brief Setea la frecuencia fundamental de la señal PWM
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Setear_Fsw (PWM_Var* PWM_Sel, uint16_t frec_sw)
{

	// HACER : Apagar timer
	
	if(frec_sw >= (PWM_Sel->frec_ref))
		return 1;
		
	PWM_Sel->frec_sw = frec_sw;
	PWM_Sel->cuenta_periodo = (PWM_Sel->frec_ref / PWM_Sel->frec_sw) - 1;
	(*PWM_Sel->registro_ccr_sw) = PWM_Sel->cuenta_periodo;
	(*PWM_Sel->registro_ccr_ctl) = OUTMOD_7;
	
	return 0;		// Terminó OK!
	
	// HACER : Encender timer
}

/********************************************************************************
 * 
 *	@brief Carga una señal a modular como salida PWM
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Cargar_Sgn (PWM_Var* p_PWM_Sel, Sgn_Res* sgn_a_cargar, int16_t sgn_largo, uint16_t fs)
{
	
	uint16_t divisor;
		
	// Cargo parámetros del divisor de frecuencia del timer
	divisor = p_PWM_Sel->frec_sw / fs;	// divisor de la fsw
	
	if(divisor <= 0)						// Si la fs > fsw
		return 1;							// Devuelvo error.

	p_PWM_Sel->cuenta_fs_div = divisor - 1;	// Seteo el divisor de fs
	p_PWM_Sel->cuenta_fs_i = 0;
	
	
	// Cargo parámetros de la señal
	p_PWM_Sel->sgn_a_modular.sgn_muestras 	= sgn_a_cargar;	// Apunto el lugar en memoria donde están las muestras
	p_PWM_Sel->sgn_a_modular.sgn_largo		= sgn_largo;	
	p_PWM_Sel->sgn_a_modular.fs				= fs;	
	p_PWM_Sel->sgn_a_modular.sgn_indice 	= 0;			// Coloco el índice en cero
		
	
		
	return 0;	// Finalizó OK!
	
}

/********************************************************************************
 * 
 * 	@brief Carga una señal a modular como salida PWM
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Setear_Resolucion_DAC(PWM_Var* PWM_Sel, PWM_Resolucion PWM_Res)
{	
	uint16_t fsw;
	
	fsw = PWM_Sel->frec_ref / (uint16_t) PWM_Res;				// Frecuencia de switching en kHz
	
	if(fsw <= 0)												// Si la cuenta dio mal
		return 1;												// Devuelvo error.
		
	PWM_Sel->frec_sw = fsw;										// Si está todo OK, asocio la fsw			
		
	
	PWM_Sel->cuenta_periodo = (uint16_t) PWM_Res;				// Elijo la cuenta a partir de la resolucion del DAC
	(*PWM_Sel->registro_ccr_sw) = PWM_Sel->cuenta_periodo;		// Escribo el registro correspondiente
	(*PWM_Sel->registro_ccr_ctl) = OUTMOD_7;					// Seteo el modo hacia arriba
	
	return 0;		// Terminó OK!
}

/********************************************************************************
 * 
 * 	@brief Carga una señal a modular como salida PWM
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Convertir_DAC(PWM_Var* PWM_Sel, Sgn_Res muestra_a_escribir)
{	
	
	if(muestra_a_escribir > PWM_Sel->cuenta_periodo)			// Si quiero escribir una muestra mayor a la resolucion 
		return 1;												// Genero error por overflow
		
	PWM_Sel->cuenta_t_on = (uint16_t) muestra_a_escribir;		// Sino escribo la muestra		
	(*PWM_Sel->registro_ccr_duty) = PWM_Sel->cuenta_t_on;	// Escribo el registro
	
	return 0;		// Terminó OK!
	
}

/********************************************************************************
 * 
 * 	@brief Activa la salida del canal de pwm indicado
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/
 
void PWM_Activar_Salida(PWM_Var* PWM_Sel)
{
	Dir_Pin(PWM_Sel->port_salida, PWM_Sel->bit_salida, DIR_SALIDA, FUNCION_PERIF);
	
	PWM_Sel->cuenta_fs_i = 0;
	PWM_Sel->sgn_a_modular.sgn_indice = 0;
		
	(*PWM_Sel->registro_hab_int) |= CCIE;					// Habilito la interrupción del timer:	
	
	return;
}

/********************************************************************************
 * 
 * 	@brief Desactiva la salida del canal de pwm indicado
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Desactivar_Salida(PWM_Var* PWM_Sel)
{
	// Deshabilito la interrupción del timer:
	(*PWM_Sel->registro_hab_int) &= ~CCIE;

	Dir_Pin(PWM_Sel->port_salida, PWM_Sel->bit_salida, DIR_SALIDA, FUNCION_IO);

	return 0;
}

/********************************************************************************
 * 
 * 	@brief Resetea los campos de la estructura PWM_Var
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Reset_Campos(PWM_Var* PWM_Sel)
{
	
	PWM_Sel->cuenta_fs_i = 0;
	PWM_Sel->sgn_a_modular.sgn_indice = 0;
	
	return 0;	
	
}

/********************************************************************************
 * 
 * 	@brief Inicializa la salida PWM en el TA1_1
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************/

int PWM_Init_TA1_1(PWM_Var* PWM_Sel)
{
	PWM_Sel->registro_ccr_sw = &TA1CCR0;		// Registro para setear el contador de la fsw
	PWM_Sel->registro_timer_ctl = &TA1CTL;		// Registro para configurar el timer
	PWM_Sel->registro_hab_int = &TA1CCTL0;		// Registro para habilitación de interrupción
	PWM_Sel->registro_ccr_duty = &TA1CCR1;		// Registro para setear el duty 			
	PWM_Sel->registro_ccr_ctl = &TA1CCTL1;		// Registro de control del Contador CCR1
	
	PWM_Sel->port_salida = 2;
	PWM_Sel->bit_salida = 0;

	// Seteo la frecuencia del cristal y el conteo máximo por default:
	PWM_Sel->frec_ref = FRECUENCIA_SISTEMA;
	PWM_Sel->duty_max = DUTY_MAX_INICIAL; 		
	
	// Seteo la frecuencia de switching y establezco el duty inicial por default
	if(PWM_Setear_Fsw(PWM_Sel, FSW_INICIAL)) 	return 1;
	if(PWM_Setear_Duty(PWM_Sel, DUTY_INICIAL)) 	return 1;
			
	// Seteo los registros para inicializar los pines y el timer
	if(Dir_Pin(PWM_Sel->port_salida, PWM_Sel->bit_salida, DIR_SALIDA, FUNCION_PERIF)) return 1;
	
	TA1CCTL1 = OUTMOD_7;                      // CCR1 reset/set	
	TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, up mode, clear TAR
	
	// Deshabilito la interrupción del timer cada vez que se cumple un período Tsw:
	(*PWM_Sel->registro_hab_int) &= ~CCIE;
	  	
	return 0;	
}

/************************************************************************************
 * 								Fin del Archivo										*
 ************************************************************************************/
