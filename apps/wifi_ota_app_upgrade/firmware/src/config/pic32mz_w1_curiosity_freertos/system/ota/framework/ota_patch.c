/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ota_patch.c
    
  Summary:
    Interface for patch library.

  Description:
    This file contains the interface definition to access OTA patch library.
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

#include "system_config.h"
#include "system_definitions.h"
#include "osal/osal.h"
#include "system/ota/framework/patch/janpatch.h"
#include "ota_patch.h"

#ifdef SYS_OTA_PATCH_ENABLE

SYS_FS_HANDLE source,patch , target;
static uint8_t patch_progress_status;

void OTA_PatchProgress(uint8_t percentage) {
#ifdef SYS_OTA_DEBUG_ENABLE
    SYS_CONSOLE_PRINT("Ota Patch Progress: %d%%\n\r", percentage);
#endif
    patch_progress_status = percentage;
}

uint8_t OTA_PatchProgressStatus(void) {
    return patch_progress_status;
}

SYS_STATUS OTA_ProcessPatch(OTA_PATCH_PARAMS_t *patch_param) {
    
    source = SYS_FS_FileOpen(patch_param->source_file, (SYS_FS_FILE_OPEN_READ));
#ifdef SYS_OTA_APPDEBUG_ENABLED
    SYS_CONSOLE_PRINT("source : %d\n\r",source);
#endif
    patch  = SYS_FS_FileOpen(patch_param->patch_file, (SYS_FS_FILE_OPEN_READ));
#ifdef SYS_OTA_APPDEBUG_ENABLED
    SYS_CONSOLE_PRINT("patch : %d\n\r",patch);
#endif
    target = SYS_FS_FileOpen(patch_param->target_file, (SYS_FS_FILE_OPEN_WRITE_PLUS));
#ifdef SYS_OTA_APPDEBUG_ENABLED
    SYS_CONSOLE_PRINT("target : %d\n\r",target);
    SYS_CONSOLE_PRINT("%s\n\r",patch_param->source_file);
    SYS_CONSOLE_PRINT("%s\n\r",patch_param->patch_file);
    SYS_CONSOLE_PRINT("%s\n\r",patch_param->target_file);
#endif
    janpatch_ctx ctx = {
                { "", OTA_PATCH_BUFFER_SIZE }, // source buffer
                { "", OTA_PATCH_BUFFER_SIZE }, // diff buffer
                { "", OTA_PATCH_BUFFER_SIZE }, // target buffer
                &SYS_FS_FileRead,
                &SYS_FS_FileWrite,
                &SYS_FS_FileSeek,
                &SYS_FS_FileTell,
                &OTA_PatchProgress
            };
    SYS_CONSOLE_PRINT("OTA Patch In Progress\n\r");
    int jpr = janpatch(ctx, source, patch, target);
    SYS_FS_FileClose(source);
    SYS_FS_FileClose(patch);
    SYS_FS_FileClose(target);
    if (jpr != 0) {
        SYS_CONSOLE_PRINT("Patching failed... (%d)\n", jpr);
        return SYS_STATUS_ERROR;
    }
    else{
        SYS_CONSOLE_PRINT("Patch completed\n\r");
    }
    return SYS_STATUS_READY;
}

#endif


