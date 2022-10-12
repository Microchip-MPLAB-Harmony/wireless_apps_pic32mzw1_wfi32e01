# Paho MQTT TLS Client

This example application acts as a Paho MQTT TLS Client to connect to Secured MQTT Broker and publishes and subscribes to data on subtopics.

## Description

This application demonstrates how a user can use the Paho MQTT Client to connect to a Secured MQTT Broker. The application shall come up in AP Provisioing mode, and user will need to configure the Wi-Fi credentials for the Home AP. The default application will try to establish MQTT TLS connection with test.mosquitto.org and publishes message to a topic every 30 sec.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)

Path of the application within the repository is **apps/paho\_mqtt\_tls\_client/firmware** .

To build the application, refer to the following table and open the project using its IDE.

|Project Name|Description|
|------------|-----------|
|pic32mz\_w1\_curiosity\_freertos.X|MPLABX project for PIC32MZ W1 Curiosity Board|
| | |

## Setting up PIC32MZ W1 Curiosity Board

-   Connect the Debug USB port on the board to the computer using a micro USB cable

-   On the GPIO Header \(J207\), connect U1RX \(PIN 13\) and U1TX \(PIN 23\) to TX and RX pin of any USB to UART converter

-   Home AP \(Wi-Fi Access Point with internet connection\)


## Running the Application

1.  Open the project and launch Harmony3 configurator.

2.  Currently MQTT Service is configured to run a MQTT Client in Secured mode to connect to test.mosquitto.org on the mqtt secured port \(8883\). In case the user wants to change this config, please make the changes in the MQTT Service Module configurations as shown below:

    ![resized_mqttservice_configurator](images/GUID-119227DD-E791-4B64-AD76-FDE82E839792-low.png)

3.  Save configurations and generate code via MHC

4.  Build and program the generated code into the hardware using its IDE

5.  Open the Terminal application \(Ex.:Tera term\) on the computer

6.  Connect to the "USB to UART" COM port and configure the serial settings as follows:

    -   Baud : 115200

    -   Data : 8 Bits

    -   Parity : None

    -   Stop : 1 Bit

    -   Flow Control : None

7.  The device will come up in AP Provisioning mode, and wait for Home AP credentials to be configured into it.

8.  Provision the device using the following CLI Command "wifiprov set 0 1 "GEN" 0 1 3 "DEMO\_AP" "password"" for Home AP with SSID DEMO\_AP, and password as 'password'. More details on the CLI command can be found here.

    ![mqtt_client_provision_console](images/GUID-C621CDC4-614F-40CC-B60B-8FBD3624EBE4-low.png)

9.  The Board will connect to Home AP and then as per the default MQTT Service configuration, it shall connect to test.mosquitto.org and publish messages on the topic 'MCHP/Sample/a' periodically every 30 sec. It also subscribes to topic 'MCHP/Sample/b'.

    ![mqtt_client_publish_console](images/GUID-BBD2A23A-003E-4D80-8EB7-BC5E4CCC57DB-low.png)

10. If any third party MQTT Client connects to test.mosquitto.org and subscribes to the topic 'MCHP/Sample/a', it will receive the periodic messages being published by the MQTT Client running on the PIC32MZW1.

    ![resized_third_party_mqtt_client_subscribe](images/GUID-A53FE7C3-8833-4833-9D7E-337FE57EB70C-low.png)

11. In case the Third Party MQTT Client publishes onto the topic 'MCHP/Sample/b', the Paho MQTT Client running on the PIC32MZW1 shall receive the messages being published onto this topic since it has subscribed to it.

    ![resized_third_party_mqtt_client_publish](images/GUID-336F2925-08D9-4740-9A21-2C47ADF16730-low.png)

    ![mqtt_client_subscribe_console](images/GUID-4FC41F37-3E57-425B-A847-FB1DBFC61908-low.png)


Note: The secured tcp connection may require the user to modify WolfSSL component settings in MHC depending on the security settings of the site/ server he is trying to access.

