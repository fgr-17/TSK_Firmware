/*************************************************************************
 * Archivo:	HAL_Wheel.c
 * 
 * Modificacion: Se cambio configuracion del AD, ultimo canal, ADC12MCTL15 			 
 * Autor: RUIZ, Maximiliano	H. (maximiliano.hernan.ruiz@gmail.com)
 * Fecha: 06/2011
 * Neróx
 * 
 *************************************************************************/

/***************************************************************************//**
 * @file       HAL_Wheel.c
 * @addtogroup HAL_Wheel
 * @{
 ******************************************************************************/
#include "msp430.h"
#include "HAL_Wheel.h"

/*
#define WHEEL_PORT_DIR P8DIR
#define WHEEL_PORT_OUT P8OUT
#define WHEEL_ENABLE  BIT0
*/

//////////////////////////////////////////////
// Definición para el PCB MSP43F5529_CPU	//
//////////////////////////////////////////////
#define WHEEL_PORT_DIR	P1DIR
#define WHEEL_PORT_OUT	P1OUT
#define WHEEL_ENABLE	BIT2
#define ADC_INPUT_A8	BIT8
//////////////////////////////////////////////

#define ADC_PORT_SEL	P5SEL
#define ADC_INPUT_A5	BIT5

volatile uint16_t positionData;
volatile uint16_t positionDataOld;

/***************************************************************************//**
 * @brief   Set up the wheel
 * @param   None
 * @return  None
 ******************************************************************************/

void Wheel_init(void)
{
	/* Configuración para la placa de Texas
	ADC12CTL0 &= ~ADC12ENC;				// Deshabilito este bit para poder modificar los registros de control.  	
	
    WHEEL_PORT_DIR |= WHEEL_ENABLE;
    WHEEL_PORT_OUT |= WHEEL_ENABLE;                    			// Enable wheel

    ADC12CTL0 = ADC12SHT02 + ADC12ON;                  			// Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP + ADC12CSTARTADD_15 + ADC12SHS_1;     	// Use sampling timer, start at 15, timer A
    
    ADC12MCTL15 = ADC12INCH_5;                          		// Use A5 (wheel) as input
    
    ADC12CTL0 |= ADC12ENC;                             			// Enable conversions
    ADC_PORT_SEL |= ADC_INPUT_A5;                      			// P6.5 ADC option select (A5)
    */
    
    /* Configuración para el PCB MSP430F5529_CPU */
	ADC12CTL0 &= ~ADC12ENC;										// Deshabilito este bit para poder modificar los registros de control.  	
	ADC12CTL0 = 0x0000;
	ADC12CTL1 = 0x0000;
	
	WHEEL_PORT_OUT |= WHEEL_ENABLE;                    			// Enable wheel
    WHEEL_PORT_DIR |= WHEEL_ENABLE;
    

    ADC12CTL0 = ADC12SHT02 + ADC12ON;                  			// Sampling time, ADC12 on
    ADC12CTL1 = ADC12SHP + ADC12CSTARTADD_15 /*+ ADC12SHS_1*/;     	// Use sampling timer, start at 15, timer A
    
    ADC12MCTL15 = ADC12INCH_8;                          		// Use A5 (wheel) as input
    
    ADC12CTL0 |= ADC12ENC;                             			// Enable conversions
    ADC_PORT_SEL |= ADC_INPUT_A8;                      			// P6.5 ADC option select (A5)

}

/***************************************************************************//**
 * @brief   Determine the wheel's position
 * @param   None
 * @return  Wheel position (0~7)
 ******************************************************************************/

uint8_t Wheel_getPosition(void)
{
    uint8_t position = 0;

    Wheel_getValue();
    //determine which position the wheel is in
    if (positionData > 0x0806)
        position = 7 - (positionData - 0x0806) / 260;  //scale the data for 8 different positions
    else
        position = positionData / 260;

    return position;
}

/***************************************************************************//**
 * @brief   Determine the raw voltage value across the potentiometer
 * @param   None
 * @return  Value
 ******************************************************************************/

uint16_t Wheel_getValue(void)
{
	positionData = 0xFFFF;
    
    //measure ADC value
    ADC12IE = (unsigned int) 0x0001 << 15;			                   // Enable interrupt
    ADC12CTL0 |= ADC12SC;                              		// Start sampling/conversion
   	// __bis_SR_register(/*LPM0_bits + */GIE);              // LPM0, ADC12_ISR will force exit
    
    while(positionData == 0xFFFF);						// Espero a tener un dato válido
    
    ADC12IE = 0x00;                                    // Disable interrupt

    //add hysteresis on wheel to remove fluctuations
    if (positionData > positionDataOld)
        if ((positionData - positionDataOld) > 10)
            positionDataOld = positionData;            //use new data if change is beyond
                                                       // fluctuation threshold
        else
            positionData = positionDataOld;            //use old data if change is not beyond
                                                       // fluctuation threshold
    else
    if ((positionDataOld - positionData) > 10)
        positionDataOld = positionData;                //use new data if change is beyond
                                                       // fluctuation threshold
    else
        positionData = positionDataOld;                //use old data if change is not beyond
                                                       // fluctuation threshold

    return positionData;
}

/***************************************************************************//**
 * @brief   Disable wheel
 * @param   None
 * @return  none
 ******************************************************************************/

void Wheel_disable(void)
{
    WHEEL_PORT_OUT &= ~WHEEL_ENABLE;                   //disable wheel
    ADC12CTL0 &= ~ADC12ENC;                            // Disable conversions
    ADC12CTL0 &= ~ADC12ON;                             // ADC12 off
}

/***************************************************************************//**
 * @brief   Enable wheel
 * @param   None
 * @return  none
 ******************************************************************************/

void Wheel_enable(void)
{
    WHEEL_PORT_OUT |= WHEEL_ENABLE;                    //disable wheel
    ADC12CTL0 |= ADC12ON;                              // ADC12 on
    ADC12CTL0 |= ADC12ENC;                             // Enable conversions
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
