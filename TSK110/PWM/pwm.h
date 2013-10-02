/********************************************************************************************************************
 *	@file pwm.h																										*
 * 																													*
 * 	@brief header del archivo del pwm.c																				*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/
 
#ifndef PWM_H_
#define PWM_H_

/********************************************************************************************************************
 * 											DEFINICIÓN DE CONSTANTES												*
 ********************************************************************************************************************/
 
#define PWM_INIT_SALIDA0		0			// Habilita la salida PWM sobre el port 1.7
#define PWM_INIT_SALIDA1		1			// Habilita la salida PWM sobre el port 2.0
#define PWM_INIT_SALIDA2		2			// Habilita la salida PWM sobre el port 2.1

#define DUTY_MAX_INICIAL		100			// Referencia de duty por defecto = 100%
#define DUTY_INICIAL			50			// Duty inicial 50%
#define FSW_INICIAL 			240			// Frecuencia de switching inicial

/********************************************************************************************************************
 * 											DEFINICIÓN DE TIPOS														*
 ********************************************************************************************************************/

typedef enum pwm_res
{
	RESOLUCION_DAC_1BIT = 2,
	RESOLUCION_DAC_2BIT	= 4,
	RESOLUCION_DAC_3BIT	= 8,
	RESOLUCION_DAC_4BIT	= 16,
	RESOLUCION_DAC_5BIT	= 32,
	RESOLUCION_DAC_6BIT	= 64,
	RESOLUCION_DAC_7BIT	= 128,
	RESOLUCION_DAC_8BIT	= 256
	
} PWM_Resolucion;


typedef struct pwm_var  
{
	char 	 timer_x;									// Timer al que está asignada la salida PWM

	// Registros asociados al hardware del timer
	volatile unsigned int* registro_ccr_duty;			// Puntero al registro con el que modifico el duty 
	volatile unsigned int* registro_timer_ctl;			// Puntero al registro de control del timer
	
	volatile unsigned int* registro_ccr_sw; 			// Puntero al registro con el que modifico la frecuencia del pwm
	volatile unsigned int* registro_ccr_ctl;			// Puntero al registro de control de la salida PWM
	volatile unsigned int* registro_hab_int;			// Puntero al registro de habilitación de interrupción
	
	// Pin físico del timer
	uint16_t port_salida;								// Puerto de salida
	uint16_t bit_salida;								// Bit de salida
	
	// Cuentas a guardar en los registros
	uint16_t cuenta_periodo;							// Cuenta a realizar para lograr la frecuencia del PWM
	uint16_t cuenta_t_on;								// Cuenta a realizar para lograr el tiempo en alto (duty)
	
	// Frecuencias de clock y switching
	uint16_t frec_ref;									// Frecuencia de referencia para generar la señal
	uint16_t frec_sw;									// Frecuencia de PWM
	
	// Ciclo de trabajo actual y total
	uint16_t duty;										// Ciclo de trabajo en referencia a duty_max
	uint16_t duty_max;									// Ciclo de trabajo total para obtener una contínua sin modular (100%)
	
	Type_Sgn	sgn_a_modular;							// Señal a modular como salida del DAC
	
	uint16_t 	cuenta_fs_i;							// Contador para dividir la fsw															
	uint16_t 	cuenta_fs_div;							// Cuenta maxima del divisor de fsw
	
} PWM_Var;

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/

extern int PWM_Init(PWM_Var* PWM_Sel, char timer, uint8_t salida);											// Inicialización de la salida PWM
extern int PWM_Setear_Fsw (PWM_Var* PWM_Sel, uint16_t frec_sw);												// Establece una frecuencia de sw 		
extern int PWM_Setear_Duty(PWM_Var* PWM_Sel, uint16_t duty_refresco);										// Establece un ciclo de trabajo determinado
extern void PWM_Activar_Salida(PWM_Var* PWM_Sel);															// Activa la salida PWM
extern int PWM_Desactivar_Salida(PWM_Var* PWM_Sel); 														// Desactiva la salida PWM
extern int PWM_Reset_Campos(PWM_Var* PWM_Sel);																// Reinicializa los contadores asociados al PWM

extern int PWM_Init_TA1_1(PWM_Var* PWM_Sel);

extern int PWM_Convertir_DAC(PWM_Var* PWM_Sel, Sgn_Res muestra_a_escribir);									// Setea el duty a partir de la muestra a convertir
extern int PWM_Cargar_Sgn(PWM_Var* PWM_Sel, Sgn_Res* sgn_a_cargar, int16_t sgn_largo, uint16_t fs);			// Carga una señal a modular en memoria:
extern int PWM_Setear_Resolucion_DAC(PWM_Var* PWM_Sel, PWM_Resolucion PWM_Res); 							// Setea la fsw a partir de la resolucion:

/********************************************************************************************************************
 * 											VARIABLES GLOBALES EXTERNAS												*
 ********************************************************************************************************************/

extern PWM_Var pwm1;

#endif /*PWM_H_*/

/************************************************************************************
 * 								Fin del Archivo										*
 ************************************************************************************/
