---
parent: Harmony 3 Wireless application examples for PIC32MZ W1 family
title: OTA Boot Loader
has_children: false
has_toc: false

family: PIC32MZW1
function: OTA Boot Loader
---

[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com)

# OTA Boot Loader 

This example acts as loader of OTA image (downloaded using OTA process) to load it to device memory .

## Description

This bootloader project can be used by user to program an image , which is downloaded through OTA process. It is required to build this project first using its IDE (MPLABX) before building OTA application, as the image of the bootloader will be integrated with the "wifi_ota_app_upgrade" application image. 

During boot-up, bootloader will check if any new valid image available in the external flash. If available, it chooses the newly (latest downloaded) available image in the External flash (sst26vf) and program it to Program-Flash area of the device. Bootloader uses SPI protocol to program the image from the external flash. 

If there is no new downloaded image present in the external flash, bootloader will handover the control to current application present in the program-flash area and application will start executing.


**For more details , Please follow documentation provided in [link](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/tree/master/system/ota/docs)**



## Downloading and Generating Bootloader Code

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)


Path of the project within the repository is **apps/ota_bootloader/firmware**.

To genearte code, refer to the following table and open the project using its IDE.

| Project Name      | Description                                    |
| ----------------- | ---------------------------------------------- |
| ota_bootloader.X | MPLABX project for PIC32MZ W1 Curiosity Board |
|||

## Setting up PIC32MZ W1 Curiosity Board

- Connect the Debug USB port on the board to the computer using a micro USB cable


## Generating BootLoader Code

1.  Open "ota_bootloader" project and launch Harmony3 configurator. Path of the application within the repository is **apps/ota_bootloader/firmware**.
2.  Generate code via MHC.



