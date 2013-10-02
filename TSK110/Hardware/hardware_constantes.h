/********************************************************************************************************************
 *	@file hardware_constantes.h																						*
 * 																													*
 * 	@brief Valores constantes relacionados con el hardware															*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 11/2011																									*
 ********************************************************************************************************************/
 
#ifndef HARDWARE_CONSTANTES_H_
#define HARDWARE_CONSTANTES_H_

/******************************************************************************************** 	
 * 									Definición de Constantes								*
 ********************************************************************************************/

#define FRECUENCIA_XTAL					4000			// Frecuencia del cristal en kHz
#define FRECUENCIA_SISTEMA				25000			// Frecuencia del sistema después de aumentar la velocidad con el FLL

#define ANALOG_SHUTDOWN_PORT_OUT		P8OUT			// Puerto asociado al pin conectado al SHUTDOWN del regulador analógico
#define ANALOG_SHUTDOWN_PIN_OUT			BIT0			// Pin del puerto anterior conectado al SHUTDOWN del regulador analógico


/////////////////////////////////////////////////
////////        Pines de la UART         ////////
/////////////////////////////////////////////////
#define	UCA0TXD_PIN						3, 3
#define	UCA0RXD_PIN						3, 4

#define	PM_UCA1TXD_PIN					4, 4
#define	PM_UCA1RXD_PIN					4, 5

/////////////////////////////////////////////////
////////   Pines de salida de los leds   ////////
/////////////////////////////////////////////////
#define	LED_CALIB_PIN_O					1, 0
#define	LED_CONFIG_PIN_O				1, 1
#define	LED_SLAVE_PIN_O					1, 2
#define	LED_MASTER_PIN_O				1, 3

/////////////////////////////////////////////////
////////   	  Pines de salida varios     ////////
/////////////////////////////////////////////////
#define REG_ANALOG_SHUTDOWN_PIN_O		8, 0
#define DAC_PWM_PIN_O					2, 0

/////////////////////////////////////////////////
//////// Pines de entrada analógicos ADC ////////
/////////////////////////////////////////////////
#define CONDUCTIVIDAD_1_PIN_I			6, 0
#define CONDUCTIVIDAD_2_PIN_I			6, 1
#define CONDUCTIVIDAD_3_PIN_I			6, 2

#define	TEMPERATURA_PIN_I				6, 3
		
////////////////////////////////////////////////
//////// 	Pines y ports del cristal	////////
////////////////////////////////////////////////
#define XT1_XT2_PORT_DIR            P5DIR
#define XT1_XT2_PORT_OUT            P5OUT
#define XT1_XT2_PORT_SEL            P5SEL
#define XT1_ENABLE                  (BIT4 + BIT5)
#define XT2_ENABLE                  (BIT2 + BIT3)

/////////////////////////////////////////////////
#endif /*HARDWARE_CONSTANTES_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
