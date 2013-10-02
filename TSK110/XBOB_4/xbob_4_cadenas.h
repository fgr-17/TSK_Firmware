/*******************************************************************************************
 *
 * 	Archivo : xbob_4_cadenas.h
 * 
 * 	Descripción: cadenas de datos para el controlador XBOB-4
 * 
 * 	Autor : ROUX, Federico G. (rouxfederico@gmail.com)
 * 
 * 	NEROX 12/2011
 * 
 *******************************************************************************************/

#ifndef XBOB_4_CADENAS_H_
#define XBOB_4_CADENAS_H_


#define	LEN_XBOB4_CADENA_TEST 	15 
uint8_t cadena_test[LEN_XBOB4_CADENA_TEST] = "Test del TSK100";

#define LEN_XBOB4_CADENA_NEROX 	12
uint8_t cadena_nerox[LEN_XBOB4_CADENA_NEROX] = "NEROX S.R.L.";

#define ALARMA_FILA				"17*"

#define CADENA_COND_LEN 		26
#define CADENA_COND_FILA		"18*"
#define CADENA_COND_COL			"0*"
uint8_t cadena_cond[CADENA_COND_LEN] = "Conductividad:        [uS]";


#define CADENA_TEMP_LEN 		25
#define CADENA_TEMP_FILA		"18*"
#define CADENA_TEMP_COL			"10*"
uint8_t cadena_temp[CADENA_TEMP_LEN] = "Temperatura:          [C]";

#define CADENA_COND2_FILA		"18*"
#define CADENA_COND2_COL		"19*"

#define CADENA_TEMP2_FILA		"18*"
#define CADENA_TEMP2_COL		"27*"

#endif /*XBOB_4_CADENAS_H_*/

/******************************************************************************************** 	
 * 									Final del Archivo										*
 ********************************************************************************************/
