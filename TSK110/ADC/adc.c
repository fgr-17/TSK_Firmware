/********************************************************************************************************************
 *	@file ADC.c																										*
 * 																													*
 * 	@brief Configuracion del ADC	 																				*
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
#include <adc.h>

#include <dma.h>
#include <pwm.h>
#include <dsp.h>

#include <Conductividad.h>

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

int Inicializar_Timer_ADC_Fs(uint16_t fs);						// Inicializa el timer de disparo del ADC
int Finalizar_Timers(void);										// Resetea los registros del timer
int Finalizar_Timer_ADC(void);

int Inicializar_ADC_Conductividad(void);						// Inicializa la configuración del ADC para la medición de conductividad
int Inicializar_ADC_Sensores(void);								// Inicializo la configuración del ADC para los sensores.

int Iniciar_Medicion_Sensores(void);							// Inicio la medición con los sensores.
int Iniciar_Medicion_Conductividad(void);						// Inicio la medición de conductividad

int Promediar_Sgn_Sensores(void);								// Promedio la medicion de todos los sensores.
int Apagar_ADC(void);											// Limpio los registros de control del ADC.

/********************************************************************************************************************
 * 											DEFINICIÓN DE VARIABLES													*
 ********************************************************************************************************************/

uint16_t medicion_adc_0[N_BUFFER_SENSORES];						// Buffer asociado a ADC12MEM0

volatile uint32_t indice = 0;									// Indice de los buffers "medicion_adc_x". Inicializado en cero
volatile uint8_t  buffers_completos = BUFFER_ADC_DISPONIBLE;	// Variable asociada a los buffers de medición de los sensores. 

volatile int32_t conductividadPromedioObtenida	= 0;
int16_t PT100PromedioObtenido = 0;

/********************************************************************************************************************
 * 										IMPLEMENTACION DE FUNCIONES													*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 																													* 
 *	@brief Deshabilito la conversión y la interrupción del ADC														*
 * 																													*
 * 	@params 	none																								*
 * 																													* 
 * 	@returns 	none																								*
 * 																													*
 ********************************************************************************************************************/

void Deshabilitar_ADC(void)
{
	ADC12CTL0 &= ~ADC12ENC;                            	// Disable conversions
	ADC12IE = 0x0000;									// Enable ADC12IFG.0~7
	// ADC12CTL0 &= ~ADC12ON;                        	// ADC12 off
}

/********************************************************************************************************************
 * 																													*
 *	@brief 		Habilito la conversión del ADC																		*
 * 																													*
 * 	@params		none																								*
 * 																													*
 * 	@returns	none																								*
 * 																													*
 ********************************************************************************************************************/

void Habilitar_ADC(void)
{
	ADC12CTL0 |= ADC12ON;                              // ADC12 on
	ADC12CTL0 |= ADC12ENC;                             // Enable conversions
}

/********************************************************************************************************************
 * 
 * 	@brief Limpia los registros de control del ADC
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Apagar_ADC(void)
{
	ADC12CTL0 &= ~ADC12ENC; 
	ADC12CTL0 = 0x0000;
	ADC12CTL1 = 0x0000;
	
	return 0;
}

/********************************************************************************************************************
 * 
 *	@brief 	Inicializa el timer que disparará el ADC(establece la
 * 	@brien	frecuencia de muestreo)
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Inicializar_Timer_ADC_Fs(uint16_t fs)
{
	TA0CCR0 = (FRECUENCIA_SISTEMA / fs) - 1;					// CCR0 es el divisor de frecuencia del SMCLK
	TA0CCR1 = TA0CCR0 - 50;										// Cuando TA0_CCR1 = 1, el ADC empieza a convertir
	TA0CCTL1 = OUTMOD_3;                       					// CCR1 set/reset mode
	TA0CTL = TASSEL_2 + MC_1 + TACLR;              				// SMCLK, Up-Mode
	
	return 0;
} 

/********************************************************************************************************************
 * 
 * 	@brief Deshabilita todos los timers TA0 TA1 TA2 TB
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Finalizar_Timers(void)
{
	TA0CTL 		= 0x0000;
	TA0CCR0 	= 0x0000;
	TA0CCR1 	= 0x0000;
	TA0CCTL0 	= 0x0000;
	TA0CCTL1 	= 0x0000;
	
	TA1CTL 		= 0x0000;
	TA1CCR0 	= 0x0000;
	TA1CCR1 	= 0x0000;
	TA1CCTL0 	= 0x0000;
	TA1CCTL1 	= 0x0000;

	TA2CTL 		= 0x0000;
	TA2CCR0 	= 0x0000;
	TA2CCR1 	= 0x0000;
	TA2CCTL0 	= 0x0000;
	TA2CCTL1 	= 0x0000;

	TBCTL 		= 0x0000;
	TBCCR0 		= 0x0000;
	TBCCR1 		= 0x0000;
	TBCCTL0 	= 0x0000;
	TBCCTL1 	= 0x0000;
	
	return 0;
}

/********************************************************************************************************************
 * 
 * 	@brief Finaliza el timer que dispara el ADC(freq. de muestreo)
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Finalizar_Timer_ADC(void)
{
	TA0CTL 		= 0x0000;
	TA0CCR0 	= 0x0000;
	TA0CCR1 	= 0x0000;
	TA0CCTL0 	= 0x0000;
	TA0CCTL1 	= 0x0000;
		
	return 0;
}

/********************************************************************************************************************
 * 
 * 	@brief Inicializa la configuración del ADC
 * 
 * 	@params	none	
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Inicializar_ADC_Conductividad(void)
{
	ADC12CTL0 &= ~ADC12ENC;				// Deshabilito este bit para poder modificar los registros de control.
	
	////////////////////////////////// Configuración del registro ADC12CTL0 : //////////////////////////////////
	ADC12CTL0  = ADC12ON;				// Enciendo el ADC12
	ADC12CTL0 |= ADC12SHT0_15;			// Uso 1024 ciclos del Clock para S&H. (pongo en alto el bit 2).
	ADC12CTL0 |= ADC12MSC;				// Disparo toda la secuencia de conversiones con un solo pulso SAMPCON
	
	// ADC12CTL0 |= ADC12REFON;			// Enciendo la referencia del ADC.
	// ADC12CTL0 |= ADC12REF2_5V;			// Seteo la referencia de 2.5V
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////// Configuración del registro ADC12CTL1 : //////////////////////////////////    
	ADC12CTL1  = ADC12CSTARTADD_5;		// Comienzo la secuencia de conversión desde el ADC12MCTL0
	ADC12CTL1 |= ADC12SHS_1;			// Conecto la señal SHI con el TA0.CCR1(disparado con el timer A0)
	ADC12CTL1 |= ADC12SSEL_3;			// Conecto el clock ADC12CLK al SMCLK
	ADC12CTL1 |= ADC12CONSEQ_3;			// Configuro para convertir una secuencia de canales
	ADC12CTL1 |= ADC12DIV_1;			// Divido el clock por 8  
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////// Configuración de los registros ADC12MCTLx: ////////////////////////////////
	ADC12MCTL0  = 0;

	// ADC12MCTL5 = ADC12SREF_0 + ADC_IN_CONDUCTIVIDAD_3;					// VR+ = AVcc, VR- = AVss
	ADC12MCTL5 = ADC12SREF_0 + ADC_IN_CONDUCTIVIDAD_1;					// VR+ = AVcc, VR- = AVss 
	ADC12MCTL6 = ADC12SREF_0 + ADC_IN_CONDUCTIVIDAD_2 + ADC12EOS;		// VR+ = AVcc, VR- = AVss, End of Sequence
	// ADC12MCTL5 = ADC12SREF_1 + ADC_IN_CONDUCTIVIDAD_1;					// VR+ = AVcc, VR- = AVss 
	// ADC12MCTL6 = ADC12SREF_1 + ADC_IN_CONDUCTIVIDAD_2 + ADC12EOS;		// VR+ = AVcc, VR- = AVss, End of Sequence
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	//ADC12IE = 0x01 << 6;           	// Enable ADC12IFG.0. Habilito para chequear que esté funcionando bien el ADC.
	
	ADC12IE = 0x0000;					// Limpio el registro de interrupciones
	ADC12CTL0 |= ADC12ENC;				// Habilita la conversiòn

	return 0;
} 

/********************************************************************************************************************
 * 
 * 	@brief Inicializa la configuración del ADC para medir los sensores(salvo conductividad)
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/
 
int Inicializar_ADC_Sensores(void)
{
	ADC12CTL0 &= ~ADC12ENC;				// Deshabilito este bit para poder modificar los registros de control.    
    
    ////////////////////////////////// Configuración del registro ADC12CTL0 : //////////////////////////////////                                        						
	ADC12CTL0  = ADC12ON;				// Enciendo el ADC12
	ADC12CTL0 |= ADC12SHT0_6;			// Uso 64 ciclos del Clock para S&H. (pongo en alto el bit 2).
	ADC12CTL0 |= ADC12MSC;				// Disparo toda la secuencia de conversiones con un solo pulso SAMPCON
	
	// ADC12CTL0 |= ADC12REFON;			// Enciendo la referencia del ADC.
	// ADC12CTL0 |= ADC12REF2_5V;			// Seteo la referencia de 2.5V
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	////////////////////////////////// Configuración del registro ADC12CTL1 : //////////////////////////////////    
    ADC12CTL1  = ADC12CSTARTADD_0;		// Comienzo la secuencia de conversión desde el ADC12MCTL0
    ADC12CTL1 |= ADC12SHS_1;			// Conecto la señal SHI con el TA0.CCR1(disparado con el timer A0)
    ADC12CTL1 |= ADC12SSEL_3;			// Conecto el clock ADC12CLK al SMCLK  
    ADC12CTL1 |= ADC12CONSEQ_1;			// Configuro para convertir una secuencia de canales
    ADC12CTL1 |= ADC12SHP;				// Pulse Sample Mode
    ADC12CTL1 |= ADC12DIV_3;			// Divido el clock por 8
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////// Configuración de los registros ADC12MCTLx: ////////////////////////////////
	// Referencia interna de 2.5V
	// Vr+=Vref+ and Vr-=AVss | Asocio la entrada AD2 al MEM3	
	// ADC12MCTL0  = ADC12SREF_1 + ADC_IN_TEMPERATURA + ADC12EOS;
	ADC12MCTL0  = ADC12SREF_0 + ADC_IN_TEMPERATURA + ADC12EOS;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ADC12IE = 0x01 << 0;					// Habilito la interrupción asociada al MEM4	
	ADC12CTL0 |= ADC12ENC;					// Habilito las conversiones	
	return 0;	
}

/********************************************************************************************************************
 * 
 * 	@brief Inicializa y mide todos los sensores, salvo conductividad.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Iniciar_Medicion_Sensores(void)
{
	
	/////////////////////////////////////////////	Inicializacion		///////////////////////////////////////////
	Inicializar_Timer_ADC_Fs(FS_SENSORES);								// Inicializa el timer de disparo del ADC		
	Inicializar_ADC_Sensores();											// Inicializo la conversión del ADC	
	indice = 0;															// Inicio el barrido del buffer
	buffers_completos = BUFFER_ADC_DISPONIBLE;			 				// Pongo en bajo el flag de buffers llenos
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////						
	
	//////////////////////////////////////////////	Medicion	/////////////////////////////////////////////
	while(!buffers_completos);											// Espero a que termine de completar el buffer.
	Promediar_Sgn_Sensores();											// Calcula el promedio de los arrays adquiridos		
	/////////////////////////////////////////////////////////////////////////////////////////////////////////	
	
	////////////////////////////////////////// Salgo de la función //////////////////////////////////////////
	Apagar_ADC();														// Apago el ADC				
	Finalizar_Timer_ADC();												// Reseteo los registros del timer					
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	return 0;															// Salgo de la rutina de adquisición sin problemas
	
} 
 
/********************************************************************************************************************
 * 
 * 	@brief 	Inicializa y mide la celda de conductividad. Antes de llamar
 * 	@brief 	esta funcion se debe invocar primero
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/
 
 int Iniciar_Medicion_Conductividad(void)
{
 	
 	uint16_t rep = 0;														// Contador de mediciones 	
 	int32_t I_Integral_Valor_Medio_Acc = 0;									// Sumatoria de valores medios corriente
 	int32_t V_Integral_Valor_Medio_Acc = 0;									// Sumatoria de valores medios tensión

 	PWM_Activar_Salida(&pwm1);												// Inicializo y activo la salida de PWM
	Inicializar_DMA();														// Inicialización del DMA
	
	DMA_Canal_0.primer_medicion = 1;										// Indico que es la primer medición del Canal 0
	DMA_Canal_1.primer_medicion = 1;		  								// Indico que es la primer medición del Canal 1

	for(rep = 0; rep < (N_PROM); rep++)
	{
		DMA0CTL |= DMAEN;													// Habilito la interrupción generada por el canal 0 del DMA
		DMA1CTL |= DMAEN;													// Habilito la interrupción generada por el canal 1 del DMA
		
		estado_canal_0 = 0;
		estado_canal_1 = 0;

		Inicializar_Timer_ADC_Fs(FS_CONDUCTIVIDAD);							// Inicializa el timer de disparo del ADC		
		Inicializar_ADC_Conductividad();									// Inicializo la conversión del ADC
	
		while(!((estado_canal_0 == 1) && (estado_canal_1 == 1)));			// Espero a que terminen de llenarse los buffers. No hago nada mientras!!

		Procesar_Sgn_Conductividad(DMA_Canal_0, DMA_Canal_1);				// Calculo conductividad
	
		I_Integral_Valor_Medio_Acc += (int32_t) I_Acumulador;				// Guardo el valor de la integral de corriente en 64 bits
		V_Integral_Valor_Medio_Acc += (int32_t) V_Acumulador;				// Guardo el valor de la integral de tensión en 64 bits
	}
	
	PWM_Desactivar_Salida(&pwm1);											// Deshabilito la salida de PWM
	
	I_Integral_Media_Onda_Promediado = (int32_t)(I_Integral_Valor_Medio_Acc >> SHIFT_DIV_N_PROM);
	V_Integral_Media_Onda_Promediado = (int32_t)(V_Integral_Valor_Medio_Acc >> SHIFT_DIV_N_PROM);		
	
 	DMA0CTL &= 	~DMAEN;														// Deshabilito la interrupción generada por el canal 0 del DMA
	DMA1CTL &= 	~DMAEN;														// Deshabilito la interrupción generada por el canal 1 del DMA	

	Apagar_ADC();															// Apago el ADC				
	Finalizar_Timer_ADC();													// Reseteo los registros del timer	

 	return 0;																// Vuelvo
}

/********************************************************************************************************************
 * 
 * 	@brief Calcula el promedio de la medicion de los sensores.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Promediar_Sgn_Sensores(void)
{
	uint16_t i;
	
	int32_t acc_0 = 0;		// Acumulador para calcular el promedio de adc_medicion_0
			
	for(i = 0;i < N_BUFFER_SENSORES;i++)
	{		
		acc_0 += medicion_adc_0[i];
	}	
		
	////////////////// Calculo Promedios /////////////////////
	PT100PromedioObtenido	= (int16_t) (acc_0 / N_BUFFER_SENSORES);
	
	return 0;
}

/********************************************************************************************************************
 * 
 * 	@brief Rutina de atención de Interrupción del ADC.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch (__even_in_range(ADC12IV, ADC12IV_ADC12IFG15))
    {
	case ADC12IV_NONE:			break;					// Vector  ADC12IV_NONE:  No interrupt		
	case ADC12IV_ADC12OVIFG:	break;					// Vector  ADC12IV_ADC12OVIFG:  ADC overflow                    
	case ADC12IV_ADC12TOVIFG:	break;					// Vector  ADC12IV_ADC12TOVIFG:  ADC Temperature overflow
	case ADC12IV_ADC12IFG0:								// Vector  ADC12IV_ADC12IFG0: ADC12IFG0:

		ADC12CTL0 &= ~ADC12ENC;							// Deshabilito este bit para poder modificar los registros de control.				
		// Leo todos los canales para limpiar los bits de "interrupción pendiente"
		if (indice >= N_BUFFER_SENSORES) 				// Incremento el contador
		{
			indice = 0;									// Si llego al final, reinicio la cuenta.
			buffers_completos = BUFFER_ADC_LLENO;
		} 		
		medicion_adc_0[indice] = ADC12MEM0;
		indice++;
		ADC12CTL0 |= ADC12ENC;							// Toggle ENC para volver a convertir  		

		break;		
	case ADC12IV_ADC12IFG1:		break;					// Vector ADC12IV_ADC12IFG1:  ADC12IFG1
	case ADC12IV_ADC12IFG2:		break;					// Vector ADC12IV_ADC12IFG2:  ADC12IFG2
	case ADC12IV_ADC12IFG3:		break;					// Vector ADC12IV_ADC12IFG3:  ADC12IFG3
	case ADC12IV_ADC12IFG4:		break;					// Vector ADC12IV_ADC12IFG4:  ADC12IFG4
	case ADC12IV_ADC12IFG5:		break;					// Vector ADC12IV_ADC12IFG5:  ADC12IFG5
	case ADC12IV_ADC12IFG6:		break;					// Vector ADC12IV_ADC12IFG6:  ADC12IFG6    
	case ADC12IV_ADC12IFG7:		break;					// Vector ADC12IV_ADC12IFG7:  ADC12IFG7
	case ADC12IV_ADC12IFG8:		break;					// Vector ADC12IV_ADC12IFG8:  ADC12IFG8
	case ADC12IV_ADC12IFG9:		break;					// Vector ADC12IV_ADC12IFG9:  ADC12IFG9
	case ADC12IV_ADC12IFG10:	break;					// Vector ADC12IV_ADC12IFG10:  ADC12IFG10
	case ADC12IV_ADC12IFG11:	break;					// Vector ADC12IV_ADC12IFG11:  ADC12IFG11
	case ADC12IV_ADC12IFG12:	break;					// Vector ADC12IV_ADC12IFG12:  ADC12IFG12
	case ADC12IV_ADC12IFG13:	break;					// Vector ADC12IV_ADC12IFG13:  ADC12IFG13
	case ADC12IV_ADC12IFG14:	break;					// Vector ADC12IV_ADC12IFG14:  ADC12IFG14
	case ADC12IV_ADC12IFG15:	break;					// Vector ADC12IV_ADC12IFG15:  ADC12IFG15
	    	    
	default:					break;
    	
    }
}

/***********************************************************************************************************************
 *												Final del archivo													   *							
 ***********************************************************************************************************************/
