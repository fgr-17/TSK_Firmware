
/***************************************************************************//**
 * @file       HAL_Buttons.c
 * @addtogroup HAL_Buttons
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Board.h"
#include "HAL_Buttons.h"
#include "HAL_Cma3000.h"
#include "Invernacion.h"

#define BUTTON_PORT_DIR   PADIR
#define BUTTON_PORT_OUT   PAOUT
#define BUTTON_PORT_SEL   PASEL
#define BUTTON_PORT_REN   PAREN
#define BUTTON_PORT_IE    PAIE
#define BUTTON_PORT_IES   PAIES
#define BUTTON_PORT_IFG   PAIFG
#define BUTTON_PORT_IN    PAIN
/*
#define BUTTON1_PIN       BIT7       //P1.7
#define BUTTON2_PIN       BIT2       //P2.2
#define BUTTON1_IFG       P1IFG      //P1.7
#define BUTTON2_IFG       P2IFG      //P1.7
*/
#define BUTTON1_PIN       BIT0       //P1.7
#define BUTTON2_PIN       BIT1       //P2.2
//#define BUTTON1_IFG       P1IFG      //P1.7		// ESTAN DEFINIDOS PERO NO SE USAN!
//#define BUTTON2_IFG       P1IFG      //P1.7

volatile uint8_t buttonDebounce = 1;
volatile uint16_t buttonsPressed = 0;

uint16_t leerFlag = 0;
uint16_t botonPresionado = 0;
int contador = 0;

// Forward declared functions
void Buttons_startWDT(void);

/***************************************************************************//**
 * @brief  Initialize ports for buttons as active low inputs
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       initialize
 * @return none
 ******************************************************************************/

void Buttons_init(uint16_t buttonsMask)
{
 	__disable_interrupt();

	BUTTON_PORT_OUT |=  buttonsMask;  //buttons are active low
    BUTTON_PORT_REN |=  buttonsMask;  //pullup resistor
    BUTTON_PORT_SEL &= ~buttonsMask;
	
	//	leer manual
	P1IFG = 0x00;
	P2IFG = 0x00;

	__enable_interrupt();	

}

/***************************************************************************//**
 * @brief  Enable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       enable
 * @return none
 ******************************************************************************/

void Buttons_interruptEnable(uint16_t buttonsMask)
{
    BUTTON_PORT_IES &= ~buttonsMask; //select falling edge trigger
    BUTTON_PORT_IFG &= ~buttonsMask; //clear flags
    BUTTON_PORT_IE |= buttonsMask;   //enable interrupts
}

/***************************************************************************//**
 * @brief  Disable button interrupts for selected buttons
 * @param  buttonsMask   Use values defined in HAL_buttons.h for the buttons to
 *                       disable
 * @return none
 ******************************************************************************/

void Buttons_interruptDisable(uint16_t buttonsMask)
{
    BUTTON_PORT_IE &= ~buttonsMask;
}

/***************************************************************************//**
 * @brief  Sets up the WDT as a button debouncer, only activated once a
 *         button interrupt has occurred.
 * @param  none
 * @return none
 ******************************************************************************/

void Buttons_startWDT()
{
    // WDT as 250 ms interval counter
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_5;
    SFRIE1 |= WDTIE;
}

void Buttons_startWDT_estado3()
{
    // WDT as 1,95 ms interval counter
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_7;
    SFRIE1 |= WDTIE;
}

void Buttons_startWDT_15ms()
{
    // WDT as 15,6 ms interval counter
    SFRIFG1 &= ~WDTIFG;
    WDTCTL = WDTPW + WDTSSEL_1 + WDTTMSEL + WDTCNTCL + WDTIS_6;
    SFRIE1 |= WDTIE;
}

/***************************************************************************//**
 * @brief  Handles Watchdog Timer interrupts.
 *
 *         Global variables are used to determine the module triggering the
 *         interrupt, and therefore, how to handle it.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
	// Globally disable interrupts
    __disable_interrupt();
    
    if (buttonDebounce == 2)
    	{
    	contador++;
    	SFRIFG1 &= ~WDTIFG;
        SFRIE1 &= ~WDTIE;
        WDTCTL = WDTPW + WDTHOLD;
    	
    	if(contador < 4)
           Buttons_startWDT_15ms();
					                
		else  
			{Buttons_startWDT_15ms(); buttonDebounce = 3;}

    	}
    
    if (buttonDebounce == 3)
    	{
    	SFRIFG1 &= ~WDTIFG;
        SFRIE1 &= ~WDTIE;
        WDTCTL = WDTPW + WDTHOLD;	
    	
		if(!(BUTTON_PORT_IN & 0x0001) || !(BUTTON_PORT_IN & 0x0002))
			{Buttons_startWDT_estado3(); return;}
    		
		buttonDebounce = 4;
		contador=0;
        Buttons_startWDT();
    	}
        
    if (buttonDebounce == 4)
    	{

        if(contador < 4)
			{        
			SFRIFG1 &= ~WDTIFG;
        	SFRIE1 &= ~WDTIE;
        	WDTCTL = WDTPW + WDTHOLD;
        	contador++;
        	Buttons_startWDT_15ms(); 
        	}
					                
		else
			{  
			contador=0;
	    	SFRIFG1 &= ~WDTIFG;
        	SFRIE1 &= ~WDTIE;
        	WDTCTL = WDTPW + WDTHOLD;

			if(estadoInvernar)
				{
				estadoInvernar = 0;

				//	despertar todo			
				DespertarPerifericos();//(1);
				
				// SET UP again
				buttonDebounce = 1;		
				buttonsPressed = 0;
				
				reiniciarContadores();
				
				//	Limpiar stack\\\\
				//	a las piñas, _STACK_END 0x004400
				// 	Inicializo el stack en el origen 
				asm(" MOV #0x004400, SP");
				
				// Todo de nuevo
				ModoUsuario();
				}

			buttonDebounce = 1;		
			buttonsPressed = botonPresionado;
				
    		}    
    	}    
                    
    // Globally enable interrupts
    __enable_interrupt();       

}

/***************************************************************************//**
 * @brief  Handles Port 1 interrupts - performs button debouncing and registers
 *         button presses.
 * @param  none
 * @return none
 ******************************************************************************/

#pragma vector=PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    // Context save interrupt flag before calling interrupt vector.
    // Reading interrupt vector generator will automatically clear IFG flag
    
    // Globally disable interrupts
    __disable_interrupt();
    
    leerFlag = PAIFG & BUTTON_ALL;

    switch (__even_in_range(P1IV, P1IV_P1IFG7))
    	{
        // Vector  P1IV_NONE:  No Interrupt pending
        case  P1IV_NONE:
            break;

        // Vector  P1IV_P1IFG0:  P1IV P1IFG.0
        case  P1IV_P1IFG0: // Boton 1

          	if (buttonDebounce == 1)
            	{
            	botonPresionado = 1;
            	buttonDebounce = 2;
            	Buttons_startWDT_15ms();
                contador=0;
            	}

            else if (buttonDebounce == 3)
                Buttons_startWDT_estado3();
        
            else if (buttonDebounce == 0)
            	buttonDebounce = 1;	// error de maquina de estado, la reseteo.

            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.1
        case  P1IV_P1IFG1: // Boton 2
        
          	if (buttonDebounce == 1)
            	{
                botonPresionado = 2;
            	buttonDebounce = 2;
            	Buttons_startWDT_15ms();
                contador=0;  		
            	}

            else if (buttonDebounce == 3)
                Buttons_startWDT_estado3();
        
            else if (buttonDebounce == 0)
            	buttonDebounce = 1;	// error de maquina de estado, la reseteo. No deberia pasar.

            break;  

        // Vector  P1IV_P1IFG2:  P1IV P1IFG.2
        case  P1IV_P1IFG2:
            break;

        // Vector  P1IV_P1IFG3:  P1IV P1IFG.3
        case  P1IV_P1IFG3:
            break;

        // Vector  P1IV_P1IFG4:  P1IV P1IFG.4
        case  P1IV_P1IFG4:
            break;

        // Vector  P1IV_P1IFG5:  P1IV P1IFG.5
        case  P1IV_P1IFG5:
            break;

        // Vector  P1IV_P1IFG1:  P1IV P1IFG.6
        case  P1IV_P1IFG6:
            break;

        // Vector  P1IV_P1IFG7:  P1IV P1IFG.7
        case  P1IV_P1IFG7:
            break;

        // Default case
        default:
            break;
    	}
    
    if(estadoInvernar)
    	__bis_SR_register_on_exit(LPM3_bits + GIE);
        
    // Globally enable interrupts
    __enable_interrupt();    
    
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
