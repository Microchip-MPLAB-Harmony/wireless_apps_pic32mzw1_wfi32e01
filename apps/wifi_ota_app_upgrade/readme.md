# Over The Air \(OTA\) Programming Using Wi-Fi

This example application acts as a Wi-Fi Station\(STA\) to connect to Access point\(AP\) and perform OTA application update process to download an image present in the user defined OTA-HTTP server.

## Description

This application demonstrates how a user can perform OTA application upgrade using Wi-Fi. The user would need to configure the Home AP credentials \(like SSID and security items\). The Wi-Fi service will use the credentials to connect to the Home AP and acquire an IP address. Once the IP address is obtained application will perform OTA update process.

![resized_wifi_sta_http_server_1](images/GUID-14DE7F63-D45F-4C34-81F7-8D15A8FBAA8A-low.png)

Application will try to connect with the defined server address and download the new image. The downloaded image will be stored in the external flash \(sst26vf\) initially. Application will reset the device to run new image once image is successfully downloaded from server. When device is reset ,bootloader will try to program the image from the external flash and if programming is successful ,downloaded image from the server will be executed .

It is required to build "ota\_bootloader" project located in the apps folder of `wireless_apps_pic32mzw1_wfi32e01` repo first before building this application as the image of the bootloader application will be used to integrate with the "wifi\_ota\_app\_upgrade" application image. A unified hex file will be built using Hexmate tool and the unified HEX image will be loaded to the device. More details about this can be found in "Running Application" section below.

This application uses File System, a harmony component which internally use SPI protocol to place the newly downloaded image to the external flash.

![spi_com](images/GUID-F6229610-6517-403F-B091-530CBC5DDBC9-low.png)

## OTA Application framework Architecture

Over the Air \(OTA\) firmware upgrade feature is designed with a two step process, Image Downloading and Image Programming process. Image Downloading process is done by `OTA SERVICE`, harmony componet and `ota_bootloader` will take care of programming process.

Abstraction model:

![abstraction_model](images/GUID-562A7C18-F3CA-4203-8DDB-E9ABECFECB5D-low.png)

**User Application:** This is where the customer application logic is built.

**OTA Service:** This layer includes the service level logic implementation. This is a Harmony component which provides certain user configurable parameters\(ex- Version, Periodic update check etc.\) . Based on user configuration, generated code will be activated with required functionalities.

![ota_service_component](images/GUID-86595BF4-AF05-4154-8DC1-DBACC49E6076-low.png)

![ota_service_conf](images/GUID-DDF7B039-BA19-4A78-99C2-B04A476E97D7-low.png)

**OTA software platform / OTA Core :** This is the platform layer that consist of the main OTA logic implementation. When OTA process is triggered , this layer will communicate with the transport layer to connect to OTA server. If new image is available , it will initiate download using transport layer. If successfully downloaded, it will store the new image into the File System .

**File System :** The architecture is designed to provide flexibility for the customer to choose the storage medium \(ex- SST26 SPI flash, SD card, USB MSD in host mode etc.\). Any medium supported by the Harmony3 file system can be used with the OTA service.

**Bootloader :** This layer consists of the logic to safely program images from the file system \(external\) into program the program memory \(NVM\) of the device. At device boot, the bootloader will check if a new image is available in the external image store and transfer it to the NVM.

## Securing OTA by verifying Images in Bootloader

We have implemented 2 configurations of OTA Bootloader to perform signature verification of images:

1.  In software using Wolfcrypt \(ota\_booloader\_wolfcrypt.x\).

2.  In Hardware using Trust Flex device \(ota\_booloader\_trustflex.x\).


By default, application is configured for `ota_booloader_wolfcrypt.x`, in which signature verification will be done using `wolfcrypt` library.

User need to load the corresponding bootloader configuration as `loadable` component as per application requirement.Loading bootloader into application is required , only for generating `factory` image. For more information on configuring `loadable project` , please refer `OTA System Service Configuration` section of `configuration.md`.

For more details , Please follow documentation provided for **Over The Air \(OTA\) firmware update System Service**.

## OTA server JSON manifest

The Application expects the HTTP based OTA server to provide metadata of images available in the server in `json` format. During update checks, the OTA service will download and parse this manifest file. Each entry in the manifest file should include the following fields :

-   **`Version`** indicates the application version number. It is a integer value.

-   **`URL`** contains the image path from which the application image can be downloaded. It is a string variable.

-   **`Digest`** contain the `SHA256` digest of image to be downloaded. It is a 64 byte string variable an should not include whitespaces

-   **`Signature`** This ***optional*** field provides a capability to verify image. Signature of image will be provided by user, which will be used by bootloader to perform signature verification of image.

    If this field is missing from Manifest file, application will not store any `Signature` in OTA database. But it is user responsibilty to undefine macro `SYS_OTA_SECURE_BOOT_ENABLED` in `bootloader` project to disable `Signature` verfication of images. In this case, only `Digest` verification will be done by bootloader.

    Please follow `Using bootloader for Non-secure OTA` section of [boolader](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/blob/master/apps/ota_bootloader/readme.md) manual.

-   **`EraseVer`** This ***optional*** field provides a capability to trigger an erase of an version which was downloaded earlier. Customer may want to remove an image from the image store due to various reason, application with bug, may be one of them. It is a bool variable.

    -   If user configures this field as "true", OTA serice will delete image version mentioned in "Version" field.

    -   If user configures this field as "false", OTA service will follow image downwload logic.<br />**Sample JSON**


```json
    {
    
    "ota": [
            {
                "Version": 3,
                "URL": "http://192.168.43.173:8000/wifi_ota103.bin",
                "Digest": "745189cbb24b752a0175de1f9d5d61433ba47d89aff5b5a3686f54ca2d5dfb22",
                "Signature": "pra2wlOiZO/zjzqaP9DZGe9dmm0aC4gx4r0yoyI7DU3sVpkdJ034v5XoiN5jdpeuLRge4RjsB/KSrVho8pwC2w==",
                "EraseVer": false
            },
            {
                "Version": 6,
                "URL": "http://192.168.43.173:8000/wifi_ota100.bin",
                "Digest": "885189cbb24b7b1a0175deef9d5d61f53c247d89a095b5a3686f54ca2d5dfbaa",
                "Signature": "pra2wlOiZO/zjzqaP9DZGe9dmm0aC4gx4r0yoyI7DU3sVpkdJ034v5XoiN5jdpeuLRge4RjsB/KSrVho8pwC2w==",
                "EraseVer": false
            }
           ]
    }
```

OTA service will download json file from server first when update check process is triggered by application, try to fetch information and proceed further .

**For more details , Please follow documentation provided for Over The Air \(OTA\) firmware update System Service**.

## OTA image generation

Please refer `Generating OTA image section` in `usage` manual present in the documentation of **Over The Air \(OTA\) firmware update System Service**.

## Downloading and building the application

To download or clone this application from Github, go to the [top level of the repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01)

Path of the application within the repository is **apps/wifi\_ota\_app\_upgrade/firmware** .

To build the application, refer to the following table and open the project using its IDE.

|Project Name|Description|
|------------|-----------|
|pic32mz\_w1\_curiosity\_freertos.X|MPLABX project for PIC32MZ W1 Curiosity Board|
| | |

## Setting up PIC32MZ W1 Curiosity Board

-   Connect the Debug USB port on the board to the computer using a micro USB cable

-   On the GPIO Header \(J207\), connect U1RX \(PIN 13\) and U1TX \(PIN 23\) to TX and RX pin of any USB to UART converter

-   Home AP \(Wi-Fi Access Point with internet connection\)

-   HTTP server.

-   python 3.9.0 .


## Running the Application

1.  Open the project "wifi\_ota\_app\_upgrade".

2.  Demo is configured with the default wifi credentials shown in the image below. To change the configurations launch the Harmony 3 configurator and update the home AP credentials for STA Mode and generate code.

    ![resized_wifi_sta_MHC1](images/GUID-4120AA9A-9A2D-439B-A8EB-AB6187572D56-low.png)

3.  Configuring server url :

    ![resized_srvr_confg_in_mhc](images/GUID-B43AAA40-5D61-4700-8B24-7DDAD7F6522F-low.png)

    For more details on manifest file, please follow `OTA server JSON manifest` section of [usage](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/system/ota/docs/usage.md) manual, which is part of `ota` system component.

4.  As a part of OTA process device will try to connect to user defined HTTP server. If device is able to connect to server without any error, it will try to fetch json manifest information .

    -   User can use any HTTP server.

    -   User may also use python command to create a local http server using below steps:

        -   Open command prompt and change driectory to the folder where ota image is present.

            ![resized_Change_dir](images/GUID-19779A7F-ED44-42C0-A43A-4A4CAE913F8E-low.png)

    -   Use below python command in command prompt:`python -m http.server 8000`

        ![resized_http_server](images/GUID-91CEC460-BF61-4E72-AB71-68B6BA66652D-low.png)

5.  Default Application supports Non-secure OTA. To enable Secure-OTA , please follow details provided in [Bootloader document.](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/blob/master/apps/ota_bootloader/readme.md)

6.  Generate the code using MHC.

7.  To create factory reset image , It is required to integrate the bootloader and ota application image and create a single unified HEX file. To integrate 2 images we can use hexmate tool, which is readily available with MPLABX package as part of the standard installation.To combine the hex files -

    -   User should load the "ota\_bootloader" project located in the apps folder of "wireless\_apps\_pic32mzw1\_wfi32e01" repo and include it into "wifi\_ota\_app\_upgrade" project as a "Loadable" component. For this, right click on the "wifi\_ota\_app\_upgrade" project, click on "properties" and select "ota\_bootloader" project. User need to make sure that the steps mentioned in "ota\_bootloader" reference document is followed, before this step.

        ![project_loading](images/GUID-C05EBC59-6162-4BBA-8FB4-AE14F32BCDB0-low.png)

        ![project_loading_1](images/GUID-5E9C8475-FFAE-444B-8C7F-289AFC26D9EB-low.png)

    -   Click on "Apply" button to make the applied changes effective:

        ![project_loading_2](images/GUID-2EDC5A2F-22B9-4A1A-A79B-DC256359A681-low.png)

8.  Build and program the application.

9.  Connect to a USB to UART converter to UART1 and Open a Terminal application \(Ex.:Tera term\) on the computer. Configure the serial settings as follows:

    -   Baud : 115200

    -   Data : 8 Bits

    -   Parity : None

    -   Stop : 1 Bit

    -   Flow Control : None

10. The device will connect to the Home AP and print the IP address obtained.

    ![wifi_sta_log1](images/GUID-403B0296-E89A-47DF-9B64-4130020C697E-low.png)

11. Once IP address is obtained the device will initiate OTA process and try to fetch json manifest content, periodically for every 60 sec .

12. User should make sure that both HTTP server and the PIC32MZW1 device are part of same wifi network \(or connected to same Home AP\).

13. User can see periodical messages as shown in belo screen shot :

    ![ota_peridocal_msg](images/GUID-13EA1555-6FD0-4EC3-B434-DE51E642E0B6-low.png)

14. In case , secure-ota is enabled please store `Factory image` signature into system \(External flash\). Please follow steps provided in `Verification of factory image` section of [usage](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/system/ota/docs/usage.md) manual, which is part of `ota` system component.

15. It is required to perform a "post-build" step to create ota image with file extension ".bin" \(which can be placed in the server and downloaded during OTA process\).

    -   All required files for post-build process will be generated \(during `step 5)`, mentioned above\) automatically in "tools" folder, created inside project folder.

        ![tools_folder](images/GUID-5C421474-4155-42A0-BFF7-EA484BC4F8E4-low.png)

    -   Right click on the "wifi\_ota\_app\_upgrade" project and click on properties.

        ![project_loading](images/GUID-C05EBC59-6162-4BBA-8FB4-AE14F32BCDB0-low.png)

    -   Select "building", insert below command and click "OK":

        `../../tools/hex2bin/hex2bin.exe`

        **Note**: python should be present in the system variable path.

        ![post_build](images/GUID-B1CAAA53-3A1A-4C1C-8507-145C155A111B-low.png)

        **For more details , Please follow documentation provided in [link](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/tree/master/system/ota/docs)**

16. For the `.bin` OTA image to be downloded, user can use the same project "wifi\_ota\_app\_upgrade":

    -   User should change the "version number" in the OTA component in the MHC higher than currently running image. Ensure the same version number is there in the json file also.

    -   Compile the project.

    -   Update the "Digest" and other relevant fields in the json file corresponding to the ota image to be downloaded. The "Digest" is printed in the compilation logs.

    -   Place the `.bin` image to be downloded, into the HTTP server. The `.bin` image can be found in below path which is generated during "wifi\_ota\_app\_upgrade" project build :<br />`..\firmware\wifi_ota_app_upgrade.X\dist\pic32mz_w1_curiosity_freertos\production`

        Please follow `OTA server JSON manifest` section of [usage](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/system/ota/docs/usage.md) manual, which is part of `ota` system component.

17. In case the user wants to download any other '.bin' image, they need to ensure that the digest corresponding to the image is calculated and updated in the json file.

18. Once image is downloaded successfully, the application will print a message in the console. User need to reset the device to load the new image.

    ![ota_process_pass](images/GUID-8EBE2506-8AAC-45A2-B70D-E60960DD196A-low.png)

19. If OTA upgrade fails, user need to reset the device to initiate OTA process again.

20. During reset, device will check if any newly downloaded image is available in the external flash\(sst26vf\):

    -   if yes, bootloader will program new image to program-flash area of the device from external flash.

        -   if programming is successful, bootloader will perform signature verification \(if secure-ota is enabled sinature verification will be performed ,otherwise signature verification will not be performed\), digest verification of image and if verification is succssful, bootloader will hand over control and application will start executing if.

        -   if verifiction is failed , bootloader will try to load previous successful image

        For more details please follow `Flow chart` provided in [usage](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/system/ota/docs/usage.md) manual, which is part of `ota` system component.

    -   if no new image is available then bootloader will hand over control without programming any image and application image already present in the program-flash area will start executing

21. User will come to know if new image is running by checking the version number in console print.


