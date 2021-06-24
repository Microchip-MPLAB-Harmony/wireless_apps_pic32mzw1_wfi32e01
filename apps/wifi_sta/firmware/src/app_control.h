/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_control.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_CONTROL_Initialize" and "APP_CONTROL_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_CONTROL_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_CONTROL_H
#define _APP_CONTROL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"

#define SSID_LENGTH      32
#define PASSWORD_LENGTH  62
#define SSID_LIST_LENGTH 4
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_CONTROL_STATE_INIT=0,
    APP_CONTROL_STATE_SERVICE_TASKS,
} APP_CONTROL_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct __attribute__((packed, aligned(4)))
{
    char ssid[SSID_LENGTH];
    unsigned char ssidLength;
    unsigned char channel;
    unsigned char authMode;
    unsigned char password[PASSWORD_LENGTH+1];
    unsigned char wepIdx;
    unsigned char wepKey[64];
}WLAN_CONFIG_DATA;

typedef struct
{
    /* The application's current state */
    APP_CONTROL_STATES state;
    WLAN_CONFIG_DATA wlanConfig;
    char regDomName[6+1];
    bool wlanConfigChanged;
    bool wlanConfigValid;
    bool regDomChanged;
} APP_CONTROL_DATA;

extern APP_CONTROL_DATA app_controlData;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_CONTROL_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_CONTROL_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_CONTROL_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_CONTROL_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_CONTROL_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_CONTROL_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_CONTROL_Tasks( void );



#endif /* _APP_CONTROL_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

