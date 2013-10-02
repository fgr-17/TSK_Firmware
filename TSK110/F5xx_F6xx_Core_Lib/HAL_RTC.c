/****************************************************************************************************************
 *
 * 	Archivo : HAL_RTC.c
 * 
 * 	Descripción: configuración y uso del Real Time Clock
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 12/2011
 * 
 ****************************************************************************************************************/

#ifndef HAL_RTC_H
#define HAL_RTC_H

/****************************************************************************************************************
 * 												Inclusión de archivos											*
 ****************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>
#include <HAL_RTC.h>
#include <strbus.h>

/****************************************************************************************************************
 * 												Prototipos de funciones											*
 ****************************************************************************************************************/

int Timeout_Slave_Desconectado(void);
int Activar_Timeout_Slave(void);
int Inicializar_Timeout_Slave(void);

int Inicializar_Timeout_UART_1(void);

int Leer_Registros_RTC(T_Boolean lectura_bloqueante);
int Establecer_Registros_RTC(uint8_t hora, uint8_t minuto, uint8_t segundo, uint8_t dia, uint8_t mes, uint8_t anio);

void Init_RTC(void);

/****************************************************************************************************************
 * 												Variables Globales												*
 ****************************************************************************************************************/

uint8_t sec_BCD 	= 0;
uint8_t min_BCD 	= 0;
uint8_t hour_BCD	= 0;

uint8_t day_BCD 	= 0; 
uint8_t mon_BCD 	= 0;
uint16_t year_BCD 	= 0;

T_Boolean lectura_registros_RTC = FALSE;
T_Timeout_Frame_Recibido timeout_UART1;
T_Timeout_Slave timeout_Slave;
uint16_t contador_ts = 0;

/****************************************************************************************************************
 * 											Implementación de funciones											*
 ****************************************************************************************************************/

/****************************************************************************************************************
 * @brief	Inicializa la estructura con las variables del timeout de disparo del slave							
 * 
 * @param  lectura_bloqueante	TRUE:  espera hasta tener la lectura
 * 								FALSE: la funcion finaliza y hago la lectura cuando esten los datos disponibles
 * 
 * @return none
 ****************************************************************************************************************/

int Inicializar_Timeout_Slave(void)
{
	timeout_Slave.cuenta_delay = 0;
	timeout_Slave.cuenta_max = TIMEOUT_RECEPCION_SLAVE_SEGUNDOS;
	timeout_Slave.estado_delay_Timer = DETENIDO;	
	timeout_Slave.slave_vivo = TRUE;																			// Supongo inicialmente que el slave está OK
	return 0;
}

/****************************************************************************************************************
 * @brief	Activa la estructura con las variables del timeout de disparo del slave, para empezar a contar							
 * 
 * @param  lectura_bloqueante	TRUE:  espera hasta tener la lectura
 * 								FALSE: la funcion finaliza y hago la lectura cuando esten los datos disponibles
 * 
 * @return none
 ****************************************************************************************************************/

int Activar_Timeout_Slave(void)
{
	RTCCTL0 = RTCRDYIE + RTCTEVIE;	        																// Habilito la ISR del RTC
	timeout_Slave.cuenta_delay = 0;
	timeout_Slave.cuenta_max = TIMEOUT_RECEPCION_SLAVE_SEGUNDOS;
	timeout_Slave.estado_delay_Timer = CUENTA;	
	timeout_Slave.slave_vivo = TRUE;																		// Supongo inicialmente que el slave está OK
	return 0;
}

/****************************************************************************************************************
 * @brief	Activa la estructura con las variables del timeout de disparo del slave, para empezar a contar							
 * 
 * @param  lectura_bloqueante	TRUE:  espera hasta tener la lectura
 * 								FALSE: la funcion finaliza y hago la lectura cuando esten los datos disponibles
 * 
 * @return none
 ****************************************************************************************************************/

int Timeout_Slave_Desconectado(void)
{
	timeout_Slave.cuenta_delay = 0;
	timeout_Slave.cuenta_max = TIMEOUT_RECEPCION_SLAVE_SEGUNDOS;
	timeout_Slave.estado_delay_Timer = DETENIDO;	
	timeout_Slave.slave_vivo = FALSE;																			// Supongo inicialmente que el slave está OK
	return 0;
}

/****************************************************************************************************************
 * @brief	Inicializa la estructura con las variables del timeout de cadena recibida									
 * 
 * @param  lectura_bloqueante	TRUE:  espera hasta tener la lectura
 * 								FALSE: la funcion finaliza y hago la lectura cuando esten los datos disponibles
 * 
 * @return none
 ****************************************************************************************************************/

int Inicializar_Timeout_UART_1(void)
{
	timeout_UART1.cuenta_delay = 0;
	timeout_UART1.cuenta_max = TIMEOUT_RECEPCION_UART1_SEGUNDOS;
	timeout_UART1.estado_delay_Timer = DETENIDO;	
	return 0;
}

/****************************************************************************************************************
 * @brief	Lee la información del RTC y la vuelca en las variables	
 * 			Ver "MSP430x5xx/MSP430x6xx User's Guide" pág. 401/402									
 * 
 * @param  lectura_bloqueante	TRUE:  espera hasta tener la lectura
 * 								FALSE: la funcion finaliza y hago la lectura cuando esten los datos disponibles
 * 
 * @return none
 ****************************************************************************************************************/
 
int Leer_Registros_RTC(T_Boolean lectura_bloqueante)
{
	lectura_registros_RTC = TRUE;								// Habilito la lectura de registro por interrupción	
	RTCCTL0 |= RTCRDYIE + RTCTEVIE;         					// Habilito las IRQ
	
	switch(lectura_bloqueante)						
	{
	case TRUE:
		while(lectura_registros_RTC == TRUE);					// Si está en modo bloqueante, espero a tener la lectura hecha
		break;
	case FALSE:													// Sino sigo de largo, y en el contexto superior chequeo el estado de lectura_registros_RTC 
		break;
	default:
		break;
	}
	
	return 0; 				
}

/****************************************************************************************************************
 * @brief	Establezco el valor de los registros de horario											
 * 
 * @param  none
 * 
 * @return none
 ****************************************************************************************************************/

int Establecer_Registros_RTC(uint8_t hora, uint8_t minuto, uint8_t segundo, uint8_t dia, uint8_t mes, uint8_t anio)
{
	RTCCTL01 = RTCBCD + RTCMODE + RTCHOLD + RTCTEV_1;																// Modo calendario, registros en BCD
    
	if(hora < 0x24) RTCHOUR = hora;	else return 1;																	// Hora de 0 a 23		
	if(minuto < 0x60) RTCMIN = minuto; else return 1;																// Minuto de 0 a 59
	if(segundo < 0x60) RTCSEC = segundo; else return 1;																// Segundo de 0 a 59
	
	if((dia <= 0x31) && (dia > 0)) RTCDAY = dia; else return 1;														// Dia del 1 al 
	if((mes <= 0x12 ) && (mes > 0)) RTCMON = mes; else return 1;														// Mes del 1 al 12 :P
	
	RTCYEAR = anio;																									// Al año se le puede mandar fruta	

    RTCCTL01 &= ~RTCHOLD;

    RTCPS1CTL = RT1IP_5;                    																		// Interrupt freq: 2Hz
    RTCPS0CTL = RT0IP_7;                    																		// Interrupt freq: 128hz

    RTCCTL0 |= RTCRDYIE + RTCTEVIE;         																		// Enable interrupt
    return 0;																										// Si salio todo bien, devuelvo éxito!
} 
 
/*******************************************************************************
 * @brief	Initializes the RTC calendar.											
 * @brief 	Initial values are January 01, 2010, 12:30:05
 * 
 * @param  none
 * 
 * @return none
 *******************************************************************************/

void Init_RTC(void)
{
    RTCCTL01 = RTCBCD + RTCMODE + RTCHOLD + RTCTEV_1;		// Modo calendario, registros en BCD
    	
	// Valores iniciales de hora:
	
	RTCHOUR = HOUR_BCD_INIT;
	RTCMIN = MINUTE_BCD_INIT;
	RTCSEC = SECOND_BCD_INIT;
			
	// Valores iniciales de fecha:			
	
	RTCDAY = DAY_BCD_INIT;	
	RTCMON = MONTH_BCD_INIT;	
	RTCYEAR = YEAR_BCD_INIT;	

    RTCCTL01 &= ~RTCHOLD;

    RTCPS1CTL = RT1IP_5;                    // Interrupt freq: 2Hz
    RTCPS0CTL = RT0IP_7;                    // Interrupt freq: 128hz

    RTCCTL0 |= RTCRDYIE + RTCTEVIE;         // Enable interrupt
}

/********************************************************************************
 * @brief  RTC Interrupt Service Routine. Handles time events.
 * 
 * @param  none
 * 
 * @return none
 ********************************************************************************/
 
#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{	
    switch (__even_in_range(RTCIV, RTC_RT1PSIFG))
    {
        case RTC_NONE:																					// Vector RTC_NONE: No Interrupt pending
            break;
        
        case RTC_RTCRDYIFG:																				// Vector RTC_RTCRDYIFG: RTC ready  
        
        	//////////////////////////////////////////////////////////////////////////////
    		//							Timeouts por RTC								//
        	//////////////////////////////////////////////////////////////////////////////
        	// Timeout referente al disparo del Slave. Sólo válido para la configuación Master.
        	// Cuando se entra al modo adquisición, el slave hace una medición y se la envía al Master
        	// Esto actúa como un disparador para el Master, que al recibir un dato también hace una medición.
        	// Si por alguna razón, se pierde la comunicación con el slave, se dispara un timeout de 5 segundos
        	// que en caso de cumplirse, hace que el Master se dispare solo para tomar muestras, y las mediciónes
        	// del slave queden anuladas.
        	if(timeout_Slave.estado_delay_Timer == CUENTA)
        	{
        		timeout_Slave.cuenta_delay++;
        		if(timeout_Slave.cuenta_delay >= timeout_Slave.cuenta_max)
        		{
        			timeout_Slave.cuenta_delay = 0;
 					timeout_Slave.cuenta_max = TIMEOUT_RECEPCION_SLAVE_SEGUNDOS;
					timeout_Slave.estado_delay_Timer = DETENIDO;	
					timeout_Slave.slave_vivo = FALSE;													// Se perdio la conexión del slave.
					terminal.Ts = TS_MODO_MEDIR;
					
        		}
        	}
        	// Timeout referente a la recepción de paquetes. Todas las cadenas de comunicación tienen un 
        	// largo predefinido, y recién son validadas cuando se completa su longitud. Si se reciben cadenas
        	// incompletas, estas se limpian al cumplirse el Timeout para cada byte recibido.
        	if(timeout_UART1.estado_delay_Timer == CUENTA)
        	{
        		timeout_UART1.cuenta_delay++;
        		if(timeout_UART1.cuenta_delay >= timeout_UART1.cuenta_max)
        		{
        			timeout_UART1.cuenta_delay = 0;
        			timeout_UART1.estado_delay_Timer = LIMPIAR_BUFFER;
        			canal_rx_2.ind = 0;
        			canal_rx_2.estado_buffer = canal_rx_2.estado_buffer_anterior;
					// canal_rx_2.estado_buffer = BUFFER_VACIO;
        			
        		}
        	}
        	//////////////////////////////////////////////////////////////////////////////
    		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
        	//////////////////////////////////////////////////////////////////////////////
        	
        	if(lectura_registros_RTC == TRUE)															// Entré a la IRQ por una lectura de registro
        	{
        		lectura_registros_RTC = FALSE;
        		// RTCCTL0 &= ~(RTCRDYIE + RTCTEVIE);         	// disable interrupt
			
				hour_BCD	= RTCHOUR;						
				min_BCD		= RTCMIN;
				sec_BCD		= RTCSEC;
				
				day_BCD		= RTCDAY;
				mon_BCD		= RTCMON;
				year_BCD	= RTCYEAR; 
        	}
        	
        	// Uso el RTC para inicializar una medición, el intervalo es cargado en el campo Ts de la estructura. 
        	// Se usa en medición del master, calibración de conductividad y de temperatura.
        	if(((terminal.modos_estado == SLAVE_MIDE)||(terminal.modos_estado == CALIBRACION_K)||(terminal.modos_estado == CALIBRACION_T))         	        	        		
        		&& (terminal.medicion == TERMINAL_LIBRE))        		
        	{        		        		
        		contador_ts++;
        		if(contador_ts >= terminal.Ts)
        		{
        			terminal.medicion = TERMINAL_MEDIR;
        			contador_ts = 0;
        		}        	        
        	}        	
        	// Si se perdió la comunicación con el slave, el master se dispara automáticamente para adquirir.
        	else if((terminal.modos_estado == MASTER_MIDE)&&(timeout_Slave.slave_vivo == FALSE))
        	{
        		contador_ts++;
        		if(contador_ts >= terminal.Ts)
        		{
        			terminal.medicion = TERMINAL_MEDIR;
        			contador_ts = 0;
        		}        	        
        	}
        	else
        		contador_ts = 0;        	      
        	
            break;
            
        // Vector RTC_RTCTEVIFG: RTC interval timer
        case RTC_RTCTEVIFG:
            break;

        // Vector RTC_RTCAIFG: RTC user alarm
        case RTC_RTCAIFG:
            break;

        // Vector RTC_RT0PSIFG: RTC prescaler 0
        case RTC_RT0PSIFG:
            break;

        // Vector RTC_RT1PSIFG: RTC prescaler 1
        case RTC_RT1PSIFG:
            break;

        // Default case
        default:
            break;    	
	}
	
}

#endif /* HAL_RTC_H */
/****************************************************************************************************************
 * 													Final del archivo 											*
 ****************************************************************************************************************/
