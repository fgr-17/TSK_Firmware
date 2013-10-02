/****************************************************************************************************************
 *
 * 	Archivo : HAL_RTC.h
 * 
 * 	Descripción: header del archivo HAL_RTC.h
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 12/2011
 * 
 ****************************************************************************************************************/
 
/****************************************************************************************************************
 * 											Definicion de constantes											*
 ****************************************************************************************************************/ 

#define TIMEOUT_RECEPCION_UART1_SEGUNDOS				1					// Tiempo [seg] en que se tarda en descartar un frame incompleto recibido en UART1
#define TIMEOUT_RECEPCION_SLAVE_SEGUNDOS				20					// Tiempo [seg] que se espera sin recibir datos del slave antes de suponer la comunicacion perdida. Se dispara auto.

//////////////////////////////////////////////////////////////////////////////
// 								Hora Inicial:								//
//////////////////////////////////////////////////////////////////////////////

#define		HOUR_BCD_INIT			0x12
#define		MINUTE_BCD_INIT			0x00
#define		SECOND_BCD_INIT			0x00

//////////////////////////////////////////////////////////////////////////////
// 								Fecha Inicial:								//
//////////////////////////////////////////////////////////////////////////////

#define		YEAR_BCD_INIT			0x2012
#define		MONTH_BCD_INIT			0x03
#define		DAY_BCD_INIT			0x10

/****************************************************************************************************************
 * 											Declaración de tipos												*
 ****************************************************************************************************************/ 

typedef enum {FALSE = 0, TRUE} T_Boolean;
typedef enum {CUENTA, LIMPIAR_BUFFER, DETENIDO} T_Estado_Timeout;
typedef struct 	{
					T_Estado_Timeout	estado_delay_Timer;
					uint16_t 			cuenta_delay;
					uint16_t 			cuenta_max;
				} T_Timeout_Frame_Recibido;
				
typedef struct 	{
					T_Estado_Timeout	estado_delay_Timer;
					uint16_t 			cuenta_delay;
					uint16_t 			cuenta_max;
					T_Boolean			slave_vivo;
				} T_Timeout_Slave;
										
 
/****************************************************************************************************************
 * 												Funciones externas												*
 ****************************************************************************************************************/
 
 
extern int Timeout_Slave_Desconectado(void);
extern int Activar_Timeout_Slave(void);
extern int Inicializar_Timeout_Slave(void);
 
extern int Leer_Registros_RTC(T_Boolean lectura_bloqueante);
extern int Establecer_Registros_RTC(uint8_t hora, uint8_t minuto, uint8_t segundo, uint8_t dia, uint8_t mes, uint8_t anio); 
extern int Inicializar_Timeout_UART_1(void);
extern void Init_RTC(void);

/****************************************************************************************************************
 * 											Variables Globales Externas											*
 ****************************************************************************************************************/

extern uint8_t sec_BCD;
extern uint8_t min_BCD;
extern uint8_t hour_BCD;

extern uint8_t day_BCD; 
extern uint8_t mon_BCD;
extern uint16_t year_BCD;
 
extern T_Boolean lectura_registros_RTC; 
extern T_Timeout_Frame_Recibido timeout_UART1;
extern T_Timeout_Slave timeout_Slave;

/****************************************************************************************************************
 * 												Final del archivo												*
 ****************************************************************************************************************/
