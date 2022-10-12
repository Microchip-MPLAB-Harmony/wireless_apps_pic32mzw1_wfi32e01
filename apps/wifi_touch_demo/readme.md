# Wi-Fi Touch Demo

This example application acts as a TCP Server to which a TCP Client can connect and visualize QT7 Touch Xpro data.

## Description

This application demonstrates how a user can implement a Wi-Fi based Touch application using PIC32MZ-W1's internal hardware CVD Controller and QT7 Xplained Pro self-capacitance touch evaluation kit. This demo will start a TCP server on the device which is running in Soft AP Mode. The Application will come up as a Soft AP and wait for a device to connect. Afterwards the TCP Server will start running on it. A python script to emulate a dummy TCP client is provided which will display real-time sensor operations on QT7 Xpro board.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)

Path of the application within the repository is **apps/wifi\_touch\_demo/firmware** .

To build the application, refer to the following table and open the project using its IDE.

|Project Name|Description|
|------------|-----------|
|PIC32\_WFI32E\_Curiosity\_QT7\_Touch\_Example.X|MPLABX project for PIC32MZ W1 Curiosity Board|
| | |

## Setting up PIC32MZ W1 Curiosity Board

-   Connect a programmer tools such as SNAP, ICD4, etc to ICSP header.

-   On the GPIO Header \(J207\), connect U1RX \(PIN 13\) and U1TX \(PIN 23\) to TX and RX pin of any USB to UART converter

-   QT7 Xpro board is connected to Curiosity Xpro header

-   Laptop / Mobile \(to run the TCP Client on it\)


## Jumper & Resistor Configuration

-   Jumper J211 is connected between pins 2-3 \(closer to xpro header\)

-   Jumper J209 is disconnected

-   Jumpers J301 are disconnected \(to use external programmer tool\)

-   Remove resistor R220 as it is pulling up the pin used by the 3rd slider segment on the QT7


![resized_hardware_setup](images/GUID-4A8C7176-707E-45F7-8206-156ADAC35835-low.png)

## Running the Application

1.  Open the project and launch Harmony3 configurator.

2.  Currently the Wi-Fi Service is configured as AP mode with default values for SSID, Auth type, etc.

    ![resized_configurator](images/GUID-51B03175-CC0A-43E5-A152-BCF801E842EB-low.png)

3.  Currently Net Service is configured to run as TCP Server running on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module as shown below:

    ![resized_netservice_configurator](images/GUID-EDBB5237-689F-4A9A-88CD-526F4E0FCB69-low.png)

4.  Save configurations and generate code via MHC

5.  Build and program the generated code into the hardware using its IDE

6.  Open the Terminal application \(Ex.:Tera term\) on the computer

7.  Connect to the "USB to UART" COM port and configure the serial settings as follows:

    -   Baud : 115200

    -   Data : 8 Bits

    -   Parity : None

    -   Stop : 1 Bit

    -   Flow Control : None

8.  The device shall come up as a Soft AP and then as per the Net Service configuration, the TCP Server shall come up, awaiting a connection from a TCP Client.

9.  Connect a PC to this Soft AP \(using params from step 2\) and run the provided python script. \(tested with Python v3.8 and Windows 10\)

10. Connect the python client giving the server IP \(the IP of the Board\) and the port as 5555. All operations from the QT7 Xpro board will now be visible real-time on the GUI:

    ![resize_sample_demo_run](images/GUID-35894342-1A08-4926-AF74-435F728433B6-low.png)


## Known Issues:

