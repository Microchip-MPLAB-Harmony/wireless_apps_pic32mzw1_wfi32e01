# UDP Server

This example application acts as a UDP Server to which a UDP Client can connect and exchange data in a non-blocking manner.

## Description

This application demonstrates how a user can start a UDP server on the device. The user would need to configure the Wi-Fi credentials for the Home AP and the port number at which the UDP server should be started. Any standard UDP client can be used to connect to the device which is operating as a UDP server.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)

Path of the application within the repository is **apps/udp\_server/firmware**

To build the application, refer to the following table and open the project using its IDE.

|Project Name|Description|
|------------|-----------|
|pic32mz\_w1\_curiosity\_freertos.X|MPLABX project for PIC32MZ W1 Curiosity Board|
| | |

## Setting up PIC32MZ W1 Curiosity Board

-   Connect the Debug USB port on the board to the computer using a micro USB cable

-   On the GPIO Header \(J207\), connect U1RX \(PIN 13\) and U1TX \(PIN 23\) to TX and RX pin of any USB to UART converter

-   Laptop/ Mobile \(to run the UDP Client on it\)

-   Home AP \(Wi-Fi Access Point\)


## Running the Application

1.Open the project and launch Harmony3 configurator.

2.Configure Home AP credentials for STA Mode.

![resized_configurator](images/GUID-84C69F97-CBAD-44D4-9E43-DA286FB8C1AE-low.png)

3.Currently Net Service is configured to run as UDP Server running on port 3333. In case the user wants to change this config, please make the changes in the Net Service Module as shown below:

![resized_netservice_configurator](images/GUID-0483C340-B420-4887-927E-92D3BE8B6721-low.png)

4.Save configurations and generate code via MHC

5.Build and program the generated code into the hardware using its IDE

6.Open the Terminal application \(Ex.:Tera term\) on the computer

7.Connect to the "USB to UART" COM port and configure the serial settings as follows:

```
- Baud : 115200
- Data : 8 Bits
- Parity : None
- Stop : 1 Bit
- Flow Control : None
```

8.The Board shall connect to Home AP and then as per the default Net Service configuration, it shall bring up the UDP Server which shall await connection from a UDP Client.

![dut_init_console](images/GUID-4A5471B4-C8F6-4B7A-BD54-3983B051FBCE-low.png)

9.User can run any third party utility or a udp\_client python script to act as UDP Client running on the Laptop.

![system_console](images/GUID-F5611019-3EEB-43DE-8C1E-0E736CBB1177-low.png)

![dut_console](images/GUID-0EF3F96D-3254-46D3-9A44-BBEC4137907E-low.png)

Note: TLS Connection is not supported for UDP Server configuration

