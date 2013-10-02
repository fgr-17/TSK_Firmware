/********************************************************************************************************************
 *	@file 	dma.h																										*
 * 																													*
 * 	@brief	configuración del hardware del DMA																		*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author	ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

/********************************************************************************************************************
 * 											INCLUSION DE ARCHIVOS													*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include "DMA/dma.h"
#include "PWM/pwm.h"

#include "Medicion_Sensores/dsp/dsp.h"
/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/ 

int Inicializar_DMA (void);
int DMA_Deshabilitar_Canales(void);
int DMA_Habilitar_Canales(void);

/********************************************************************************************************************
 * 											DEFINICIÓN DE VARIABLES													*
 ********************************************************************************************************************/

DMA_Type	DMA_Canal_0;
DMA_Type 	DMA_Canal_1;

uint8_t 	estado_canal_0 = 0;
uint8_t 	estado_canal_1 = 0;

/********************************************************************************************************************
 * 											IMPLEMENTACION DE FUNCIONES												*
 ********************************************************************************************************************/

/********************************************************************************
 *	@brief Inicializa el DMA para la medición de Conductividad					
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************/

int Inicializar_DMA (void)
{
	
	// Configuración general del DMA
	DMACTL0 = DMA0TSEL_24 + DMA1TSEL_24;                       		// ADC12IFGx triggered
	DMACTL4 = DMARMWDIS;                         					// Read-modify-write disable
	
	// Configuración del canal DMA0:	
	
	DMA0CTL &= ~DMAIFG;												// Limpio el bit de IRQ pendiente
	DMA0CTL = DMADT_4 + DMAIE + DMADSTINCR_3;  						// Rpt single tranfer, inc dst
	DMA0SZ = sizeof(DMA_Canal_0.Buffer)/sizeof(short int);			// Cantidad de datos a mover

	__data16_write_addr((unsigned short) &DMA0SA,(unsigned long) &ADC12MEM5);			// Source block address
	__data16_write_addr((unsigned short) &DMA0DA,(unsigned long) DMA_Canal_0.Buffer);	// Destination single address                                               
       
    // Configuración del canal DMA1:
                                            
	DMA1CTL &= ~DMAIFG;												// Limpio el bit de IRQ pendiente
	DMA1CTL = DMADT_4 + DMAIE + DMADSTINCR_3;  						// Rpt single tranfer, inc dst, Habilito Int
	DMA1SZ = sizeof(DMA_Canal_0.Buffer)/sizeof(short int);			// Cantidad de datos a mover

	__data16_write_addr((unsigned short) &DMA1SA,(unsigned long) &ADC12MEM6);			// Source block address
	__data16_write_addr((unsigned short) &DMA1DA,(unsigned long) DMA_Canal_1.Buffer);	// Destination single address  
                                            
	return 0;                                                                                      
}
 
/********************************************************************************
 *	@brief 		Deshabilito las interrupciones de los 2 canales del DMA					
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************/ 
 
int DMA_Deshabilitar_Canales(void)
{
	
	DMA0CTL &= 	~DMAEN;								//	Deshabilito el canal 0 del DMA
	DMA0CTL &= 	~DMAIE;								//	Deshabilito la interrupción generada por el canal 0 del DMA
	DMA1CTL &= 	~DMAEN;								//	Deshabilito el canal 1 del DMA
	DMA1CTL &= 	~DMAIE;								//	Deshabilito la interrupción generada por el canal 1 del DMA
	DMA2CTL &= 	~DMAEN;								//	Deshabilito el canal 2 del DMA
	DMA2CTL &= 	~DMAIE;								//	Deshabilito la interrupción generada por el canal 2 del DMA
	
	estado_canal_0 = 0;
	estado_canal_1 = 0;
	
	return 0;	
} 

/********************************************************************************
 *	@brief 		Habilito las interrupciones de los 2 canales del DMA					
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************/ 
  
int DMA_Habilitar_Canales(void)
{

	DMA0CTL |= DMAEN;								//	Habilito el canal 0 del DMA
	DMA0CTL |= DMAIE;								//	Habilito la interrupción generada por el canal 0 del DMA
	DMA1CTL |= DMAEN;								//	Habilito el canal 1 del DMA
	DMA1CTL |= DMAIE;								//	Habilito la interrupción generada por el canal 1 del DMA
	
	estado_canal_0 = 0;
	estado_canal_1 = 0;
	
	return 0;
	
}

/********************************************************************************
 *	@brief 		IRQ del DMA.				
 * 
 * 	@params 	none
 * 
 * 	@returns 	none
 * 
 ********************************************************************************/ 

#pragma vector = DMA_VECTOR
__interrupt void DMA_ISR(void)
{
	static unsigned int cuenta_dma0 = 0;
	static unsigned int cuenta_dma1 = 0; 
	
  switch(__even_in_range(DMAIV,16))
  {
    case 0: break;																							// NO INTERRUPT
    case 2:                                 																// DMA0IFG = DMA Channel 0
		if(!DMA_Canal_0.primer_medicion)																	// Pregunto si no es la primer pasada del buffer
		{																									// 1) No era la 1er pasada, ya paso el transitorio.
			estado_canal_0 = 1;																				//    Señalizo que se llenó el buffer y está listo para procesar 	
			DMA0CTL &= 	~DMAEN;																				//    Deshabilito la interrupción para procesar
		}																									//
		else																								// 2) Es la 1er pasada, estoy en el transitorio:
		{																									//
			cuenta_dma0++;																					// 	  Incremento la cuenta de ciclos a despreciar
			if(cuenta_dma0 >= CICLOS_CONDUCTIVIDAD_DESPRECIADOS)											// 	  Pregunto si llegué al final
			{																								// 	  Si llegué al final
				DMA_Canal_0.primer_medicion = 0;															// 	  Señalizo que terminó el transitorio 	  
				cuenta_dma0 = 0;																			//	  Reinicio la cuenta para la próxima pasada
				estado_canal_0 = 1; 																		//	  Señalizo que el buffer está listo para procesar
				DMA0CTL &= 	~DMAEN;																			//    Deshabilito la interrupción para procesar	
			}
		}
		break;
      
    case 4:		                          																	// DMA1IFG = DMA Channel 1
		if(!DMA_Canal_1.primer_medicion)																	// Pregunto si no es la primer pasada del buffer
		{																									// 1) No era la 1er pasada, ya paso el transitorio.
			estado_canal_1 = 1;																				//    Señalizo que se llenó el buffer y está listo para procesar 	
			DMA1CTL &= 	~DMAEN;																				//    Deshabilito la interrupción para procesar
		}																									//
		else																								// 2) Es la 1er pasada, estoy en el transitorio:
		{																									//
			cuenta_dma1++;																					// 	  Incremento la cuenta de ciclos a despreciar
			if(cuenta_dma1 >= CICLOS_CONDUCTIVIDAD_DESPRECIADOS)											// 	  Pregunto si llegué al final
			{																								// 	  Si llegué al final
				DMA_Canal_1.primer_medicion = 0;															// 	  Señalizo que terminó el transitorio 	  
				cuenta_dma1 = 0;																			//	  Reinicio la cuenta para la próxima pasada
				estado_canal_1 = 1; 																		//	  Señalizo que el buffer está listo para procesar
				DMA1CTL &= 	~DMAEN;																			//    Deshabilito la interrupción para procesar	
			}
		}
		break;
    	
    case 6: break;                          				// DMA2IFG = DMA Channel 2
    case 8: break;                          				// DMA3IFG = DMA Channel 3
    case 10: break;                         				// DMA4IFG = DMA Channel 4
    case 12: break;                         				// DMA5IFG = DMA Channel 5
    case 14: break;                         				// DMA6IFG = DMA Channel 6
    case 16: break;                         				// DMA7IFG = DMA Channel 7
    default: break;
  }
  return;
  
}

/***********************************************************************************************************************
 *												Final del archivo													   *							
 ***********************************************************************************************************************/
