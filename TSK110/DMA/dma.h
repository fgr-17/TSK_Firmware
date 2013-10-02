/********************************************************************************************************************
 *	@file dma.h																										*
 * 																													*
 * 	@brief header del archivo del dma.h																				*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

#ifndef DMA_H_
#define DMA_H_

/********************************************************************************************************************
 * 											DEFINICIÓN DE CONSTANTES												*
 ********************************************************************************************************************/ 

// Cantidad de paquetes de señales
#define LARGO_BUFFER_DMA					256			// El largo del buffer es potencia de dos para dividir más rápido
#define SHIFT_DIV_BUFFER					8
#define CICLOS_CONDUCTIVIDAD_DESPRECIADOS	40

/********************************************************************************************************************
 * 											DECLARACIÓN DE TIPOS													*
 ********************************************************************************************************************/ 

typedef int16_t Sgn_Res;														// Tipo de dato asociado a las señales(resolución)

typedef struct
{
	// Señal a sintetizar
	Sgn_Res*			sgn_muestras;							// Puntero al array de muestras
	int16_t  			sgn_largo;								// Longitud de la señal
	volatile uint16_t  	sgn_indice;								// Indice de recorrido del array de muestras
	uint16_t 			fs;										// Frecuencia de sampleo de la señal															
		
} Type_Sgn;

typedef enum dma_estados {DMA_PROCESAR, DMA_INACTIVO, DMA_ACTIVO} DMA_Estados;
  
typedef struct dma_type
{
	Sgn_Res			Buffer[LARGO_BUFFER_DMA];
	DMA_Estados 	Estado_Canal; 
	uint8_t			primer_medicion;	
} DMA_Type;

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

extern int Inicializar_DMA(void);
extern int DMA_Deshabilitar_Canales(void);
extern int DMA_Habilitar_Canales(void);

/********************************************************************************************************************
 * 											VARIABLES GLOBALES EXTERNAS												*
 ********************************************************************************************************************/

extern DMA_Type	DMA_Canal_0;
extern DMA_Type DMA_Canal_1;

extern uint8_t 	estado_canal_0;
extern uint8_t 	estado_canal_1;


#endif /*DMA_H_*/

/***********************************************************************************************************************
 *												Final del archivo													   *							
 ***********************************************************************************************************************/
