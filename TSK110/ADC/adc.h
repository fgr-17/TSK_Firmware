/********************************************************************************************************************
 *	@file adc.h																										*
 * 																													*
 * 	@brief header del archivo del adc.h																				*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/


#ifndef ADC_H_
#define ADC_H_

/********************************************************************************************************************
 * 											DEFINICIÓN DE CONSTANTES												*
 ********************************************************************************************************************/ 

#define FS_CONDUCTIVIDAD				150				// Frecuencia de muestreo inicial en [kHz]
#define FS_SENSORES						1				// Frecuencia de muestreo de sensores [kHz]

#define N_BUFFER_SENSORES				64
#define CANT_BARRIDOS					1

#define BUFFER_ADC_DISPONIBLE			0				// Token para señalizar la variable global "buffers_lleno" si el buffer no está lleno.
#define BUFFER_ADC_LLENO				1				// Token para señalizar la variable global "buffers_lleno" si el buffer está lleno.

//#define N_PROM 						50				// Cantidad de mediciones a promediar

#define ADC_IN_CONDUCTIVIDAD_1		ADC12INCH_0
#define ADC_IN_CONDUCTIVIDAD_2		ADC12INCH_1
#define ADC_IN_CONDUCTIVIDAD_3		ADC12INCH_2
#define ADC_IN_TEMPERATURA			ADC12INCH_3

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

extern int Inicializar_Timer_ADC_Fs(uint16_t fs);	// Inicializa el timer de disparo del ADC
extern int Finalizar_Timers(void); 					// Resetea los registros del timer1
extern int Finalizar_Timer_ADC(void);

extern int Inicializar_ADC_Conductividad(void);		// Inicializa la configuración del ADC
extern int Inicializar_ADC_Sensores(void);			// Inicializo la configuración del ADC para los sensores.

extern int Iniciar_Medicion_Sensores(void);			// Inicio la medición con los sensores.
extern int Iniciar_Medicion_Conductividad(void);	// Inicio la medición de conductividad

extern int Promediar_Sgn_Sensores(void);			// Promedio la medicion de todos los sensores.
extern int Apagar_ADC(void);						// Limpio los registros de control del ADC.

/********************************************************************************************************************
 * 											VARIABLES GLOBALES EXTERNAS												*
 ********************************************************************************************************************/
  
extern uint16_t medicion_adc_0[N_BUFFER_SENSORES];
extern uint16_t medicion_adc_1[N_BUFFER_SENSORES];
extern uint16_t medicion_adc_2[N_BUFFER_SENSORES];
extern uint16_t medicion_adc_3[N_BUFFER_SENSORES];
extern uint16_t medicion_adc_4[N_BUFFER_SENSORES];

extern volatile uint32_t indice;
extern volatile uint8_t  buffers_completos;	

extern volatile long conductividadPromedioObtenida;
extern int16_t presionMP3VObtenida;
extern int16_t presionMPM281Obtenida;
extern int16_t oxigenoPromedioObtenido;
extern int16_t PT100PromedioObtenido;
extern int16_t bateriaPromedioObtenida;



#endif /*ADC_H_*/
/***********************************************************************************************************************
 *												Final del archivo													   *							
 ***********************************************************************************************************************/
