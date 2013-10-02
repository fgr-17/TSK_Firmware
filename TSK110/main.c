/********************************************************************************************************
 *																										*
 * 	Archivo : main.c																					*
 * 																										*
 * 	Descripción: 	Módulo principal del programa comandado desde una terminal a través 				*
 * 					del puerto serie																	*
 * 																										*
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)													*
 * 																										*
 * 	NEROX 11/2011																						*
 *																										* 
 ********************************************************************************************************/

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/
 
#include <msp430f5529.h>
#include <stdint.h>

#include <hardware_macros.h>
#include <hardware_constantes.h>
#include <hardware_drivers.h>

#include <HAL_UCS.h>
#include <HAL_PMM.h>
#include <HAL_RTC.h>

#include <HAL_Board.h>

#include <modos.h>
#include <strbus.h>
#include <uart.h>

#include <adc.h>
#include <dma.h>
#include <pwm.h>
#include <dsp.h>

#include <sensores.h>

#include <Timer_A2.h>

#include <log.h>
#include <xbob_4.h>

#include <Temperatura.h>
#include <Conductividad.h>
#include <SD_Card.h>

// #include <RST_NMI.h>
#include <flash_mapeo_variables.h>

/********************************************************************************************************
 * 										Prototipos de funciones											*
 ********************************************************************************************************/
 
int Inicializar_FLL(void); 
int Inicializar_Master_Midiendo(void);

/********************************************************************************************************
 * 											Variables globales											*
 ********************************************************************************************************/

int haySD = 0;

/********************************************************************************************************
 * 											Programa principal											*
 ********************************************************************************************************/

int main(void)
{	
	Inicializacion_Hardware_Board();													// Inicializo el hardware en general
	Inicializacion_Hardware_Conductividad();											// Inicializo la salida PWM y las entradas del ADC de conductividad
	
	Inicializar_FLL();																	// Inicializo el FLL
	
	Inicializar_Terminal();																// Inicializo la estructura de la terminal del equipo
 	
 	Inicializar_Canal_1();																// Inicializo el canal 1
 	Inicializar_Canal_2();																// Inicializo el canal 2

	Inicializar_XBOB();																	// Inicalizo el software del XBOB4
	
	Inicializar_Timeout_UART_1();														// Inicializo el timeout de la UART1	 	
 	Inicializar_UART0();																// Inicializo la comunicación serie UART0
	Inicializar_UART1();																// Inicializo la comunicación serie UART1
	
	Init_RTC();																			// Inicializo el Real Time Clock
 	
 	if(ExistingFile(ArchivoGeneral))													// Si el archivo existe, sino lo creo
		haySD = ArchivoLogGeneral("TSK100_Info", "El equipo se inició");				// Guardo la inicialización del equipo

	RecuperarVariablesFlash();
		 	 
	// Inicializar_Master_Midiendo();		 	 
		 	 
    __enable_interrupt();																// Habilito las interrupciones
    	 		 	 	
 	while(1)
 	{  		
 		Frame_Timeout(&canal_rx_0, delay_timer_A2);										// Pregunto si tengo que limpiar el buffer de entrada del canal rx 1
 		Procesar_Frame_Recibido(&canal_rx_0, &canal_tx_0);								// Proceso el frame del canal rx 1 en caso que haya uno pendiente
 		Procesar_Frame_Recibido(&canal_rx_2, &canal_tx_2);								// Proceso el frame del canal rx 1 en caso que haya uno pendiente 		
 		Medicion_Sensores();															// Hago una medición de acuerdo al estado del equipo
 		Modos_Maq_Estados();															// Evaluo la máquina de estados
 		Frame_Respuesta(&canal_tx_0, canal_rx_0, UART0);								// Respondo en caso que sea necesario
 		Frame_Respuesta(&canal_tx_2, canal_rx_2, UART1);								// Respondo en caso que sea necesario
 	}	
}

/************************************************************************************************************
 * 									Implementación de funciones	locales										*
 ************************************************************************************************************/

/************************************************************************************************************
 *	@brief 		Inicializo el hardware del sistema para que funcione a 25MHz								*
 * 																											*
 * 	@params 	none																						*
 * 																											* 		
 * 	@returns 	none																						*
 * 																											*
 ************************************************************************************************************/

int Inicializar_FLL(void)
{
    WDTCTL = WDTPW + WDTHOLD;															// Detengo el Watchdog
	XT1_XT2_PORT_SEL |= XT1_ENABLE + XT2_ENABLE;										// Selecciono los pines del xtal		
    SetVCore(3);																		// Seteo la tensión del Core para máxima velocidad    
    LFXT_Start(XT1DRIVE_0);																// Uso el xtal de 32.768kHz como referencia
    Init_FLL_Settle(FRECUENCIA_SISTEMA, 762);											// Llevo el system clock al máx (25MHz)
    SFRIFG1 = 0;																		// Limpio el flag de irq pendiente de los flags del sistema
    SFRIE1 |= OFIE;																		// Habilito la interrupción de overflow del sistema
	
	return 0;
}

/************************************************************************************************************
 *	@brief 		Inicializo el hardware del sistema para que funcione a 25MHz								*
 * 																											*
 * 	@params 	none																						*
 * 																											* 		
 * 	@returns 	none																						*
 * 																											*
 ************************************************************************************************************/
 
int Inicializar_Master_Midiendo(void)
{
	timeout_Slave.slave_vivo = FALSE;
	terminal.modos_estado = MASTER_MIDE;
	terminal.Ts = 3;
	
	return 0;	
	
} 
 

/************************************************************************************************************ 	
 * 											Final del Archivo												*
 ************************************************************************************************************/
