/********************************************************************************************************************
 *	@file dsp.h																										*
 * 																													*
 * 	@brief Header del archivo dsp.h																					*
 * 																													*
 * 	@version 1.0																									*
 * 																													*
 * 	@author ROUX, Federico G. (rouxfederico@gmail.com)																*
 * 																													*
 * 	@date 06/2011																									*
 ********************************************************************************************************************/

#ifndef DSP_H_
#define DSP_H_

/********************************************************************************************************************
 * 											INCLUSI�N DE ARCHIVOS													*
 ********************************************************************************************************************/
 
/********************************************************************************************************************
 * 											DEFINICI�N DE CONSTANTES												*
 ********************************************************************************************************************/

#define ERROR_SGN_LEIDA		0xFFFF

/********************************************************************************************************************
 * 											PROTOTIPO DE FUNCIONES													*
 ********************************************************************************************************************/

extern Sgn_Res Leer_Sgn(Type_Sgn sgn_a_leer, int16_t delay);					// Lee una muestra de una se�al
extern int Incrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);						// Incrementa el �ndice de una se�al en forma circular

extern Sgn_Res Leer_e_Incrementar_Sgn(Type_Sgn* p_sgn_a_leer);					// Lee e incrementa el buffer de una se�al
extern int Decrementar_Indice_Sgn (Type_Sgn* sgn_a_leer);						// Decrementa el �ndice de una se�al en forma circular

extern int Procesar_Sgn_Conductividad(DMA_Type V_Array, DMA_Type I_Array);		// Calcula la medici�n de conductividad

/********************************************************************************************************************
 * 											VARIABLES GLOBALES EXTERNAS												*
 ********************************************************************************************************************/

extern int16_t sgn_senoidal_largo;
extern uint16_t sgn_senoidal_fs;
extern Sgn_Res sgn_senoidal_muestras[];
extern volatile long  conductividadPromedioObtenida;

/********************************************************************************************************************
 * 												FIN DEL ARCHIVO 													*
 ********************************************************************************************************************/

#endif /*DSP_H_*/
