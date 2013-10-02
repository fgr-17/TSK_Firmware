/********************************************************************************************************
 * 
 * @file       SDCard.c
 * 
 * @addtogroup SDCard
 * @{
 * 
 ********************************************************************************************************/

#include "SD_Card.h"

char names[NAMES_SIZE];																				// GetDirectory tira en este buffer todos los items.
char output[LINE_SIZE];
char path[PATH_SIZE];

uint8_t pathLengths[PATH_LENGTHS_SIZE];
uint8_t nameLengths[NAME_LENGTHS_SIZE];																// indice de inicio de cada item de "names[...]".

uint8_t numNames = 0;
uint8_t namesIndex = 0;
uint8_t listIndex = 0;
uint8_t selectedIndex = 0;
uint8_t selectedLength = 0;

char buff[255]; 																					// 255

/********************************************************************************************************
 * @brief   Get the directory listing from the SDcard and store it
 * @param   directoryName Name of the directory to open. "" = root directory
 * @return  None
 ********************************************************************************************************/

FRESULT GetDirectory(char* directoryName)
{
    FRESULT rc;                                            												// Result code 
    FATFS fatfs;                                           												// File system object
    DIRS dir;                                               											// Directory object 
    FILINFO fno;                                           												// File information object 

    f_mount(0, &fatfs);                                    												// Register volume work area (never fails) 
    rc = f_opendir(&dir, directoryName);

    ClearTempData();                                       												// make sure everything is clear
    for (;;)
    {
        rc = f_readdir(&dir, &fno);                        												// Read a directory item
        if (rc || !fno.fname[0]) break;                    												// Error or end of dir
        if (fno.fattrib & AM_DIR)                          												// this is a directory
        {
            strcat(names, "<d> ");                         												// add "<d>" to indicate this is a directory
            strcat(names, fno.fname);                      												// add this to our list of names
            nameLengths[numNames] = strlen(fno.fname) + 4; 												// add three because we added "<d>"
            numNames++;
        }
        else                                               												// this is a file
        {
            strcat(names, "    ");                         												// add indentation since not a directory
            strcat(names, fno.fname);                      												// add this to our list of names
            nameLengths[numNames] = strlen(fno.fname) + 4; 												// record the length of this name
            numNames++;
        }
    }
                                                           												// accessing the SD card
    return rc;
}

/********************************************************************************************************
 * @brief   Get the current file and store it
 * @param   None
 * @return  None
 ********************************************************************************************************/

FRESULT GetFile(char* fileName)
{
    FRESULT rc;                                            												// Result code
    FATFS fatfs;                                           												// File system object
    FIL fil;                                               												// File object
    UINT br;

    f_mount(0, &fatfs);                                    												// Register volume work area (never fails)
    rc = f_open(&fil, fileName, FA_READ);

    for (;;)
    {
        rc = f_read(&fil, buff, sizeof(buff), &br);        												// Read a chunk of file
        if (rc || !br) break;                              												// Error or end of file
    }
    if (rc)                                                												// if error
    {
        f_close(&fil);
        return rc;
    }

    rc = f_close(&fil);
    return rc;
}

/********************************************************************************************************
 * @brief   This function will create a new file, writes, and close the file.
 *          This function will overwrite your file.
 * @param   fileName Specify the filename of the file
 * @param   text     Data to write to file
 * @param   size     Data size to be written
 * @return  None
 ********************************************************************************************************/

FRESULT WriteFile(char* fileName, char* text, WORD size)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object
    FIL fil;																							// File object
    UINT bw;

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
    rc = f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);											// Open file
    
    if (rc)
    {
        //die(rc);
    }

    rc = f_write(&fil, text, size, &bw);																// Write to file
    if (rc)
    {
        //die(rc);
    }

    rc = f_close(&fil);																					// Close the file
    if (rc)
    {
        //die(rc);
    }

    return rc;
}

/********************************************************************************************************
 * Funcion:	ExistingFile(char*)
 * 
 * Descripcion: dado el nombre del archivo, verifica su existencia
 * 
 * Autor: RUIZ, Maximiliano	H. (maximiliano.hernan.ruiz@gmail.com)
 * Fecha: 05/2011
 * Neróx
 * 
 ********************************************************************************************************/

FRESULT ExistingFile(char* fileName)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object
	FIL fil;																							// File object

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
    rc = f_open(&fil, fileName, FA_OPEN_EXISTING);														// Open file

    return rc;
}

/********************************************************************************************************
 * Funcion:	FileSize(char*)
 * 
 * Descripcion: dabo el nombre del archivo, devuelve su size
 * 
 * Autor: RUIZ, Maximiliano	H. (maximiliano.hernan.ruiz@gmail.com)
 * Fecha: 05/2011
 * Neróx
 * 
 ********************************************************************************************************/

int FileSize(char* fileName)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object
    FIL fil;																							// File object

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
    rc = f_open(&fil, fileName, FA_OPEN_EXISTING);														// Open file
    
    if (rc)
    {
        //die(rc);
    }
	return fil.fsize;
}

/********************************************************************************************************
 * Funcion:	AppWriteFile(char* fileName, char* text, WORD size)
 * 
 * Descripcion: dabo el nombre del archivo, el string a copiar y su 
 * 				longitud, abre el archivo y lo copia al final del
 * 				mismo, importantisimo para el log
 * 
 * Autor: RUIZ, Maximiliano	H. (maximiliano.hernan.ruiz@gmail.com)
 * Fecha: 05/2011
 * Neróx
 * 
 ********************************************************************************************************/

FRESULT AppWriteFile(char* fileName, char* text, WORD size)
{
    FRESULT rc;																							// Result code				
    FATFS fatfs;																						// File system object
	FIL fil;																							// File object
    //ULONG br; // tenes que leer que da calambre
    UINT br;

	strcpy(buff,"\0");  																				// inicializacion del buffer
    f_mount(0, &fatfs);																					// Register volume work area (never fails)

    rc = f_open(&fil, fileName, FA_READ | FA_WRITE | FA_OPEN_ALWAYS | FA__WRITTEN);						// Open file
    
    if (rc)                                                												// if error
        return FR_INVALID_OBJECT;																		// die(rc);
	
	ULONG pos = 0;

	for (;;)
    {
        rc = f_read(&fil, buff, sizeof(buff), &br);        												// Read a chunk of file 
        if (!br)                               															// Error or end of file 
        	break;
        else
        	pos += br;
    }

    if (rc)                                                												// if error
        return FR_INVALID_OBJECT;																		// die(rc);

	rc = f_lseek (&fil, pos-1);																			// Posiciona a la ultima pos
    if (rc)                                                												// if error
        return FR_INVALID_OBJECT;																		// die(rc);

    rc = f_write(&fil, text, size, &br);																// Write to file
    if (rc)
        return FR_INVALID_OBJECT;																		// die(rc);	

    rc = f_close(&fil);																					// Close the file
    if (rc)
        return FR_INVALID_OBJECT;																		// die(rc);
		
    return rc;
}

/********************************************************************************************************
 * @brief   This function will create a new file, writes, and close the file.
 *          This function will overwrite your file.
 * @param   fileName Specify the filename of the file
 * @param   text     Data to write to file
 * @param   size     Data size to be written
 * @return  None
 ********************************************************************************************************/

FRESULT CreateDirectory(char* DirectoryName)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
	rc = f_mkdir (DirectoryName);																		// Create Directory
    if (rc)
    {
        //die(rc);
    }

    return rc;
}

/********************************************************************************************************
 * @brief   
 * @param   fileName Specify the filename of the file
 * @return  None
 ********************************************************************************************************/

FRESULT ExistingDirectory(char* DirectoryName)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object
    DIRS dir;																							// Directory object   

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
    rc =  f_opendir (&dir, DirectoryName);																// Open Directory	   

    return rc;
}

/********************************************************************************************************
 * @brief   This function will delete your file / directory.
 * @param   fileName Specify the filename of the file
 * @return  None
 ********************************************************************************************************/
/*	Ojo con esta funcion, no verifica la existencia del archivo,
 *  rompe si no existe el archivo 
 *  die(rc) */
 
FRESULT DeleteFileDirectory(char* fileName)
{
    FRESULT rc;																							// Result code
    FATFS fatfs;																						// File system object

    f_mount(0, &fatfs);																					// Register volume work area (never fails)
    rc = f_unlink (fileName);																			// Delete file / directory
    if (rc)
    {
        //die(rc);
    }

    return rc;
}

/********************************************************************************************************
 * @brief   Clears the data structures storing the data read from the card
 * @param   None
 * @return  None
 ********************************************************************************************************/

void ClearTempData(void)
{
    uint8_t i = 0;

    for (i = 0; i < 255; i++)
    {
        buff[i] = 0;
    }
    for (i = 0; i < NAMES_SIZE; i++)
    {
        names[i] = 0;
    }
    for (i = 0; i < 17; i++)
    {
        output[i] = 0;
    }
    for (i = 0; i < NAME_LENGTHS_SIZE; i++)
    {
        nameLengths[i] = 0;
    }
    numNames = 0;
    namesIndex = 0;
    listIndex = 0;
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
