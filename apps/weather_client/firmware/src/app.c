/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "system/net/sys_net.h"
#include "system/wifiprov/sys_wifiprov_json.h"
// *****************************************************************************
// *****************************************************************************
// Section: Declarations
// *****************************************************************************
// *****************************************************************************
#define SERVER_PREFIX_BUFFER                  "GET /data/2.5/weather?q="
#define SERVER_POST_BUFFER                    "&appid=c592e14137c3471fa9627b44f6649db4&units=metric \r\n\r\n"
/** Weather information provider server(Configured with MHC).*/
#define WEATHER_SERVER_NAME            "api.openweathermap.org"
/** Input City Name. */
#define CITY_NAME                        "New York"//"paris" "London" "Bengaluru" "Ahmedabad" "California" "New York"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
SYS_MODULE_OBJ      g_tcpSrvcHandle = SYS_MODULE_OBJ_INVALID;


// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void TcpClientCallback(uint32_t event, void *data, void* cookie)
{
    switch(event)
    {
        case SYS_NET_EVNT_CONNECTED:
        {
            char networkBuffer[256];
            SYS_CONSOLE_PRINT("TcpClientCallback(): Status UP\r\n");
            memset(networkBuffer, 0, sizeof(networkBuffer));
            sprintf(networkBuffer, "%s%s%s", SERVER_PREFIX_BUFFER, (char *)CITY_NAME, SERVER_POST_BUFFER);
            
            while(SYS_NET_SendMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer, strlen(networkBuffer)) == 0);
            break;
        }

        case SYS_NET_EVNT_DISCONNECTED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): Status DOWN\r\n");
            break;
        }

        case SYS_NET_EVNT_RCVD_DATA:
        {
            char networkBuffer[512];
            memset(networkBuffer, 0, sizeof (networkBuffer));
            
            SYS_NET_RecvMsg(g_tcpSrvcHandle, (uint8_t*) networkBuffer, sizeof (networkBuffer));
            SYS_CONSOLE_PRINT("SERVER Received JSON Data: %s:%zu\r\n",networkBuffer,strlen(networkBuffer));

            struct json_obj root,child,sub ;
            
            if(!json_create(&root,(const char*)networkBuffer,strlen((const char*)networkBuffer)))
            {
                if(!json_find(&root, "name", &child))
                    SYS_CONSOLE_PRINT("City name: %s\r\n",child.value.s);

                if(!json_find(&root, "main", &child))
                    if(!json_find(&child, "temp_max", &sub))
                        SYS_CONSOLE_PRINT("City temp Max: %d: Celsius\r\n",sub.value.b);
            }
            break;
        }

        case SYS_NET_EVNT_SSL_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): SSL Negotiation Failed\r\n");
            break;
        }

        case SYS_NET_EVNT_DNS_RESOLVE_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): DNS Resolution Failed\r\n");
            break;
        }

        case SYS_NET_EVNT_SOCK_OPEN_FAILED:
        {
            SYS_CONSOLE_PRINT("TcpClientCallback(): Socket Open Failed\r\n");
            break;
        }
    }
}


// *****************************************************************************
// *****************************************************************************
// Section: Local data
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
void APP_Initialize ( void )
{
    SYS_CONSOLE_MESSAGE("Weather Client Application\n");
	g_tcpSrvcHandle = SYS_NET_Open(NULL, TcpClientCallback, 0);				
}

/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    SYS_CMD_READY_TO_READ();
	SYS_NET_Task(g_tcpSrvcHandle);
}


/*******************************************************************************
 End of File
 */
