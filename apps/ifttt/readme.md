# IFTTT Demo

This example application demosntrates how we can generate a trigger to IFTTT web platform to make it perform a specific action like sending a mail/ sms etc using an applet.

## Description

IFTTT is short for "If This Then That". IFTTT is a third-party web platform which uses applets for performing specific actions when they are triggered via a connection. This application demonstrates how a user can trigger a pre-existing applet. The application shall come up in STA mode, and will establish a TLS connection with maker.ifttt.com. It shall send a trigger every time the button \(SW1\) on the Curiosity Board is pressed.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)

Path of the application within the repository is **apps/ifttt/firmware** .

To build the application, refer to the following table and open the project using its IDE.

|Project Name|Description|
|------------|-----------|
|pic32mz\_w1\_curiosity\_freertos.X|MPLABX project for PIC32MZ W1 Curiosity Board|
| | |

## Creating an Applet on IFTTT web platform

IFTTT is a third-party web platform, and one can create an applet of their own. More details on how one can use IFTTT and create their own applets can be found at [https://help.ifttt.com/hc/en-us/articles/360021401373-Creating-your-own-Applet](https://help.ifttt.com/hc/en-us/articles/360021401373-Creating-your-own-Applet)

More details on how to use a webhook applet in IFTTT can be found in the Appendix of this document.

## Setting up PIC32MZ W1 Curiosity Board

-   Connect the Debug USB port on the board to the computer using a micro USB cable to power on the board.

-   On the GPIO Header \(J207\), connect U1RX \(PIN 13\) and U1TX \(PIN 23\) to TX and RX pin of any USB to UART converter

-   Home AP \(Wi-Fi Access Point with internet connection\)


## Running the Application

1.  Open the project and launch Harmony3 configurator.

2.  Configure home AP credentials for STA Mode.

    ![MHC_SSID_Configuration](images/GUID-75A79677-9EF2-45D6-9C0A-229A66AEC86A-low.png)

3.  Save configurations and generate code via MHC.

4.  Currently application expects the user to create an applet at the IFTTT web platform and configure the relevant details related to the applet in the app.c file. These details will help in creating the trigger which we send to make the applet perform the action.

5.  The user would need to populate the following

    |Parameter Name|Default Value|Description|
    |--------------|-------------|-----------|
    |IFTTT\_EVENT\_TYPE|IFTTT\_EVENT\_BUTTON\_PRESS|The IFTTT applet can be triggered in two ways via this application - by pressing the button on the board \(IFTTT\_EVENT\_BUTTON\_PRESS\), or by running a timer \(IFTTT\_EVENT\_PERIODIC\_TIMER\) and sending the trigger periodically. The default event type is button press to trigger the applet.|
    |IFTTT\_EVENT\_PERIOIDC\_TIMER\_TIMEOUT|100 sec|This parameter is used for setting the timeperiod in sec, after which a periodic trigger will be generated. This is valid only when the IFTTT\_EVENT\_TYPE has been set to IFTTT\_EVENT\_PERIODIC\_TIMER|
    |IFTTT\_EVENT|"YOUR\_EVENT"|This is a mandatory parameter to be filled in by the user, and is the 'event' which triggers the applet. This 'event' was used while creating the applet at the IFTTT web platform.|
    |IFTTT\_KEY|"YOUR\_KEY"|This is a mandatory parameter to be filled in by the user, and is the the 'key' which is generated after the creation of the applet at the IFTTT web platform.|
    |IFTTT\_NUM\_OF\_VALUES|1|The IFTTT applet can use upto 3 values/ data while performing an action. This parameter indicates the number of values which this application needs to send along with the trigger.|
    |IFTTT\_VALUE1|"Value1"|Data Value1 that needs to be sent along with the trigger.|
    |IFTTT\_VALUE2|"Value2"|Data Value2 that needs to be sent along with the trigger.|
    |IFTTT\_VALUE3|"Value3"|Data Value3 that needs to be sent along with the trigger.|
    |IFTTT\_USER\_CALLBACK|NULL|In case users want to process some data and based on the result want to dynamically decide if the trigger needs to be sent or not, they will need to map this parameter to APP\_IFTTT\_Callback\(\) function and implement the action inside this function. This function is expected to return a true if the trigger needs to be sent, else false based on the processing of the data relevant to the user application. For example the user may be implementing a temperature sensor and would want to poll periodically for the temperature but send in a trigger only when the temperature crosses certain threshold. In such a case, implementing this function accordingly can help achive the above objective. Also, there can be requirement that the user needs to also send in the sensed temperature value with the trigger, which can be populated in the 'Value1' and sent.|
    |IFTTT\_USER\_CALLBACK\_CTX|NULL|Callback context/ parameter which will be passed on to APP\_IFTTT\_Callback\(\). This could a structure pointer which contains the data to be processed and decide if the trigger needs to be sent.|
    | | | |

6.  Build and program the generated image into the hardware using the IDE.

7.  Open the terminal application\(Ex:Tera term\) on the computer.

8.  Connect to the COM port related to the external USB-UART connected to the GPIO heade and configure the serial settings as follows:

    -   Baud : 115200

    -   Data : 8 Bits

    -   Parity : None

    -   Stop : 1 Bit

    -   Flow Control : None

9.  The Board will connect to Home AP.

    ![Console_STA_Connected](images/GUID-C311498A-7404-4F67-886A-A38B9AFE8563-low.png)

10. Every time the button/ switch \(SW1\) is pressed

    ![Curiosity_Board](images/GUID-843C5638-0F7B-4920-8FC7-0FE6E96805B9-low.png),

    it shall connect to maker.ifttt.com and send the trigger.

    ![Console_Trigger_Sent](images/GUID-587A8EE4-8BB3-4D5E-A495-2C72FB81742C-low.png)

11. This Trigger will be received by the ifttt server which will invoke the applet. Let us assume the user created an applet using webhooks which shall 'send an email' as an action. The email could look something like this:

    ![Applet_Email](images/GUID-D313EBCD-FAA5-4A48-8352-99D607BBE3B3-low.png)

12. Every time a trigger is being sent by the Curiosity Board, the red LED on the board will be ON, and it will go OFF once the trigger request has been successfully trasmitted to IFTTT server. Any new request while the LED is ON will not be honoured. Hence, it is receommended to press the button for a new request only when the red LED has gone OFF.


## Appendix

### Creating a Webhook applet on IFTTT web platform.

1.  Create an IFTTT account.

2.  Sign in to the account.

3.  Click "Create" on the right side toolbar to create an applet

    ![applet_create](images/GUID-EF14E9DA-E72F-4BF8-896F-F93B2B946B9B-low.png)

4.  Clock on 'Add' and choose "webhooks"

    ![applet_choose_service](images/GUID-39023696-28FC-4C56-8EEB-CD34B5AA7A3D-low.png)

    ![applet_choose_webhooks](images/GUID-BA1CE8F9-5FB8-431B-8401-B64F5CAC54C6-low.png)

5.  Choose a trigger - 'Receive a webrequest'

    ![applet_trigger](images/GUID-D772B504-0BEE-4702-82FE-6DA9D5E76442-low.png)

6.  Fill in the event name and create a 'trigger'.Copy the event and past it to parameter IFTTT\_EVENT in the demo code.

    ![applet_trigger_create](images/GUID-9FF82895-D914-46B6-BB59-500A8C2023D8-low.png)

7.  Add the action - could be sending an 'email'

    ![applet_action_add](images/GUID-578E8CC2-3882-4E42-B235-907DC3892469-low.png)

    ![applet_action_email](images/GUID-6D9D2247-D609-4579-8CF2-DE95F11AC271-low.png)

8.  Set the action fields. In case of 'email' it could be subject and body of the mail.

    ![applet_action_fields](images/GUID-A6572774-7C50-47DB-A0E5-BC57E8FA3612-low.png)

9.  'Finish' the applet.

    ![applet_finish](images/GUID-BB28D174-0DD7-4B70-ABB0-EB0B1C4EB95D-low.png)

10. Goto webhooks settings \(https://ifttt.com/maker\_webhooks/setting\)

    ![applet_webhook_settings](images/GUID-3009DB01-AA1A-4613-A8F6-A4E9B012BCA7-low.png)

11. The string after the https://maker.ifttt.com/use/ in the URL is the 'Key' needed by the Demo Application. Copy the key and pasted it to parameter IFTTT\_KEY in the demo code.


