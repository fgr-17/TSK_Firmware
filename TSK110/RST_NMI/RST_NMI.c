/********************************************************************************************************************
 *	@file RST_NMI.c																									*
 * 																													*
 * 	@brief Manejo de la interrupción de reset																		*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 03/2012																									*
 ********************************************************************************************************************/
 
/******************************************************************************************************************** 	
 * 												Inclusión de archivos												*
 ********************************************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>
#include <RST_NMI.h>

#include <HAL_Board.h>

/******************************************************************************************************************** 	
 * 												Prototipos de Funciones 											*
 ********************************************************************************************************************/

int Configurar_NMI_Ascendente(void);																// Configuro el pin de RST como NMI, ascendente
int Configurar_NMI_Descendente(void);																// Configuro el pin de RST como NMI, Descendente

/******************************************************************************************************************** 	
 * 													Variables globales												*
 ********************************************************************************************************************/

uint8_t NMI_flanco_ascendente = 1;																	// flag para indicar como quedó configurada la NMI

/******************************************************************************************************************** 	
 * 												Implementación de funciones											*
 ********************************************************************************************************************/

/********************************************************************************************************************
 *	@brief 		Configuro el pin de Reset como NMI en forma ascendente												*
 *  																												*
 * 	@params 	none																								*
 *  																												* 
 * 	@returns 	0 -> éxito																							*
 *  																												*
 ********************************************************************************************************************/
 
int Configurar_NMI_Ascendente(void)
{
	SFRRPCR = 0;																					// Reseteo el valor del registro por valor default
	
	SFRIE1 |= NMIIE;
	
	SFRRPCR&= ~SYSNMIIES;																			// Flanco descendente
	SFRRPCR&= ~SYSRSTRE;																			// Deshabilito el pullup/pulldown
	
	SFRRPCR |= SYSNMI;																				// Habilito la función NMI del pin
	NMI_flanco_ascendente = 1;																		// Indico con el flag que está configurada como ascendente
	return 0; 																						// Devuelvo éxito
}

/********************************************************************************************************************
 *	@brief 		Configuro el pin de Reset como NMI en forma descendente												*
 *  																												*
 * 	@params 	none																								*
 *  																												* 
 * 	@returns 	0 -> éxito																							*
 *  																												*
 ********************************************************************************************************************/
 
int Configurar_NMI_Descendente(void)
{
	SFRRPCR = 0;																					// Reseteo el valor del registro por valor default
	
	SFRIE1 |= NMIIE;
	
	SFRRPCR = SYSNMIIES;																			// Flanco descendente
	SFRRPCR&= ~SYSRSTRE;																			// Deshabilito el pullup/pulldown
	
	SFRRPCR |= SYSNMI;																				// Habilito la función NMI del pin
	NMI_flanco_ascendente = 0;																		// Indico con el flag que está configurada como ascendente
	return 0; 																						// Devuelvo éxito
}

/********************************************************************************************************
 *																										*
 *  @brief		ISR de la UART1																			*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/
/*
#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
	
	switch(__even_in_range(SYSUNIV, SYSUNIV_SYSBUSIV))
	{
		case SYSUNIV_NONE:	                           												// SVMLIFG interrupt pending (highest priority)
			break;
		case SYSUNIV_NMIIFG:
			if(NMI_flanco_ascendente)
			{
				__bic_SR_register(GIE);
				P8OUT |= BIT1;
				P8OUT &=~BIT2;
				SFRRPCR = 0;																		// Reseteo el valor del registro por valor default
				SFRRPCR = SYSNMIIES;																// Flanco descendente
				SFRRPCR|= SYSNMI;																	// Habilito la función NMI del pin
				NMI_flanco_ascendente = 0;															// Indico con el flag que está configurada como ascendente
				__bis_SR_register_on_exit(GIE);
			}
			else
			{
				__bic_SR_register(GIE);
				P8OUT |= BIT2;
				P8OUT &=~BIT1;								
				SFRRPCR = 0;																		// Reseteo el valor del registro por valor default
				SFRRPCR&= ~SYSNMIIES;																// Flanco ascendente
				SFRRPCR|= SYSNMI;																	// Habilito la función NMI del pin
				NMI_flanco_ascendente = 1;															// Indico con el flag que está configurada como ascendente
				__bis_SR_register_on_exit(LPM0_bits + GIE);											// Me voy a bajo consumo
			}
			break;
			
		case SYSUNIV_OFIFG:          	                         									// SVMHIFG interrupt pending
			break;
		case SYSUNIV_ACCVIFG:																		// SVSMLDLYIFG interrupt pending
			break;		
		case SYSUNIV_BUSIFG:
			break;
		default: 
			break;  
	}	
	
}
*/


#pragma vector = UNMI_VECTOR
__interrupt void UNMI_ISR(void)
{
    switch (__even_in_range(SYSUNIV, SYSUNIV_BUSIFG))
    {
        case SYSUNIV_NONE:
            __no_operation();
            break;
        case SYSUNIV_NMIIFG:
            __no_operation();
            break;
        case SYSUNIV_OFIFG:
            UCSCTL7 &= ~(DCOFFG + XT1LFOFFG + XT2OFFG); // Clear OSC flaut Flags fault flags
            SFRIFG1 &= ~OFIFG;                          // Clear OFIFG fault flag
            break;
        case SYSUNIV_ACCVIFG:
            __no_operation();
            break;
        case SYSUNIV_BUSIFG:
            // If bus error occurred - the cleaning of flag and re-initializing of USB is required.
            SYSBERRIV = 0;                              // clear bus error flag
            USB_disable();                              // Disable
    }
}

/********************************************************************************************************
 *																										*
 *  @brief		ISR de la UART1																			*
 * 																										*
 * 	@params 	none																					*
 * 																										*
 * 	@returns 	none																					*
 * 																										*
 ********************************************************************************************************/
/*
#pragma vector = RESET_VECTOR
__interrupt void RESET_ISR(void)
{
	uint8_t flag = 0;
	switch(__even_in_range(SYSRSTIV, SYSRSTIV_PMMKEY))
	{
		case SYSRSTIV_NONE:										// No Interrupt pending
			flag = 1;
			break;
		case SYSRSTIV_BOR:										// SYSRSTIV : BOR
			flag = 2;
			break;
		case SYSRSTIV_RSTNMI:							       	// SYSRSTIV : RST/NMI  
			flag = 3;
			break;
		case SYSRSTIV_DOBOR:									// SYSRSTIV : Do BOR  
			flag = 4;
			break;		
		case SYSRSTIV_LPM5WU:									// SYSRSTIV : Port LPM5 Wake Up  
			flag = 5;
			break;
		case SYSRSTIV_SECYV:						       		// SYSRSTIV : Security violation  
			flag = 6;
			break;
		case SYSRSTIV_SVSL:				       					// SYSRSTIV : SVSL  
			flag = 7;
			break;
		case SYSRSTIV_SVSH:										// SYSRSTIV : SVSH  
			flag = 8;
			break;
		case SYSRSTIV_SVML_OVP:									// SYSRSTIV : SVML_OVP  
			flag = 9;	
			break;
		case SYSRSTIV_SVMH_OVP:									// SYSRSTIV : SVMH_OVP  
			flag = 10;
			break;
		case SYSRSTIV_DOPOR:									// SYSRSTIV : Do POR  
			flag = 11;
			break;
		case SYSRSTIV_WDTTO:									// SYSRSTIV : WDT Time out  
			flag = 12;
			break;
		case SYSRSTIV_WDTKEY:									// SYSRSTIV : WDTKEY violation  
			flag = 13;
			break;
		case SYSRSTIV_KEYV:										// SYSRSTIV : Flash Key violation  
			flag = 14;
			break;
		case SYSRSTIV_FLLUL:									// SYSRSTIV : FLL unlock  
			flag = 15;
			break;
		case SYSRSTIV_PERF:										// SYSRSTIV : peripheral/config area fetch  
			flag = 16;
			break;
		case SYSRSTIV_PMMKEY:									// SYSRSTIV : PMMKEY violation  
			flag = 17;
			break;
		default:
			flag = 18;		
			break;		
		
	}
}
*/
/******************************************************************************************************************** 	
 * 													Fin del archivo 												*
 ********************************************************************************************************************/
