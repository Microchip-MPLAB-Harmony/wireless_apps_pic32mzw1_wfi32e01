---
parent: Harmony 3 Wireless application examples for PIC32MZ W1 family
title: Paho MQTT TLS Client
has_children: false
has_toc: false

family: PIC32MZW1
function: Paho MQTT TLS Client
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# Paho MQTT TLS Client 

This example application acts as a Paho MQTT TLS Client to connect to Secured MQTT Broker and publishes and subscribes to data on subtopics.

## Description

This application demonstrates how a user can use the Paho MQTT Client to connect to a Secured MQTT Broker. The application shall come up in AP Provisioing mode, and user will need to configure the Wi-Fi credentials for the Home AP. The default application will try to establish MQTT TLS connection with test.mosquitto.org and publishes message to a topic every 30 sec.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)


Path of the application within the repository is **apps/paho_mqtt_tls_client/firmware** .

To build the application, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board |
|||

## Setting up PIC32MZ W1 Curiosity Board

- Connect the Debug USB port on the board to the computer using a micro USB cable
- On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter
- Home AP (Wi-Fi Access Point with internet connection)

## Running the Application

1. Open the project and launch Harmony3 configurator.
2. Currently MQTT Service is configured to run a MQTT Client in Secured mode to connect to test.mosquitto.org on the mqtt secured port (8883). In case the user wants to change this config, please make the changes in the MQTT Service Module configurations as shown below:
![MHC](images/mqttservice_configurator.png)

3.	Save configurations and generate code via MHC 
4.	Build and program the generated code into the hardware using its IDE
5. Open the Terminal application (Ex.:Tera term) on the computer
6. Connect to the "USB to UART" COM port and configure the serial settings as follows:
    - Baud : 115200
    - Data : 8 Bits
    - Parity : None
    - Stop : 1 Bit
    - Flow Control : None

7.	The device will come up in AP Provisioning mode, and wait for Home AP credentials to be configured into it.
8.	Provision the device using the following CLI Command "wifiprov set 0 1 "GEN" 0 1 3 "DEMO_AP" "password"" for Home AP with SSID DEMO_AP, and password as 'password'. More details on the CLI command can be found here.
![Console](images/mqtt_client_provision_console.png)

9.	The Board will connect to Home AP and then as per the default MQTT Service configuration, it shall connect to test.mosquitto.org and publish messages on the topic 'MCHP/Sample/a' periodically every 30 sec. It also subscribes to topic 'MCHP/Sample/b'
![Console](images/mqtt_client_publish_console.png)

10.	If any third party MQTT Client connects to test.mosquitto.org and subscribes to the topic 'MCHP/Sample/a', it will receive the periodic messages being published by the MQTT Client running on the PIC32MZW1.
![Third Party MQTT Client](images/third_party_mqtt_client_subscribe.png)

11. In case the Third Party MQTT Client publishes onto the topic 'MCHP/Sample/b', the Paho MQTT Client running on the PIC32MZW1 shall receive the messages being published onto this topic since it has subscribed to it.
![Third Party MQTT Client](images/third_party_mqtt_client_publish.png)
![Console](images/mqtt_client_subscribe_console.png)

Note: The secured tcp connection may require the user to modify WolfSSL component settings in MHC depending on the security settings of the site/ server he is trying to access.
