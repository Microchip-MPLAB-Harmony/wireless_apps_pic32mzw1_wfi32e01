---
parent: Harmony 3 Wireless application examples for PIC32MZ W1 family
title: TCP Server
has_children: false
has_toc: false

family: PIC32MZW1
function: TCP Server
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# TCP Server 

This example application acts as a TCP Server to which a TCP Client can connect and exchange data in a non-blocking manner.

## Description

This application demonstrates how a user can start a TCP server on the device which is running in Soft AP Mode. The user would need to configure the Wi-Fi settings for the Soft AP and the port number at which the TCP server would be started. The Application will come up as a Soft AP with the TCP Server running on it. A third-party STA will connect to the Soft AP device. The TCP Client on the third -party STA will then connect to the TCP Server running on the device.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)


Path of the application within the repository is **apps/tcp_server/firmware** .

To build the application, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board |
|||

## Setting up PIC32MZ W1 Curiosity Board

- Connect the Debug USB port on the board to the computer using a micro USB cable
- On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter
- Laptop/ Mobile (to run the TCP Client on it)

## Running the Application

1. Open the project and launch Harmony3 configurator.
![MHC](images/configurator.png)
2.	Currently Net Service is configured to run as TCP Server running on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module as shown below:
![MHC](images/netservice_configurator.png)

3. Currently Net Service is configured to run a TCP Server which awaits connection from a TCP Client on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module configurations as shown below:

4.	Save configurations and generate code via MHC 
5.	Build and program the generated code into the hardware using its IDE
6. Open the Terminal application (Ex.:Tera term) on the computer
7. Connect to the "USB to UART" COM port and configure the serial settings as follows:
    - Baud : 115200
    - Data : 8 Bits
    - Parity : None
    - Stop : 1 Bit
    - Flow Control : None

8.	The device shall come up as SoftAP and then as per the default Net Service configuration, the TCP Server shall come up, awaiting a connection from a TCP Client.

9.	Connect a Laptop to the Soft AP (with ssid DEMO_AP_SOFTAP) running on the DUT
10. Start a TCP Client (python script) on laptop, giving the server IP as the IP of the Board, and the port as 5555:
![System Console](images/system_console.png)
![Console](images/tcp_client_console.png)

