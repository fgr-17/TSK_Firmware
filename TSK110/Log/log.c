/********************************************************************************************************
 * 																										*
 * 	@file log.c																							*
 * 																										* 
 *	@brief Logeo del Sistema, log de mediciones, rutinas.  Generacion de tabla de datos.				*
 *		  																								*
 * 	@version 1.0																						*
 * 																										*
 * 	@author RUIZ, Maximiliano H. (maximiliano.hernan.ruiz@gmail.com)									*
 * 																										*
 * 	@date 06/2011																						*
 * 																										*
 ********************************************************************************************************/

/********************************************************************************************************
 * 										Inclusión de archivos											*
 ********************************************************************************************************/

#include <msp430f5529.h>

#include <stdint.h>
#include <string.h>

#include <log.h>	
#include <SD_Card.h>

#include <Conductividad.h>
#include <Temperatura.h>
#include <xbob_4.h>
#include <HAL_RTC.h>
#include <Timer_A2.h>
#include <modos.h>
/********************************************************************************************************
 * 											Prototipos de funcioens										*
 ********************************************************************************************************/

int Log_Datos_Binarios(char* TipoInfo, char* Info); 
int Archivo_Log_Datos_Binario(void);

int ArchivoLog(char* TipoInfo, char* Info);
int DatosFechaHora(void);
int Encabezado(char* pathArchivo);
int EncabezadoTabla(char* pathArchivo);

int Leer_Stream_Archivo(void);
int Init_Leer_Stream_Archivo(void);

char *sput_ip1(int integer, char *string);
char *sput_i(int integer, char *string);
void itoa(int integer, char *string);

/********************************************************************************************************
 * 											Variables globales											*
 ********************************************************************************************************/
 
char bufferArchivo[25] = ""; 	
char hora[3];
char min[3];
char seg[3];
char dia[3];
char mes[3];
char anio[3];

T_Stream_Archivo stream_archivo;
T_Modbus* canal_tx_stream  = &canal_tx_0;

/********************************************************************************************************
 * 										Implementación de funciones										*
 ********************************************************************************************************/

/********************************************************************************************************
 * 
 * 	@brief Log de Datos, se genera la tabla con los valores obtenidos de los sensores.
 * 	       Invoca a ArchivoLog()
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 * 
 ********************************************************************************************************/

#define	INDEX_K1_B0				0
#define	INDEX_K1_B1				1
#define	INDEX_T1_B0				2
#define	INDEX_T1_B1				3
#define	INDEX_K2_B0				4
#define	INDEX_K2_B1				5
#define	INDEX_T2_B0				6
#define	INDEX_T2_B1				7
#define CANT_BYTES_DATOS		(INDEX_T2_B1 + 1)
 
int Archivo_Log_Datos_Binario(void)
{
	T_Bytes_a_Word dato;																					// Uso para separar los bytes de los datos de 16 bits
	char buffer[30];

	strcpy(buffer, "");																						// Vacío el buffer que voy a transmitir
	
	dato.word = conductividadProcesada;																		// Convierto la conductividad del Master		
	buffer[INDEX_K1_B0] = dato.bytes[0];																	// Primer byte del dato K1
	buffer[INDEX_K1_B1] = dato.bytes[1];																	// Segundo byte del dato K1 
		
	dato.word = temperaturaProcesada;		
	buffer[INDEX_T1_B0] = dato.bytes[0];
	buffer[INDEX_T1_B1] = dato.bytes[1];
		
	dato.word = conductividadProcesada_recibida;
	buffer[INDEX_K2_B0] = dato.bytes[0];
	buffer[INDEX_K2_B1] = dato.bytes[1];
		
	dato.word = temperaturaProcesada_recibida;
	buffer[INDEX_T2_B0] = dato.bytes[0];
	buffer[INDEX_T2_B1] = dato.bytes[1];
	
	buffer[INDEX_T2_B1 + 1] = '\0';																			// Le mando el enter al final
	
    return Log_Datos_Binarios(NULL, buffer);
}

/********************************************************************************************************
 * 
 * 	@brief Funcion de log general, ArchivoLog(TipoInfo, breve descipcion).
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 * 
 ********************************************************************************************************/

#define INDEX_HORA				0
#define	INDEX_MIN				1
#define	INDEX_SEG				2
#define CANT_BYTES_HORA			(INDEX_SEG + 1)
 	
int Log_Datos_Binarios(char* TipoInfo, char* Info)
{
	char buffer[200] = "";
		 	
	DatosFechaHora();
	
	if(ExistingDirectory(root))
		CreateDirectory(root);															/*	NO Existe Carpeta, se crea	*/

 	strcpy(bufferArchivo, "");															/*	Path Logger/mes-año	*/
	strcpy(bufferArchivo, root);
 	
	if(ExistingDirectory(bufferArchivo))
		CreateDirectory(bufferArchivo);													/* NO Existe Carpeta, se crea	*/
	
	strcat(bufferArchivo, "/");															/* verificacion de archivo	*/

	strcat(bufferArchivo, dia);
	strcat(bufferArchivo, "-");
	strcat(bufferArchivo, mes);
 	strcat(bufferArchivo, EXTENSION_ARCHIVO_LOG);										
 	
 	if(ExistingFile(bufferArchivo))
	{
		if(Encabezado(bufferArchivo) == -1)												/*	NO Existe Archivo, se crea	*/
			return -1;
			
		if(EncabezadoTabla(bufferArchivo) == -1)										/* ya se verifico arriba, no deberia irse por aca */
			return -1;
	}
			
	buffer[INDEX_HORA] = hour_BCD;
	buffer[INDEX_MIN] = min_BCD;
	buffer[INDEX_SEG] = sec_BCD;
	// buffer[INDEX_SEG + 1] = '\0';
	
   	if(AppWriteFile(bufferArchivo, buffer, CANT_BYTES_HORA + 1) == FR_INVALID_OBJECT)
   		return -1;
   	  		
	// strcpy(buffer,Info);																				//	Informacion
	// strcat(buffer,"\n");

   	if(AppWriteFile(bufferArchivo, Info, CANT_BYTES_DATOS + 1)== FR_INVALID_OBJECT)
   		return -1;

   	return 0;
}

/********************************************************************************************************
 * 
 * 	@brief Funcion de log general, ArchivoLog(TipoInfo, breve descipcion).
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 * 
 ********************************************************************************************************/
 
int ArchivoLog(char* TipoInfo, char* Info)
{
	char buffer[200] = "";
		 	
	DatosFechaHora();
	
	if(ExistingDirectory(root))
		CreateDirectory(root);															/*	NO Existe Carpeta, se crea	*/

 	strcpy(bufferArchivo, "");															/*	Path Logger/mes-año	*/
	strcpy(bufferArchivo, root);
 	
	if(ExistingDirectory(bufferArchivo))
		CreateDirectory(bufferArchivo);													/* NO Existe Carpeta, se crea	*/
	
	strcat(bufferArchivo, "/");															/* verificacion de archivo	*/

	strcat(bufferArchivo, dia);
	strcat(bufferArchivo, "-");
	strcat(bufferArchivo, mes);
 	strcat(bufferArchivo, EXTENSION_ARCHIVO_LOG);										
 	
 	if(ExistingFile(bufferArchivo))
	{
		if(Encabezado(bufferArchivo) == -1)												/*	NO Existe Archivo, se crea	*/
			return -1;
			
		if(EncabezadoTabla(bufferArchivo) == -1)										/* ya se verifico arriba, no deberia irse por aca */
			return -1;
		}
		
	/*	Se añade informacion Todo TABULADO, para poder visualizarlo en forma de tabla */
	/*	
	strcpy(buffer, dia);																
 	strcat(buffer, "-");
	strcat(buffer, mes);
 	strcat(buffer, "-");
	strcat(buffer, anio);
	strcat(buffer,"\t");
	
   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer) + 1) == FR_INVALID_OBJECT)
   		return -1;
 	*/
 	
   	strcpy(buffer, hora);																/*	Se arma la hora		*/		
 	strcat(buffer, ":");
	strcat(buffer, min);
 	strcat(buffer, ":");
	strcat(buffer, seg);   		
	strcat(buffer,"\t");
	
   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer) + 1) == FR_INVALID_OBJECT)
   		return -1;
   	
   	if(TipoInfo!= NULL)																	/*	LOG		*/
	{
    	strcpy(buffer, "[");
		strcat(buffer,TipoInfo);														/*	Tipo de Informacion*/
		strcat(buffer,"]:");

     	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   			return -1;
	}
   		
	strcpy(buffer,Info);																//	Informacion
	strcat(buffer,"\n");

   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   		return -1;

   	return 0;

}

/********************************************************************************************************
 * 
 * 	@brief Log en archivo general, logs relevantes. Ej: reinicio del equipo
 * 
 * 	Ejemplo de Log general:
 * 	ArchivoLogGeneral("ERROR", "No se pudo medir")
 * 	ArchivoLogGeneral("INFO", "Archivo Log inicializado")
 * 	ArchivoLogGeneral("INFO", "Medicion Exitosa")
 * 	Usar un criterio general para determinar el TipoInfo
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 *
 ********************************************************************************************************/

int ArchivoLogGeneral(char* TipoInfo, char* Info)
{

	char buffer[200] = "";
	
	strcpy(bufferArchivo, "");
	strcpy(bufferArchivo, ArchivoGeneral);

	if(ExistingFile(bufferArchivo))
		if(Encabezado(bufferArchivo) == -1)														/*	NO Existe Archivo, se crea	*/
			return -1;
		
	DatosFechaHora();	
	
	strcpy(buffer, "[");																		/*	Se arma la fecha	*/
	strcat(buffer, dia);
 	strcat(buffer, "-");
	strcat(buffer, mes);
 	strcat(buffer, "-");
	strcat(buffer, anio);
	strcat(buffer,"]:");
   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   		return -1;
   	
	strcpy(buffer, "[");																		/*	Se arma la hora		*/
	strcat(buffer, hora);
 	strcat(buffer, ":");
	strcat(buffer, min);
 	strcat(buffer, ":");
	strcat(buffer, seg);   		
	strcat(buffer,"]:");
	
   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   		return -1;
   	
   	if(TipoInfo!= NULL)
	{
    	strcpy(buffer, "[");
		strcat(buffer,TipoInfo);																//	Tipo de Informacion
		strcat(buffer,"]:");
   		if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   			return -1;
	}
   		
   	strcpy(buffer, "[");
	strcat(buffer,Info);																		//	Informacion
	strcat(buffer,"]\n");
	
   	if(AppWriteFile(bufferArchivo, buffer, strlen(buffer)+1)== FR_INVALID_OBJECT)
   		return -1;

	return 0;

}

/********************************************************************************************************
 * 
 * 	@brief Log de Datos, se genera la tabla con los valores obtenidos de los sensores.
 * 	       Invoca a ArchivoLog()
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 * 
 ********************************************************************************************************/

int ArchivoLogDatos(void)
{
	int x;
	char buffer[30];
	
	char cond_A[7];
	char temp_A[7];
	char cond_B[7];
	char temp_B[7];
	
	int index = 0;
	int largo = 0;
		
	strcpy(buffer, "");	
	
	largo = my_itoa(conductividadProcesada, cond_A, (uint8_t) CONDUCTIVIDAD_A_CANTIDAD_DECIMALES);
		
	index = 0;
	
	for(x = 0; x < largo;x++)
	{
		buffer[index] = cond_A[x];
		index++;
	}
	
	buffer[index] = '\t';	
	index++;
	buffer[index] = '\t';	
	index++;
	
	largo = my_itoa(temperaturaProcesada, temp_A, (uint8_t) TEMPERATURA_A_CANTIDAD_DECIMALES);
	
	for(x = 0; x < largo;x++)
	{
		buffer[index] = temp_A[x];
		index++;
	}
	
	buffer[index] = '\t';	
	index++;
	buffer[index] = '\t';	
	index++;
	
	largo = my_itoa(conductividadProcesada_recibida, cond_B, CONDUCTIVIDAD_B_CANTIDAD_DECIMALES);
		
	for(x = 0; x < largo;x++)
	{
		buffer[index] = cond_B[x];
		index++;
	}
	
	buffer[index] = '\t';	
	index++;
	buffer[index] = '\t';	
	index++;
	
	largo = my_itoa(temperaturaProcesada_recibida, temp_B, TEMPERATURA_B_CANTIDAD_DECIMALES);
	
	for(x = 0; x < largo;x++)
	{
		buffer[index] = temp_B[x];
		index++;
	}
	
	buffer[index] = '\0';
	
	
    return ArchivoLog(NULL, buffer);
}

/********************************************************************************************************
 * 
 * 	@brief Llena los vectores con la fecha y hora actual
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************/

int DatosFechaHora(void)
{
 	/*	Datos de Fecha Actual	*/
 	Leer_Registros_RTC(TRUE); 	
 
 	hora[0] = '0' + (RTCHOUR   >> 4);
	hora[1] = '0' + (RTCHOUR   &  0x0F);
	hora[2] = '\0';
	
	min[0] = '0' + (RTCMIN >> 4);
	min[1] = '0' + (RTCMIN &  0x0F);
	min[2] = '\0';

	seg[0] = '0' + (RTCSEC >> 4);
	seg[1] = '0' + (RTCSEC &  0x0F);
	seg[2] = '\0';
	
    dia[0] = '0' + (RTCDAY  >> 4);
    dia[1] = '0' + (RTCDAY  &  0x0F);
 	dia[2] = '\0';   
    
    mes[0] = '0' + (RTCMON >> 4);
    mes[1] = '0' + (RTCMON  &  0x0F);
 	mes[2] = '\0';   
    
    anio[0] = '0' + (RTCYEAR   >> 4);
    anio[1] = '0' + (RTCYEAR   &  0x0F); 	 	
 	anio[2] = '\0';   
 	
 	return 0;
}

/********************************************************************************************************
 * 
 * 	@brief Genera el encabezado del archivo log de datos
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD  	
 * 
 ********************************************************************************************************/

int Encabezado(char* pathArchivo)
{
	char buffer2[5];
	
	// Encabezado
	/*
	strcpy(buffer2, "#####################################################\n");
	strcat(buffer2, "# Automatically generated file. PLEASE do not edit. #\n");
	strcat(buffer2, "#####################################################\n");
   	*/
   	
   	strcpy(buffer2, "");
   	WriteFile(pathArchivo, buffer2, strlen(buffer2)+1);
	//strcpy(buffer2, "                     Nerox - SRL					 \n\n");

   	if(AppWriteFile(pathArchivo, buffer2, strlen(buffer2)+1)== FR_INVALID_OBJECT)
   		{
   		//cartelNoSD();
   		//Dogs102x6_clearScreen();
   		return -1;
   		}
	
	return 0;
}

/********************************************************************************************************
 * 
 * 	@brief Genera el encabezado de la tabla del archivo log de datos
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 * 	@returns -1 No se encontro SD
 * 
 ********************************************************************************************************/

int EncabezadoTabla(char* pathArchivo)
{
	char buffer[5];

	/*
	strcpy(buffer,  "[Visualizar en una hoja de cálculo]\n\n");	
	strcat(buffer, "Dia\t\t\tHora\t\tCond.A\tTemp.A\tCond.B\tTemp.B\n");
	strcat(buffer, "   \t\t\t    \t\t [uS] \t [°C] \t [uS] \t [°C] \n\n");
	*/
	
	strcpy(buffer, "");
		
   	if(AppWriteFile(pathArchivo, buffer, strlen(buffer) + 1) == FR_INVALID_OBJECT)
   		return -1;

	return 0;
}

/********************************************************************************************************
 * 
 * 	@brief Inicializa la transmisión del stream de archivos
 * 
 * 	@returns 0 funcionamiento correcto 
 * 
 ********************************************************************************************************/

int Init_Leer_Stream_Archivo(void)
{
	
    f_mount(0, &(stream_archivo.fatfs));																	// Monto el dispositivo en memoria
    (stream_archivo.rc) = f_open(&(stream_archivo.fil), (stream_archivo.fileName), FA_READ);				// Abro el archivo buscado

    if(stream_archivo.rc)																					// Chequeo el resultado de abrir el archivo
        return 1;																							// No se pudo abrir el archivo 

	return 0;																								// Salio todo bien
}

/********************************************************************************************************
 * 
 * 	@brief máquina de estados que manda streaming de un archivo
 * 
 * 	@returns 0 funcionamiento correcto 
 * 
 ********************************************************************************************************/

int Leer_Stream_Archivo(void)
{
	UINT bytes_leidos;																						// UINT porque la función f_read lo pide
	UINT bytes_a_leer;
	uint8_t* ptr_buffer;
	uint8_t i;
	
	if(canal_tx_stream->estado_buffer == BUFFER_VACIO)														// Si el buffer está disponible para enviar
	{
		Timer_A2_Delay(1000, BLOQUEANTE);																	// Espero para no saturar la entrada de la PC
		if(f_eof(&(stream_archivo.fil)))																	// Pregunto si llegué al final del archivo
		{
			return 2;																						// respondo con #2 para indicar el final 
		}
		else
		{
			ptr_buffer = canal_tx_stream->frame + CANT_BYTES_TAG;											// apunto a donde comienza la cadena a escribir
			bytes_a_leer = BYTES_STREAM_ARCHIVO;											// cantidad de bytes que voy a leer
			canal_tx_stream->frame [0] = 'a';																// tag para indicar que son datos de un archivo
			f_read(&(stream_archivo.fil), ptr_buffer, bytes_a_leer, &bytes_leidos);							// copio un pedazo de archivo al buffer de salida
		
			if(bytes_leidos < bytes_a_leer)
				for(i = bytes_leidos; i < bytes_a_leer; i++)
					canal_tx_stream->frame[i + CANT_BYTES_TAG] = '\0';
								
			return 0;																						// respondo con #0 para indicar que lei un pedazo
		}
	}
	else																									// El buffer estaba ocupado
	{	
		return 1;																							// La función no hizo nada
	}
}

/********************************************************************************************************
 * 
 * 	@brief convierte un entero en una cadena.
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************/
 
void itoa(int integer, char *string)
{
	if (0 > integer) {
	++integer;
	*string++ = '-';
	*sput_ip1(-integer, string) = '\0';
	} else {
	*sput_i(integer, string) = '\0';
	}
}

/********************************************************************************************************
 * 
 * 	@brief Auxiliar de itoa()
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************/

char *sput_i(int integer, char *string)
{
	if (integer / 10 != 0) {
	string = sput_i(integer / 10, string);
	}
	*string++ = (char)('0' + integer % 10);
	return string;
}

/********************************************************************************************************
 * 
 * 	@brief Auxiliar de itoa()
 * 
 * 	@returns 0 funcionamiento correcto
 * 
 ********************************************************************************************************/

char *sput_ip1(int integer, char *string)
{
	int digit;

	digit = (integer % 10 + 1) % 10;
	if (integer / 10 != 0) {
	string = (digit == 0 ? sput_ip1 : sput_i)(integer / 10, string);
	*string++ = (char)('0' + digit);
	} else {
	if (digit == 0) {
	*string++ = '1';
	}
	*string++ = (char)('0' + digit);
	}
	return string;
}

/******************************************************************************************************** 	
 * 											Final del Archivo											*
 ********************************************************************************************************/
