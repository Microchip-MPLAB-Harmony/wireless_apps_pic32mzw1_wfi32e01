// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

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
/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    database_parser.c
    
  Summary:
    Interface for the Bootloader library.

  Description:
    This file contains the interface definition for the OTA library.
 *******************************************************************************/



// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************


#include "definitions.h"
#include "driver/driver_common.h"
#include "osal/osal.h"
#include "../bootloader/csv/csv.h"
#include "ota_database_parser.h"
#include "bootloader.h"
#include <string.h>
#define APP_MOUNT_NAME          "/mnt/myDrive1"
#define SYS_CONSOLE_DEBUG1      printf /*To Do*/

#ifdef SYS_OTA_FS_ENABLED
/*
 ** Get buffer for image data base in the external flash
 */
OTA_DB_BUFFER *OTA_GetDBBuffer() {
    OTA_DB_BUFFER *imageDB = (OTA_DB_BUFFER *) csv_create_buffer();
    return imageDB;
}

/*
 ** Get row nuber of a particular image
 */
int GetImageRow(uint32_t ImgVersion, OTA_DB_BUFFER *imageDB) {
    int selected_row = -1;
    size_t csv_field_read_size = 30;
    char *csv_field_read = malloc(csv_field_read_size + 1);
    if (csv_field_read == NULL)
        return -1;
    uint8_t num_rows = csv_get_height((CSV_BUFFER *) imageDB);
    int r;
    uint32_t ver = 0;
    for (r = 0; r < num_rows; r++) {

        csv_get_field(csv_field_read, csv_field_read_size, (CSV_BUFFER *) imageDB, r, OTA_IMAGE_VERSION);
        ver = (uint8_t) strtol(csv_field_read, NULL, 16);
        if (ver == ImgVersion) {
            selected_row = r;
        }
    }
    return selected_row;
}

/*
 ** Get the database entry into a buffer
 */
int OTAGetDb(OTA_DB_BUFFER *imageDB, char *file_name) {
    return (csv_load((CSV_BUFFER *) imageDB, file_name));
}

/*
 ** Get the database entry into a buffer
 */
int OTASaveDb(OTA_DB_BUFFER *imageDB, char *file_name) {
    return (csv_save(file_name, (CSV_BUFFER *) imageDB));
}

/*
 ** Make a new entry in external flash OTA database
 */
int OTADbNewEntry(char *file_name, OTA_DB_ENTRY *image_data) {

    if (file_name == NULL || image_data == NULL)
        return -1;

    CSV_BUFFER *imageDB = csv_create_buffer();
    if (imageDB == NULL)
        return -1;

    char data_entry[1000];
    int row = 0;
    int status = csv_load(imageDB, file_name);
    if (status == 1)
        row = 0;
    else if (status == 0) {
        row = csv_get_height(imageDB);
        if (image_data->db_full == true) {
            uint8_t total_images = GetTotalImgs(imageDB);
            #ifdef DEBUG
            SYS_CONSOLE_DEBUG1("total_images : %d\n", total_images);
            #endif
            uint32_t ver = 0;
            uint32_t version_l = 255;
            uint8_t i;
            for (i = 0; i < total_images; i++) {
                if (GetFieldValue_32Bit((OTA_DB_BUFFER *)imageDB, OTA_IMAGE_VERSION, i, &ver) != 0) {
                    #ifdef DEBUG
                    SYS_CONSOLE_DEBUG1("Image version field not read properly\n");
                    #endif
                    return SYS_STATUS_ERROR;
                }
                if (version_l > ver) {
                    version_l = ver;
                    row = i;
                }
            }
        }
    } else {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    #ifdef DEBUG
    SYS_CONSOLE_DEBUG1("\n\n row : %d\n\n", row);
    #endif
    char ver[4];
    sprintf(ver, "%d", image_data->version);
    strcpy(data_entry, strrchr(image_data->image_name, '/') + 1);
    if (csv_set_field(imageDB, row, 0, data_entry) != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    if (csv_set_field(imageDB, row, 1, "FE") != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    if (csv_set_field(imageDB, row, 2, ver) != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    if (csv_set_field(imageDB, row, 3, image_data->type) != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    if (csv_set_field(imageDB, row, 4, image_data->digest) != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    if (csv_save(file_name, imageDB) != 0) {
        csv_destroy_buffer(imageDB);
        return -1;
    }
    csv_destroy_buffer(imageDB);
    return 0;
}

/*
 ** Get number of images
 */
uint8_t GetTotalImgs(OTA_DB_BUFFER *imageDB) {
    return (csv_get_height((CSV_BUFFER *) imageDB));
}

/*
 ** Get Image Field value
 */
uint8_t GetFieldValue(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, uint8_t *field_value) {
    uint8_t field_status = 0;
    size_t csv_field_read_size = 30;
    char *csv_field_read = malloc(csv_field_read_size + 1);
    if (csv_get_field(csv_field_read, csv_field_read_size, (CSV_BUFFER *) imageDB, selected_row, (size_t) field) == 0) {
        field_status = 0;
        *field_value = (uint8_t) strtol(csv_field_read, NULL, 16);
        #ifdef DEBUG
        printf("ctx->img.status : %x\n", (uint8_t) strtol(csv_field_read, NULL, 16));
        printf("*field_value : %x\n", *field_value);
        #endif
    } else {
        field_status = 1;
    }
    free(csv_field_read);
    return field_status;
}

/*
 ** Get Image 32 bit Field value 
 */
uint8_t GetFieldValue_32Bit(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, uint32_t *field_value) {
    uint8_t field_status = 0;
    size_t csv_field_read_size = 30;
    char *csv_field_read = OSAL_Malloc(csv_field_read_size + 1);
    if (csv_get_field(csv_field_read, csv_field_read_size, (CSV_BUFFER *) imageDB, selected_row, (size_t) field) == 0) {
        field_status = 0;
        *field_value =  strtol(csv_field_read, NULL, 16);
        #ifdef OTA_DB_DEBUG
        SYS_CONSOLE_PRINT("SYS_OTA_DB : ctx->img.status : %x\r\n", (uint8_t) strtol(csv_field_read, NULL, 16));
        SYS_CONSOLE_PRINT("SYS_OTA_DB : *field_value : %x\r\n", *field_value);
        #endif
    } else {
        field_status = 1;
    }
    OSAL_Free(csv_field_read);
    return field_status;
}

/*
 ** Get Image Field string
 */
uint8_t GetFieldString(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, char *field_value) {
    uint8_t field_status = 0;
    size_t csv_field_read_size = 100;
    char *csv_field_read = malloc(csv_field_read_size + 1);
    if (csv_get_field(csv_field_read, csv_field_read_size, (CSV_BUFFER *) imageDB, selected_row, (size_t) field) == 0) {
        field_status = 0;
        strcpy(field_value, csv_field_read);
        #ifdef DEBUG
        SYS_CONSOLE_DEBUG1("*field_value : %s\n", field_value);
        #endif
    } else {
        field_status = 1;
    }
    free(csv_field_read);
    return field_status;
}

/*
 ** Set Image Field value
 */
uint8_t SetFieldValue(OTA_DB_BUFFER *imageDB, OTA_DB_FIELD_TYPE field, uint8_t selected_row, char *field_value) {
    uint8_t field_status = 0;
    if (csv_set_field((CSV_BUFFER *) imageDB, selected_row, 1, field_value) == 0) {
        field_status = 0;
    } else {
        field_status = 1;
    }
    return field_status;
}

/*
 ** Delete entry from external flash OTA database
 */
int OTADbDeleteEntry(OTA_DB_BUFFER *imageDB, uint8_t selected_row) {
    if (csv_remove_row((CSV_BUFFER *) imageDB, selected_row) == -1)
        return -1;
    return 0;
}
#endif