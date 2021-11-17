/*******************************************************************************
  MPLAB Harmony WSS system service Header File

  Company:
    Microchip Technology Inc.

  File Name:
    sys_wss.h

  Summary:
    This header file provides prototypes and definitions for the WSS system service.

  Description:
    This header file provides function prototypes and data type definitions for
    the WSS system service. 
 *******************************************************************************/
/*******************************************************************************
Copyright (C) 2021 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _SYS_WSS_H
#define _SYS_WSS_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************



// DOM-IGNORE-BEGIN
#ifdef __cplusplus // Provide C++ Compatibility

extern "C" {
#endif

    
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types and Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************    
    
/* SYS_WSS_KA_TIMER_PERIOD

  Summary:
     - Set to 100

  Remarks:
       This parameter will decide the accuracy of the client time out.
 */
#define SYS_WSS_KA_TIMER_PERIOD  100
    
/* SYS_WSS_CLIENT_KEY_SIZE

  Summary:
     - Array size for holding the client key

  Remarks:
       None.
 */
#define SYS_WSS_CLIENT_KEY_SIZE 50
    
/* SYS_WSS_CLIENT_KEY_SIZE_DECODED

  Summary:
     - The size of the decoded client key defined by the RFC 6455

  Remarks:
       None.
 */    
#define SYS_WSS_CLIENT_KEY_SIZE_DECODED 16

/* SYS_WSS_SERVER_KEY_SIZE

  Summary:
     - Array size for holding the server key

  Remarks:
       None.
 */ 
#define SYS_WSS_SERVER_KEY_SIZE 50

/* SYS_WSS_SHA1_DIGEST_SIZE

  Summary:
     - The predefined sha digest size

  Remarks:
       None.
 */ 
#define SYS_WSS_SHA1_DIGEST_SIZE 20
    
/* SYS_WSS_HTTP_VERSION_X_X

  Summary:
     - Supported HTTP versions

  Remarks:
       None.
 */ 
#define SYS_WSS_HTTP_VERSION_0_9 0x0009
#define SYS_WSS_HTTP_VERSION_1_0 0x0100
#define SYS_WSS_HTTP_VERSION_1_1 0x0101
    
/* SYS_WSS_STATUS_CODE_LEN

  Summary:
     - The size of the status code to be returned in a closing handshake defined by the RFC 6455

  Remarks:
       None.
 */     
#define SYS_WSS_STATUS_CODE_LEN  2   
    
/* SYS_WSS_GUID

  Summary:
     - The unique ID for generating the sever acceptance key defined by the RFC 6455

  Remarks:
       WebSocket GUID as per Section 1.3 of RFC 6455
 */ 
    
#define SYS_WSS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

// *****************************************************************************

/* WSS service return values

  Summary:
 Reporting the success/failure of an API execution.

  Remarks:
    None.
 */

    typedef enum {
        SYS_WSS_SUCCESS = 0,                    //successful execution
        SYS_WSS_FAILURE = 1,                    //Failure in execution
        SYS_WSS_ERROR_INVALID_REQUEST,          //Failure in validation of the client handshake
        SYS_WSS_ERROR_INVALID_KEY,              //Failure in validation of the client handshake
        SYS_WSS_ERROR_INVALID_FRAME,            //Failure in validation of the client request
    } SYS_WSS_RESULT;
    
// *****************************************************************************

/* WSS service states

  Summary:
 States for handling the various requirements of the Web Socket Server implementation like opening handshake,ping-pong,
 data messages, closing handshakes etc.

  Remarks:
    None.
 */
    typedef enum {
        SYS_WSS_STATE_CLOSED=0,                   //Connection closed, the initial state of the service 
        SYS_WSS_STATE_CLOSING,                    //Handles the processing of the client closing handshake        
        SYS_WSS_STATE_CONNECTING,                 //Handles the processing of the client opening handshake
        SYS_WSS_STATE_CONNECTED,                  //State where the data exchanges happen
       
    } SYS_WSS_STATE;
    
// *****************************************************************************

/* WSS service frame types 

  Summary:
    The supported frames (control and data) as defined in RFC 6455 Section 5.

  Remarks:
    None.
 */
    typedef enum {
        SYS_WSS_FRAME_CONTINUATION = 0x00,        //Continuation frame      - Data frame with opcode 0
        SYS_WSS_FRAME_TEXT = 0x01,                //Text data frame         - Data frame with opcode 1
        SYS_WSS_FRAME_BINARY = 0x02,              //Binary data frame       - Data frame with opcode 2
        SYS_WSS_FRAME_CLOSE = 0x08,               //Connection close frame  - Control frame with opcode 8
        SYS_WSS_FRAME_PING = 0x09,                //Ping frame              - Ping frame with opcode 9
        SYS_WSS_FRAME_PONG = 0x0A                 //Pong frame              - Pong frame with opcode 10
    } SYS_WSS_FRAME;
    
// *****************************************************************************

/* WSS service events

  Summary:
    The event to be reported to the application from the WSS service according to the message exchanges
    with the client.

  Remarks:
    None.
 */ 
    typedef enum {
        SYS_WSS_EVENT_ERR=0,                         //Reports any error condition
        //SYS_WSS_EVENT_UP,
        SYS_WSS_EVENT_CLIENT_CONNECTING,             //Reports the connection request from the client
        SYS_WSS_EVENT_CLIENT_CONNECTED,              //Reports connection completed status 
        SYS_WSS_EVENT_CLIENT_BIN_DATA_RX,            //Reports the reception of a binary data
        SYS_WSS_EVENT_CLIENT_TXT_DATA_RX,            //Reports the reception of a text data
        SYS_WSS_EVENT_CLIENT_PING_RX,                //Report the reception of a ping message
        SYS_WSS_EVENT_CLIENT_PONG_RX,                //Reports the reception of a pong message
        SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX,         //Reports the reception of a closing handshake
        SYS_WSS_EVENT_CLIENT_CLOSING,                //Reports the connection closing
        SYS_WSS_EVENT_CLIENT_CLOSED,                 //Reports the connection closure completion
        SYS_WSS_EVENT_ERR_INVALID_FRAME,             //Reports the reception of an invalid frame
      //  SYS_WSS_EVENT_DOWN,                         
        SYS_WSS_EVENT_CLIENT_TIMEOUT,                //Reports the client connection timeout
    } SYS_WSS_EVENTS;

// *****************************************************************************

/* WSS service status codes for closing handshake as per RFC6455

  Summary:
    The status codes to be reported to the clients as per RFC6455 11.7

  Remarks:
    None.
 */ 
    typedef enum {
        SYS_WSS_STATUS_CODE_NORMAL_CLOSURE = 1000,
        SYS_WSS_STATUS_CODE_GOING_AWAY = 1001,
        SYS_WSS_STATUS_CODE_PROTOCOL_ERROR = 1002,
        SYS_WSS_STATUS_CODE_UNSUPPORTED_DATA = 1003,
        SYS_WSS_STATUS_CODE_NO_STATUS_RCVD = 1005, //reserved code, Not to be used in the close frames
        SYS_WSS_STATUS_CODE_ABNORMAL_CLOSURE = 1006, //reserved, Not to be used in the close frames
        SYS_WSS_STATUS_CODE_INVALID_PAYLOAD_DATA = 1007,
        SYS_WSS_STATUS_CODE_POLICY_VIOLATION = 1008,
        SYS_WSS_STATUS_CODE_MESSAGE_TOO_BIG = 1009,
        SYS_WSS_STATUS_CODE_MANDATORY_EXT = 1010,
        SYS_WSS_STATUS_CODE_INTERNAL_ERROR = 1011,
        SYS_WSS_STATUS_CODE_TLS_HANDSHAKE = 1015 // reserved code, Not to be used in the close frames
    } SYS_WSS_STATUS_CODE;
    

    
// *****************************************************************************
/* Function:
    void (*SYS_WSS_CALLBACK)(SYS_WSS_EVENTS event, void *data, int32_t clientIndex, void *cookie)

   Summary:
    Pointer to a WSS system service callback function.

   Description:
    This data type defines a pointer to a WSS service callback function, thus
    defining the function signature.  Callback functions may be registered by
    clients of the WSS service during the initialization.

   Precondition:
    None.

   Parameters:
        event	    - An event (SYS_WSS_EVENTS) for which the callback was called.<br>
        data	    - Data (if any) related to the Event<br>
        cookie      - A context value, returned untouched to the client when the
                      callback occurs.<br>
        clientIndex - The clientID, for identifying the client which caused the event to occur.<br>
				 
   Returns:
    None.

  Example:
    <code>
    void wss_user_callback(SYS_WSS_EVENTS event, void *data, int32_t clientIndex, void *cookie) {

        int i = 0;
        switch (event) {

            case SYS_WSS_EVENT_CLIENT_CONNECTING:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTING\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CONNECTED:
            {

                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTED\r\n",clientIndex);

                break;
            }
            case SYS_WSS_EVENT_CLIENT_BIN_DATA_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_BIN_DATA_RX\r\n",clientIndex);
                for (i = 0; i < ((SYS_WSS_RXDATA*) data)->datalen; i++) {
                    SYS_CONSOLE_PRINT("%X ", ((SYS_WSS_RXDATA*) data)->data[i]);
                }
                SYS_CONSOLE_PRINT("\r\n");
                //echo server.
                SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_TXT_DATA_RX:
            {

                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TXT_DATA_RX\r\n",clientIndex);
                for (i = 0; i < ((SYS_WSS_RXDATA *) data)->datalen; i++) {
                    SYS_CONSOLE_PRINT("%c", ((SYS_WSS_RXDATA*) data)->data[i]);
                }
                SYS_CONSOLE_PRINT("\r\n");
                // Enable the below statement for echo server
               //SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);

                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSING:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSING\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSED:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSED\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_ERR_INVALID_FRAME:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR_INVALID_FRAME\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_PING_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PING_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_PONG_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PONG_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_ERR:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR\r\n",clientIndex);
                break;
            }
    #if 0   
            case SYS_WSS_EVENT_DOWN:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_DOWN\r\n",clientIndex);
                break;
            }
    #endif
            case SYS_WSS_EVENT_CLIENT_TIMEOUT:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TIMEOUT\r\n",clientIndex);
                break;
            }
        }
    }   
    </code>

  Remarks:
    None.
 */
    typedef void (*SYS_WSS_CALLBACK)(SYS_WSS_EVENTS event, void *data, int32_t clientIndex, void *cookie);
// *****************************************************************************

/* System WSS Service Handshake Context

  Summary:
    Used for the storing the information related to the opening handshake.

  Remarks:
    None.
 */
    typedef struct {
        uint16_t http_version;    //HTTP version specified in the client handshake
        uint8_t ws_version;      //Web socket version specified in the client handshake
        bool upgradeWebSocket;   //If upgrade websocket field specified in the client handshake
        bool origin;            //If origin field specified in the client handshake
        bool connectionUpgrade; //If upgrade field present in the client handshake
        bool iskey;             //If websocket key is present in the client handshake
        char clientKey[SYS_WSS_CLIENT_KEY_SIZE + 1]; //Client key
        char serverKey[SYS_WSS_SERVER_KEY_SIZE + 1]; // Server key
    } SYS_WSS_HANDSHAKE_CTXT;
    
// *****************************************************************************

/* System WSS Service port and secure connection config

  Summary:
    Used for the providing the port and secure connection information during WSS service initialization.
    SYS_MODULE_OBJ SYS_WSS_Initialize(SYS_WSS_CONFIG *config, SYS_WSS_CALLBACK callback, void *cookie)

  Remarks:
    None.
 */
    typedef struct {
        unsigned int port;             //port used by the WSS service.
        bool isTLSEnabled;            //TLS enabled?
    } SYS_WSS_CONFIG;

// *****************************************************************************

/* System WSS Service frame header

  Summary:
  The frame header structure as defined by the RFC6455 Section :5.2.  Base Framing Protocol

  Remarks:
    None.
 */
    typedef __PACKED_STRUCT {
        uint8_t opcode : 4;      //byte0, Opcode as defined by RFC6455
        uint8_t resvd : 3;       //Reserved bits as defined by RFC6455
        uint8_t fin : 1;         // fin bit as defined by RFC6455.Indicates that this is the final fragment in a message.0=first frame, 1= final frame
        uint8_t payloadLen : 7;  //byte1, Payload length as defined by the RFC6455.It can be 7 bits, 7+16 bits, or 7+64 bits.
        uint8_t mask : 1;        //Defines whether the "Payload data" is masked. 1=masked,0=not masked
        uint8_t extPayloadLen[]; //byte2,Extended payload length, I can be 16 or 64 bits based on the payloadLen
    } SYS_WSS_FRAME_HEADER;

// *****************************************************************************

/* System WSS Service RX data structure

  Summary:
  The RX data structure used for sharing the received data and length to the application.

  Remarks:
    None.
 */
    typedef struct {
        uint8_t * data;               //Pointer to the rx data buffer
        int64_t datalen;              //length of the received data
    } SYS_WSS_RXDATA;
       
    
// *****************************************************************************
// *****************************************************************************
// Section: Initialization functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
       SYS_MODULE_OBJ SYS_WSS_Initialize()

  Summary:
      Returns SYS_MODULE_OBJ after the successful initialization of data structures of the WSS service.

  Description:
       This function is used for initializing the data structures of the WSS service and is called from within the System Task.
       This function also can be called by the application with required arguments.

  Returns:
       Returns the address of an array of SYS_MODULE_OBJ which will have the initialization data for all clients.        

  Example:
       <code>
        sysWSS_obj = SYS_WSS_Initialize(NULL,NULL,NULL); //Default config, callback and cookie will be taken
       </code>

  Remarks:
                If the Web Socket Server system service is enabled using MHC, then auto generated code will take care of initialization.
 */

    SYS_MODULE_OBJ SYS_WSS_Initialize(SYS_WSS_CONFIG *config, SYS_WSS_CALLBACK callback, void *cookie);
       
    
// *****************************************************************************
// *****************************************************************************
// Section: Initialization functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
       void SYS_WSS_Deinitialize()

  Summary:
      When invoked the API will de-initialize the  WSS service.

  Description:
       This function is used for de-initializing the data structures and state machine of the WSS service.
       This function can be called by the application with WSS service object handle.

  Returns:
       None.

  Example:
       <code>
 *      SYS_WSS_Deinitialize(&sysWSSObj);
       </code>

  Remarks:
      None.
 */

    void SYS_WSS_Deinitialize(SYS_MODULE_OBJ *sysWSSObj);
    
// *****************************************************************************
// *****************************************************************************
// Section: Task and Client - Sever communication  functions
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
    
// *****************************************************************************
/* Function:
    void SYS_WSS_Task(SYS_MODULE_OBJ obj)

  Summary:
       Executes the SYS NET service and handles the WSS service timer processing.

   Description:

  
  Precondition:
       SYS_WSS_Initialize() should have been called before calling this function

  Parameters:
       Application can pass NULL as argument. The service will internally take care about the network handles<br><br>

   Returns:
       Always returns 0.

   Example:
        <code>
                while(1)
                {
                        ...
                        SYS_WSS_Task(NULL);
                        ...
                }
        </code>

 */
    uint8_t SYS_WSS_Task(SYS_MODULE_OBJ object);

    
// *****************************************************************************
/* Function:
   SYS_WSS_RESULT SYS_WSS_register_callback(SYS_WSS_CALLBACK userCallback, void* cookie)

   Summary:
      API for registering the application callback function with the WSS service.

   Description:
     Callback functions may be registered by clients of the WSS service during the initialization using this API.

   Precondition:
     None.

   Parameters:
        userCallback     	    - A function pointer to the application call back API.<br>
        event           	    - Data (if any) related to the Event<br>
        cookie                  - A context value, returned untouched to the client when the
                                  callback occurs.<br>	 
   Returns:
    SYS_WSS_RESULT.

  Example:
    <code>
    void wss_user_callback(SYS_WSS_EVENTS event, void *data, int32_t clientIndex, void *cookie) {

        int i = 0;
        switch (event) {

            case SYS_WSS_EVENT_CLIENT_CONNECTING:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTING\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CONNECTED:
            {

                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CONNECTED\r\n",clientIndex);

                break;
            }
            case SYS_WSS_EVENT_CLIENT_BIN_DATA_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_BIN_DATA_RX\r\n",clientIndex);
                for (i = 0; i < ((SYS_WSS_RXDATA*) data)->datalen; i++) {
                    SYS_CONSOLE_PRINT("%X ", ((SYS_WSS_RXDATA*) data)->data[i]);
                }
                SYS_CONSOLE_PRINT("\r\n");
                //echo server.
                SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_TXT_DATA_RX:
            {

                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TXT_DATA_RX\r\n",clientIndex);
                for (i = 0; i < ((SYS_WSS_RXDATA *) data)->datalen; i++) {
                    SYS_CONSOLE_PRINT("%c", ((SYS_WSS_RXDATA*) data)->data[i]);
                }
                SYS_CONSOLE_PRINT("\r\n");
                // Enable the below statement for echo server
               //SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);

                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSING:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSING\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSED:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSED\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_ERR_INVALID_FRAME:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR_INVALID_FRAME\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_PING_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PING_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_PONG_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_PONG_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_CLOSE_FRAME_RX\r\n",clientIndex);
                break;
            }
            case SYS_WSS_EVENT_ERR:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_ERR\r\n",clientIndex);
                break;
            }
    #if 0   
            case SYS_WSS_EVENT_DOWN:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_DOWN\r\n",clientIndex);
                break;
            }
    #endif
            case SYS_WSS_EVENT_CLIENT_TIMEOUT:
            {
                SYS_CONSOLE_PRINT("wssSysServCallback(%d): SYS_WSS_EVENT_CLIENT_TIMEOUT\r\n",clientIndex);
                break;
            }
        }
    }   
    </code>

  Remarks:
    None.
 */
    SYS_WSS_RESULT SYS_WSS_register_callback(SYS_WSS_CALLBACK userCallback, void* cookie);


// *****************************************************************************
/* Function:
   SYS_WSS_RESULT SYS_WSS_CloseConnection(SYS_WSS_STATUS_CODE code, uint8_t *data, size_t dataLen, int32_t clientIndex)

  Summary:
    Closes the Web Socket connection to the client indicated by the parameter clientIndex.

  Description:
    This API initiate the closing handshake with the status code provided and and terminate  the
    Web Socket connection to the client indicated by the parameter clientIndex.

  Precondition:
    The connection state of the client indicated by the clientIndex (g_wssSrvcObj[clientIndex].wssState) shall be SYS_WSS_STATE_CONNECTED

  Parameters:
    code	    - Status code to be send to the client in the closing handshake(SYS_WSS_STATUS_CODE) .<br>
    *data	    - A pointer to buffer with the data (if any) related to the connection closure<br>
    dataLen     - Length of the data <br>
    clientIndex - The clientID, for identifying the client which caused the event to occur.<br>

  Returns:
    SYS_WSS_RESULT.

  Example:
        <code>
    SYS_WSS_RESULT res;
    res =SYS_WSS_CloseConnection(SYS_WSS_STATUS_CODE_UNSUPPORTED_DATA, NULL, 0, 1);
        </code>

  Remarks:
    None.
 */ 
    SYS_WSS_RESULT SYS_WSS_CloseConnection(SYS_WSS_STATUS_CODE code, uint8_t *data, size_t dataLen, int32_t clientIndex);
    
// *****************************************************************************
/* Function:
   SYS_WSS_RESULT SYS_WSS_sendMessage(bool fin, SYS_WSS_FRAME type, uint8_t *data, size_t dataLen, int32_t clientIndex)

  Summary:
    Sends a message to the client indicated by the parameter clientIndex.

  Description:
    This API sends the data provided in the web socket frame format to the client indicated by the
    parameter clientIndex.

  Precondition:
    The connection state of the client indicated by the clientIndex (g_wssSrvcObj[clientIndex].wssState) shall be SYS_WSS_STATE_CONNECTED

  Parameters:
    fin  	    - Fin value as defined by RFC6455 to indicate if the frame is final or continuation frame. 
                  0 = Indicates more messages to follow, the current frame is a continuation frame
                  1 = Indicates the final frame of the message<br>
    type        - The data type of the frame , TEXT or BINARY
    *data	    - A pointer to buffer with the data to be sent<br>
    dataLen     - Length of the data <br>
    clientIndex - The clientID, for identifying the client which caused the event to occur.<br>

  Returns:
    SYS_WSS_RESULT.

  Example:
        <code>
           SYS_WSS_RESULT res;      
           res =  SYS_WSS_sendMessage(1, SYS_WSS_FRAME_TEXT, ((SYS_WSS_RXDATA*) data)->data, ((SYS_WSS_RXDATA *) data)->datalen, clientIndex);
        </code>

  Remarks:
    None.
 */ 
    SYS_WSS_RESULT SYS_WSS_sendMessage(bool fin, SYS_WSS_FRAME type, uint8_t *data, size_t dataLen, int32_t clientIndex);
        
// *****************************************************************************
/* Function:
    SYS_WSS_RESULT SYS_WSS_PingClient( uint8_t *data, size_t dataLen, int32_t clientIndex)

  Summary:
    Sends a ping message to the client indicated by the parameter clientIndex.

  Description:
    This API sends a ping message to the client indicated by the parameter clientIndex.

  Precondition:
    The connection state of the client indicated by the clientIndex (g_wssSrvcObj[clientIndex].wssState) shall be SYS_WSS_STATE_CONNECTED

  Parameters:
    *data	    - A pointer to buffer with the data (if any).<br>
    dataLen     - Length of the data <br>
    clientIndex - The clientID, for identifying the client which caused the event to occur.<br>

  Returns:
    SYS_WSS_RESULT.

  Example:
        <code>
        SYS_WSS_RESULT res;
        res =SYS_WSS_PingClient(&data, dataLen, 1);
        </code>

  Remarks:
    None.
 */ 
    SYS_WSS_RESULT SYS_WSS_PingClient( uint8_t *data, size_t dataLen, int32_t clientIndex);
#endif /* _SYS_WSS_H */

    //DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END
