

![Microchip logo](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_logo.png)
![Harmony logo small](https://raw.githubusercontent.com/wiki/Microchip-MPLAB-Harmony/Microchip-MPLAB-Harmony.github.io/images/microchip_mplab_harmony_logo_small.png)

# Microchip MPLAB® Harmony 3 Release Notes

## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.8.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       20            |

### New Features
- Migrated all the applications with latest "wireless_wifi" driver v3.9.1 
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)


### Known Issues
- In paho mqtt client power save application, user is requested to ensure that plib_clk.c, app_mqtt.c , app_mqtt.h,app.c and app.h files are not overwritten with generated code.

### Development Tools

- [MPLAB® X IDE v6.15](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator (MCC) v5.3.1
- [MPLAB® XC32 C/C++ Compiler v4.35](https://www.microchip.com/mplab/compilers)
- **DFP 1.8.326**



## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.7.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       20            |

### New Features
- All applications have been migrated from MHC to MCC.
- Low Power demo application updated with Wi-Fi connected sleep modes(WSM and WDS).
- Updated OTA bootloader and application with support for WOLFSSL and Trustflex.
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)


### Known Issues
- In paho mqtt client power save application, user is requested to ensure that plib_clk.c, app_mqtt.c , app_mqtt.h,app.c and app.h files are not overwritten with generated code.

### Development Tools

- [MPLAB® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Code Configurator (MCC) v5.1.17
- [MPLAB® XC32 C/C++ Compiler v3.01](https://www.microchip.com/mplab/compilers)
- **DFP 1.6.220**


## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.6.1**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       20            |

### New Features
- -	New Low power save demo added.
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)


### Known Issues
- Due to an underlying issue in the underlying `net` repo, you need to turn off `-Werror` for the `icmp.c` file in your projects.
- While programming the image into hardware, user may observe the “Target Device ID is an Invalid Device Id” warning which needs to be ignored. The issue will be resolved with new DFP package release.

### Development Tools

- [MPLAB® X IDE v6.00](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Harmony Configurator (MHC) v3.8.2
- [MPLAB® XC32 C/C++ Compiler v3.01](https://www.microchip.com/mplab/compilers)
- **DFP 1.5.203**

## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.6.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       19            |

### New Features
- New IFTTT and Web Socket Server demos added
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)


### Known Issues
- Due to an underlying issue in the underlying `net` repo, you need to turn off `-Werror` for the `icmp.c` file in your projects.
- L2 Bridging is known to have some instabilities when there is high network data traffic.

### Development Tools

- [MPLAB® X IDE v5.50](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Harmony Configurator (MHC) v3.8.0
- [MPLAB® XC32 C/C++ Compiler v3.01](https://www.microchip.com/mplab/compilers)
- **DFP 1.5.203**

## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.5.1**

**Note** : This is an incremental release. All notes under the `v3.5.0` release are applicable to this release as well.
### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       17            |

### New Features
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)


### Known Issues
- Due to an underlying issue in the DHCP code in `net` repo, compiler optimization should be turned off in the dual interface project.
- L2 Bridging is known to have some instabilities when there is high network data traffic.

### Development Tools

- [MPLAB® X IDE v5.50](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Harmony Configurator (MHC) v3.8.0
- [MPLAB® XC32 C/C++ Compiler v3.01](https://www.microchip.com/mplab/compilers)

## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.5.0**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       17            |

### New Features
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)

### Bug fixes and Improvements
- Please refer to the [system services release notes.](https://github.com/Microchip-MPLAB-Harmony/wireless_system_pic32mzw1_wfi32e01/blob/master/release_notes.md)
- UART3 is now configured to use RK13/RK14 pins.

### Known Issues
- Due to an underlying issue in the DHCP code in `net` repo, compiler optimization should be turned off in the dual interface project.
- L2 Bridging is known to have some instabilities when there is high network data traffic.

### Development Tools

- [MPLAB® X IDE v5.50](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Harmony Configurator (MHC) v3.8.0
- [MPLAB® XC32 C/C++ Compiler v3.01](https://www.microchip.com/mplab/compilers)


## Harmony 3 Wireless application examples for PIC32MZ W1 family  **v3.4.1**

### Development kit and demo application support

Following table provides number of Wireless examples available for different development kits.

| Development Kits           | MPLABx applications |
|:--------------------------:|:-------------------:|
| PIC32MZ W1 Curiosity Board |       17            |

### New Features
- Split wireless apps into a seperate repo

### Bug fixes
- None

### Known Issues
- None

### Development Tools

- [MPLAB® X IDE v5.40](https://www.microchip.com/mplab/mplab-x-ide)
- MPLAB® X IDE plug-ins:
  - MPLAB® Harmony Configurator (MHC) v3.6.2
- [MPLAB® XC32 C/C++ Compiler v2.50](https://www.microchip.com/mplab/compilers)
