/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ota_database_parser.h
    
  Summary:
    Interface for the CSV library.

  Description:
    This file contains the interface for the OTA CSV library.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2020-2021 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************
#ifndef _DATABASE_PARSER_H
#define _DATABASE_PARSER_H


#include "system_config.h"
#include "system_definitions.h"
#include "driver/driver_common.h"
#include "osal/osal.h"
#include "system/ota/framework/csv/csv.h"

typedef struct OTA_DB_FIELD {
        char *text;
        size_t length;
} OTA_DB_FIELD;
typedef struct OTA_DB_BUFFER {
        OTA_DB_FIELD ***field;
        size_t rows;
        size_t *width; 
        char field_delim;
        char text_delim;
} OTA_DB_BUFFER;
typedef struct OTA_DB_ENTRY {
        char *image_name;
        char *status;
        uint32_t version;
        char *type;
        char *digest;
        char *digest_sign;
        bool db_full;
} OTA_DB_ENTRY;
typedef enum {
    OTA_IMAGE_NAME = 0,
    OTA_IMAGE_STATUS,
    OTA_IMAGE_VERSION,
    OTA_IMAGE_TYPE,
    OTA_IMAGE_DIGEST
} OTA_DB_FIELD_TYPE;
/*******************************************************************************
  Function:
    void open_data_base ( void )

  Summary:
 Open ota database file

  Description:
 This routine will create a database file 

  Precondition:
    

  Parameters:
    None.

  Returns:
    0: success
    1: error opening database file

  Example:
    <code>
    APP_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */
    
    
// int open_data_base();
/* Function: modify_field
 * -----------------------
 * modify a field text to the string provided. . 
 * 
 * Returns:
 *  0: success
 *  1: error allocating space to the string
 */
 int modify_field(size_t row, size_t entry,char *field);
 
 
 /*
 ** Get buffer for image data base in the external flash
 */
 OTA_DB_BUFFER *OTA_GetDBBuffer();
 
 
 /*
 ** Get row nuber of a particular image
 */
int GetImageRow(uint32_t ImgVersion, OTA_DB_BUFFER *imageDB);


/*
 ** Get the database entry into a buffer
 */
int OTAGetDb(OTA_DB_BUFFER *imageDB, char *file_name);

/*
 ** Get the database entry into a buffer
 */
int OTASaveDb(OTA_DB_BUFFER *imageDB, char *file_name);

/*
 ** Get number of images
 */
uint8_t GetTotalImgs();

/*
 ** Get Image Field value
 */
uint8_t GetFieldValue(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, uint8_t *field_value );

/*
 ** Get Image 32 bit Field value 
 */
uint8_t GetFieldValue_32Bit(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, uint32_t *field_value);

/*
 ** Set Image Field value
 */
uint8_t SetFieldValue(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, char *field_value );

/*
 ** Make a new entry in external flash OTA database
 */
int OTADbNewEntry(char *file_name, OTA_DB_ENTRY *image_data);

/*
 ** Get Image Field string
 */
uint8_t GetFieldString(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, char *field_value);

int OTADbDeleteEntry(OTA_DB_BUFFER *imageDB, uint8_t selected_row) ;



#endif
