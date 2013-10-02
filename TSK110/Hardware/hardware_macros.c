/********************************************************************************************************************
 *	@file hardware_macros.c																							*
 * 																													*
 * 	@brief Funciones de manejo de ports																				*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 11/2011																									*
 ********************************************************************************************************************/

/******************************************************************************************** 	
 * 									Inclusi�n de archivos									*
 ********************************************************************************************/

#include <msp430f5529.h>
#include <stdint.h>

#include <hardware_macros.h>

/******************************************************************************************** 	
 * 									Prototipos de funciones									*
 ********************************************************************************************/

int Dir_Pin(uint8_t Puerto, uint8_t Bit, uint8_t Dir, uint8_t Funcion);
int Activar_Pin(uint8_t Puerto, uint8_t Bit);
int Pasivar_Pin(uint8_t Puerto, uint8_t Bit);
int Toggle_Pin(uint8_t Puerto, uint8_t Bit);

/********************************************************************************************************************
 * 
 * 	@brief Inicializa un pin elegido con una cierta direcci�n (I/O)
 *
 * 	@params	Puerto	Nro.de puerto a definir
 * 
 * 	@params	Bit		Pin del puerto a definir
 * 
 * 	@params	Dir		Direcci�n del puerto en caso que sea I/O
 * 
 * 	@params	Funcion	En caso de que sea una funci�n particular o como I/O
 *  
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Dir_Pin(uint8_t Puerto, uint8_t Bit, uint8_t Dir, uint8_t Funcion)
{
	switch(Puerto)
	{
		case 1:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P1SEL |= 0x01<<Bit;				// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P1SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin
				
			P1DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P1DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
			break;
			
		case 2:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P2SEL |= 0x01<<Bit;				// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P2SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin			
		
			P2DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P2DIR |= Dir << Bit;				// Selecciono el bit con la dirreci�n elegida
			 
			break;
		case 3:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P3SEL |= 0x01<<Bit;				// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P3SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin				
			
			P3DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P3DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
			break;			
		case 4:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P4SEL |= 0x01 << Bit;			// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P4SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin		
			
			P4DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P4DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
			break;
		case 5:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P5SEL |= 0x01 << Bit;			// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P5SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin
		
			P5DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P5DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
				
			break;
			
		case 6:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P6SEL |= 0x01 << Bit;			// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P6SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin
				
			P6DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P6DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida			
			
			break;
			
		case 7:

			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P7SEL |= 0x01 << Bit;			// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P7SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin
		
			P7DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P7DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
			
			break;

		case 8:
		
			if (Funcion == FUNCION_PERIF)		// Si la funci�n es de perif�rico
				P8SEL |= 0x01 << Bit;			// Pongo el bit en alto para seleccionar el pin
			else if(Funcion == FUNCION_IO)		// Si la funci�n es de entrada/salida digital
				P8SEL &= ~(0x01 << Bit);		// Pongo el bit en alto para seleccionar el pin
					
			P8DIR &= ~(0x01 << Bit);			// Limpio el bit correspondiente a la dirreci�n
			P8DIR |= Dir << Bit; 				// Selecciono el bit con la dirreci�n elegida
			break;
		default:
			return 1;				// Error en el par�metro del timer
	}

	return 0; 		// La funci�n corri� OK!	
}

/********************************************************************************************************************
 * 
 * 	@brief Activa la salida de un pin
 *
 * 	@params	Puerto	Nro.de puerto a definir
 * 
 * 	@params	Bit		Pin del puerto a definir
 *  
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Activar_Pin(uint8_t Puerto, uint8_t Bit)
{
	if(Bit > 8) return 1;												// Los ports tienen 8 bits nomas		
	
	switch(Puerto)
	{		
		case 1:
			P1OUT |= 0x01 << Bit;
			break;
		case 2:
			P2OUT |= 0x01 << Bit;
			break;
		case 3:
			P3OUT |= 0x01 << Bit;
			break;
		case 4:
			P4OUT |= 0x01 << Bit;
			break;
		case 5:
			P5OUT |= 0x01 << Bit;
			break;
		case 6:
			P6OUT |= 0x01 << Bit;
			break;
		case 7:
			P7OUT |= 0x01 << Bit;
			break;
		case 8:
			P8OUT |= 0x01 << Bit;
			break;
		default:
			return 1;													// Mand� cualquier cosa como puerto.
			
	}
	return 0;
}

/********************************************************************************************************************
 * 
 * 	@brief Pasiva la salida de un pin
 *
 * 	@params	Puerto	Nro.de puerto a definir
 * 
 * 	@params	Bit		Pin del puerto a definir
 *  
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Pasivar_Pin(uint8_t Puerto, uint8_t Bit)
{
	if(Bit > 8) return 1;												// Los ports tienen 8 bits nomas		
	
	switch(Puerto)
	{
	case 1:
		P1OUT &= ~(0x01 << Bit);
		break;
	case 2:
		P2OUT &= ~(0x01 << Bit);
		break;
	case 3:
		P3OUT &= ~(0x01 << Bit);
		break;
	case 4:
		P4OUT &= ~(0x01 << Bit);
		break;
	case 5:
		P5OUT &= ~(0x01 << Bit);
		break;
	case 6:
		P6OUT &= ~(0x01 << Bit);
		break;
	case 7:
		P7OUT &= ~(0x01 << Bit);
		break;
	case 8:
		P8OUT &= ~(0x01 << Bit);
		break;				
	default:
		return 1;													// Mand� cualquier cosa como puerto.
			
	}
	return 0;
}

/********************************************************************************************************************
 * 
 * 	@brief Invierte la salida de un pin
 *
 * 	@params	Puerto	Nro.de puerto a definir
 * 
 * 	@params	Bit		Pin del puerto a definir
 *  
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************************/

int Toggle_Pin(uint8_t Puerto, uint8_t Bit)
{
	if(Bit > 8) return 1;												// Los ports tienen 8 bits nomas		
	
	switch(Puerto)
	{
	case 1:
		P1OUT ^= (0x01 << Bit);
		break;
	case 2:
		P2OUT ^= (0x01 << Bit);
		break;
	case 3:
		P3OUT ^= (0x01 << Bit);
		break;
	case 4:
		P4OUT ^= (0x01 << Bit);
		break;
	case 5:
		P5OUT ^= (0x01 << Bit);
		break;
	case 6:
		P6OUT ^= (0x01 << Bit);
		break;
	case 7:
		P7OUT ^= (0x01 << Bit);
		break;	
	case 8:
		P8OUT ^= (0x01 << Bit);
		break;
	default:
		return 1;													// Mand� cualquier cosa como puerto.
			
	}
	return 0;
}

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
