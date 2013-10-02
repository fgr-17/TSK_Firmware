/*********************************************************************************************************
 * 
 *	@file Temperatura.c
 * 
 * 	@brief Calculo de la temperatura, calibracion
 * 
 *  @version 1.0
 * 
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	@date 06/2011
 * 
 *********************************************************************************************************/

/************************************************************************************************************
 * 											Inclusión de archivos											*
 ************************************************************************************************************/

#include <Temperatura.h>

#include <adc.h>

/************************************************************************************************************
 * 												Variables globales											*
 ************************************************************************************************************/

int16_t PT100A = 1137;            												// Primera medicion 100 ohm
int16_t PT100B = 3663;          												// Segunda medicion 194 ohm
long PPT100;           															// Pendiente de la PT100

int32_t ADC_R_Alto = 3700;														// Valor medido de ADC correspondiente al valor alto de resistencia para calibración
int32_t ADC_R_Bajo = 1000;														// Valor medido de ADC correspondiente al valor bajo de resistencia para calibración
int32_t R_Alto = 1194;															// Valor alto de resistencia para calibración
int32_t R_Bajo = 1000;															// Valor bajo de resistencia para calibración

int32_t Pendiente_Resistencia = 10462;											// Valor de la pendiente
int32_t Offset_Resistencia = 975;												// Valor del offset de la conversión
int32_t Resistencia_Medida;														// Valor calibrado de resistencia

volatile int16_t temperaturaProcesada = 0;										// Valor de temperatura calibrada
volatile int16_t temperaturaProcesada_recibida = 0;								// Valor final de conductividad recibido del Slave

volatile int16_t temperaturaProcesada_prev = 0;									// Valor de temperatura calibrada previo
volatile int16_t temperaturaProcesada_recibida_prev = 0;						// Valor final de conductividad recibido del Slave previo

int32_t Pendiente_Temperatura; //= PENDIENTE_TEMPERATURA;
int32_t Offset_Temperatura;// = OFFSET_TEMPERATURA;

long RLeida = 0;													

/************************************************************************************************************
 * 											Prototipos de funciones											*
 ************************************************************************************************************/

int calculoPendientePT100(void);
int Temperatura(void);
int Calculo_Pendiente_Resistencia(void);
int Resistencia_Calibracion(void);

/************************************************************************************************************
 * 											Implementación de funciones										*
 ************************************************************************************************************/


/************************************************************************************************************
 * 
 *  @brief Se obtiene la pendiente PT100 con los extremos obtenidos por el AD
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ************************************************************************************************************/ 

int calculoPendientePT100(void)
{
	
/************************************************************************************************************
 * 										Calibracion Temperatura												*			
 ************************************************************************************************************
 * 
 * Calculo para la pt100
 * PT100A   -----> Primera medicion
 * PT100B   -----> Segunda medicion
 *
 *
 *               EB0000			--> 235 * 65536
 *  PPT100  = -----------------
 *            (PT100B - PT100A)
 * 
 * 			          320000		-->	50 (fondo de escala)* 65536 = 3276800
 * PPT1000 =	-------------------
 *       		(PT1000B - PT1000A)
 * 
 ************************************************************************************************************/
  
  PPT100 = ConstanteConver / (PT100B - PT100A);
	 
  return 0;
}

/************************************************************************************************************
 * 
 *  @brief Cálculo de temperatura a partir de la medición del ADC
 * 
 * 	@returns 0 funcionamiento correcto
 *
 ************************************************************************************************************/ 

int Temperatura(void)
{
	
	int32_t calculo_parcial = 0;																						// Variable para el cálculo auxiliar
																														// -- Calculo la Pendiente de Temperatura --
	Pendiente_Temperatura   = (int32_t) TEMPERATURA_SPAN;																// Span total de temperatura sin escalar
	Pendiente_Temperatura <<= (int32_t) ESCALA_PENDIENTE_TEMPERATURA;													// Escalo la pendiente
	Pendiente_Temperatura  /= (int32_t) R_TEMPERATURA_SPAN;																// Divido por el span de R sin escalar
	Pendiente_Temperatura  /= ESCALA_DECIMALES_RESISTENCIA;																// La resistencia tiene decimales
																														// -- Calculo el Offset de Temperatura --	
	Offset_Temperatura     = (int32_t) R_TEMPERATURA_MINIMA * Pendiente_Temperatura * ESCALA_DECIMALES_RESISTENCIA;		// Multiplico el valor minimo por la pendiente
	Offset_Temperatura   >>= (int32_t) ESCALA_PENDIENTE_TEMPERATURA;													// Quito la pendiente del cálculo
	Offset_Temperatura     = (int32_t) TEMPERATURA_MINIMA - Offset_Temperatura;											// Le resto el valor anterior a la temperatura mínima
																														// -- Calculo la resistencia --
	Resistencia_Medida	   = (PT100PromedioObtenido);																	// Obtengo el promedio medido
	Resistencia_Medida 	  *= Pendiente_Resistencia;																		// Multiplico por el valor de la pendiente
	Resistencia_Medida   >>= (int32_t) ESCALA_PENDIENTE_RESISTENCIA;													// Le saco la escala a la pendiente
	Resistencia_Medida    += Offset_Resistencia;																		// Le sumo el offset
																														// -- Calculo la Temperatura --
	calculo_parcial 	   = Resistencia_Medida;																		// Obtengo el valor de resistencia calibrado
	calculo_parcial 	  *= Pendiente_Temperatura;																		// Multiplico para convertir a temperatura
	calculo_parcial		 >>= (int32_t)ESCALA_PENDIENTE_TEMPERATURA;														// Quito la escala de la pendiente
	calculo_parcial 	  += Offset_Temperatura;																		// Quito el offset de la conversión
	// calculo_parcial	  	  /= (int32_t) ESCALA_DECIMALES_RESISTENCIA;												// Convierto a unidad de temperatura
	
	if(calculo_parcial < (int32_t)TEMPERATURA_MINIMA) calculo_parcial = (int32_t)TEMPERATURA_MINIMA;					// Saturo por debajo
	if(calculo_parcial > (int32_t)TEMPERATURA_MAXIMA) calculo_parcial = (int32_t)TEMPERATURA_MAXIMA;					// Saturo por arriba
	
	temperaturaProcesada_prev = temperaturaProcesada;																	// Guardo el valor anterior
	temperaturaProcesada = (int16_t) calculo_parcial;																	// Actualizo el valor actual
	
	return 0;
}


/************************************************************************************************************
 * 
 *  @brief 	Actualiza el valor de la pendiente de medición de resistencia
 * 
 * 	@returns 0 funcionamiento correcto
 *
 ************************************************************************************************************/ 

int Calculo_Pendiente_Resistencia(void)
{
	
	int32_t D_ADC;
	int32_t D_Res;
	
	D_ADC = (ADC_R_Alto - ADC_R_Bajo);										// Diferencia entre valores medidos del ADC
	D_Res = (R_Alto - R_Bajo) << ESCALA_PENDIENTE_RESISTENCIA;				// Diferencia entre valores de resistencia ingresados
	
	if(D_Res <= 0)	return 1;												// Para no dividir por cero o un negativo
	
	Pendiente_Resistencia = D_Res / D_ADC;									// Calculo de la pendiente

	Offset_Resistencia  = Pendiente_Resistencia * ADC_R_Bajo ;				// Calculo el offset	
	Offset_Resistencia >>= ESCALA_PENDIENTE_RESISTENCIA;
	Offset_Resistencia = R_Bajo - Offset_Resistencia;
	
	return 0; 
	
}

/************************************************************************************************************
 * 
 *  @brief 	Calculo resistencia a partir de la medición del ADC
 * 
 * 	@returns 0 funcionamiento correcto
 *
 ************************************************************************************************************/ 
 
int Resistencia_Calibracion(void)
{
	
	Resistencia_Medida = (PT100PromedioObtenido);								// Obtengo el promedio medido
	Resistencia_Medida*= Pendiente_Resistencia;									// Multiplico por el valor de la pendiente
	Resistencia_Medida>>= ESCALA_PENDIENTE_RESISTENCIA;							// Le saco la escala a la pendiente
	Resistencia_Medida+= Offset_Resistencia;									// Le sumo el offset

	return 0;	
}

/************************************************************************************************************ 	
 * 											Final del Archivo												*
 ************************************************************************************************************/
