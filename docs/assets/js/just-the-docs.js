var myVariable = `
{"0": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "WLAN STA",
    "content": "This example application shows how to use the PIC32MZW1 WLAN APIs to configure the device in STA mode directly using the WLAN driver APIs. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#wlan-sta",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#wlan-sta"
  },"1": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Description",
    "content": "This application shows the various commands to be used for STA mode of PIC32MZW1 device. The user can use these commands in configuring the device that it wants to connect, scan operations, etc… . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#description",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#description"
  },"2": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository and click . Path of the application within the repository is apps/wifi_sta/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_driver.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#downloading-and-building-the-application"
  },"3": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Configuring the Application",
    "content": "Setting up a regulatory region . The available regulatory domains are - GEN, USA, EMEA, CUST1 and CUST2. One of these regulatory domains must be selected as an active domain. This selection can be done in two ways. Using MHC . Figure below shows how to set the regulatory domain in MHC and the path is: TCPIP &amp; SYS_CONSOLE -&gt; TCP/IP STACK -&gt; BASIC CONFIGURATION -&gt; PIC32MZW1 . Using command . wlan set regdomain &lt;reg_domain_name&gt; is the command used to set the regulatory domain. (wlan set regdomain USA – sets the regulatory domain to USA). ++Note++: User can change the regulatory domain using this command only if the current setting is “None” (cofigured using MHC) . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#configuring-the-application",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#configuring-the-application"
  },"4": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Setting up the hardware",
    "content": "The following table shows the target hardware for the application projects. | Project Name | Board | . | pic32mz_w1_curiosity_driver.X | PIC32MZ W1 Curiosity Board | . |   |   | . Setting up PIC32MZ W1 Curiosity Board . | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter like USB UART click respectively | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#setting-up-the-hardware",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#setting-up-the-hardware"
  },"5": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Running the Application",
    "content": ". | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | Build and Program the application project using its IDE | Console displays the initialization messages and WLAN config if already saved in NVM | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#running-the-application",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#running-the-application"
  },"6": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Supported Commands",
    "content": "| Command | Description | . | wlan config &lt;ssid&gt; &lt;ssid_length&gt; &lt;channel&gt; &lt;open/wpa2/wpam/wpa3/wep&gt; &lt;password&gt; | Configures the SSID, SSID length, channel number and the security of the AP that DUT wants to connect. NOTE: Valid channel numbers are in range 1-13 and 0 or 255 - scans all channels | . | wlan connect | Connects the device to the AP configured in the command “wlan config” | . | wlan scan_options &lt;num_slots&gt; &lt;active_slot_time in ms&gt; &lt;probes_per_slot&gt; &lt;passive_scan_time in ms&gt; &lt;stop_on_first&gt; | Configure scan parameters | . | wlan scan &lt;active / passive&gt; &lt;channel&gt; | Runs either a active or passive scan (as per the input) on the channel number specified for the time set in “wlan scan_options”. Note: If no time is set, default value is used | . | wlan scan_ssidlist &lt;channel&gt; &lt;num_ssids&gt; &lt;ssid_list&gt; | Scan a list of known SSID’s (Maximum SSID is 4) | . | wlan get rssi | Gets the RSSI of current association | . | wlan save config | Stores the WLAN configurations given in command “wlan config” to flash memory. On restart an attempt is made to establish a connection to the AP based on the credentials stored in flash memory. | . | wlan set regdomain | Sets the regulatory domain. | . | wlan get regdomain &lt;all / current&gt; | Displays the regulatory domain all – request all regulatory domains; current - request current regulatory domain | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html#supported-commands",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html#supported-commands"
  },"7": {
    "doc": "Wi-Fi STA (driver mode)",
    "title": "Wi-Fi STA (driver mode)",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html",
    "relUrl": "/apps/wifi_sta/config_driver_readme.html"
  },"8": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "WLAN AP",
    "content": "This example application shows how to use the PIC32MZW1 WLAN APIs to configure the device in AP mode directly using the WLAN driver APIs. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#wlan-ap",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#wlan-ap"
  },"9": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Description",
    "content": "This application shows the various commands to be used for AP mode of PIC32MZW1 device. The user can use these commands in configuring the device as AP, start or stop the device as AP, scan operations etc… . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#description",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#description"
  },"10": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository and click . Path of the application within the repository is apps/wifi_ap/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_driver.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#downloading-and-building-the-application"
  },"11": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Configuring the Application",
    "content": "Setting up a regulatory region . The available regulatory domains are - GEN, USA, EMEA, CUST1 and CUST2. One of these regulatory domains must be selected as an active domain. This selection can be done in two ways. Using MHC . Figure below shows how to set the regulatory domain in MHC and the path is: TCPIP &amp; SYS_CONSOLE -&gt; TCP/IP STACK -&gt; BASIC CONFIGURATION -&gt; PIC32MZW1 . Using command . wlan set regdomain &lt;reg_domain_name&gt; is the command used to set the regulatory domain. (wlan set regdomain USA – sets the regulatory domain to USA). ++Note++: User can change the regulatory domain using this command only if the current setting is “None” (cofigured using MHC) . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#configuring-the-application",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#configuring-the-application"
  },"12": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Setting up the hardware",
    "content": "The following table shows the target hardware for the application projects. | Project Name | Board | . | pic32mz_w1_curiosity_driver.X | PIC32MZ W1 Curiosity Board | . |   |   | . Setting up PIC32MZ W1 Curiosity Board . | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter like USB UART click respectively | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#setting-up-the-hardware",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#setting-up-the-hardware"
  },"13": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Running the Application",
    "content": ". | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | Build and Program the application project using its IDE | Console displays the initialization messages and WLAN config if already saved in NVM | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#running-the-application",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#running-the-application"
  },"14": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Supported Commands",
    "content": "| Command | Description | . | wlan config &lt;ssid&gt; &lt;ssid_length&gt; &lt;ssid_visible&gt; &lt;channel&gt; &lt;open/wpa2/wpam/wpa3/wpa3m/wep&gt; &lt;password&gt; | Configures the SSID, SSID length, SSID visibility, channel number and the security of the AP that DUT wants to connect. NOTE: Valid channel numbers are in range 1-13 | . | wlan ap start | Start the AP | . | wlan ap stop | Stop the AP | . | wlan show devices | The connected devices are shown – MAC and IP addresses are displayed | . | wlan scan &lt;active / passive&gt; &lt;channel&gt; | Runs either a active or passive scan (as per the input) on the channel number specified | . | wlan save config | Stores the WLAN configurations given in command “wlan config” to flash memory. On restart DUT is started as AP with the credentials stored in flash. | . | wlan set regdomain | Sets the regulatory domain | . | wlan get regdomain &lt;all / current&gt; | Displays the regulatory domain all – request all regulatory domains; current - request current regulatory domain | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html#supported-commands",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html#supported-commands"
  },"15": {
    "doc": "Wi-Fi Soft AP (driver mode)",
    "title": "Wi-Fi Soft AP (driver mode)",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html",
    "relUrl": "/apps/wifi_ap/config_driver_readme.html"
  },"16": {
    "doc": "License",
    "title": "License",
    "content": "IMPORTANT: READ CAREFULLY . MICROCHIP IS WILLING TO LICENSE THIS INTEGRATED SOFTWARE FRAMEWORK SOFTWARE AND ACCOMPANYING DOCUMENTATION OFFERED TO YOU ONLY ON THE CONDITION THAT YOU ACCEPT ALL OF THE FOLLOWING TERMS. TO ACCEPT THE TERMS OF THIS LICENSE, CLICK “I ACCEPT” AND PROCEED WITH THE DOWNLOAD OR INSTALL. IF YOU DO NOT ACCEPT THESE LICENSE TERMS, CLICK “I DO NOT ACCEPT,” AND DO NOT DOWNLOAD OR INSTALL THIS SOFTWARE. NON-EXCLUSIVE SOFTWARE LICENSE AGREEMENT FOR MICROCHIP MPLAB HARMONY INTEGRATED SOFTWARE FRAMEWORK . This Nonexclusive Software License Agreement (“Agreement”) is between you, your heirs, agents, successors and assigns (“Licensee”) and Microchip Technology Incorporated, a Delaware corporation, with a principal place of business at 2355 W. Chandler Blvd., Chandler, AZ 85224-6199, and its subsidiary, Microchip Technology (Barbados) II Incorporated (collectively, “Microchip”) for Microchip’s MPLAB Harmony Integrated Software Framework (“Software”) and accompanying documentation (“Documentation”). The Software and Documentation are licensed under this Agreement and not sold. U.S. copyright laws and international copyright treaties, and other intellectual property laws and treaties protect the Software and Documentation. Microchip reserves all rights not expressly granted to Licensee in this Agreement. | License and Sublicense Grant. (a) Definitions. As used this Agreement, the following terms shall have the meanings defined below: . (i) \\\"Licensee Products\\\" means Licensee products that use or incorporate Microchip Products. (ii) \\\"Microchip Product\\\" means Microchip 16-bit and 32-bit microcontrollers, digital signal controllers or other Microchip semiconductor products with PIC16 and PIC18 prefix and specifically excepting the CX870 and CY920, which are not covered under this Agreement, that use or implement the Software. (iii) \\\"Object Code\\\" means the Software computer programming code provided by Microchip that is in binary form (including related documentation, if any) and error corrections, improvements and updates to such code provided by Microchip in its sole discretion, if any. (iv) \\\"Source Code\\\" means the Software computer programming code provided by Microchip that may be printed out or displayed in human readable form (including related programmer comments and documentation, if any), and error corrections, improvements, updates, modifications and derivatives of such code developed by Microchip, Licensee or Third Party. (v) \\\"Third Party\\\" means Licensee's agents, representatives, consultants, clients, customers, or contract manufacturers. (vi) \\\"Third Party Products\\\" means Third Party products that use or incorporate Microchip Products. (b) Software License Grant. Subject to the terms of this Agreement, Microchip grants strictly to Licensee a personal, worldwide, non-exclusive, non-transferable limited license to use, modify (except as limited by Section 1(f) below), copy and distribute the Software only when the Software is embedded on a Microchip Product that is integrated into Licensee Product or Third Party Product pursuant to Section 2(d) below. Any portion of the Software (including derivatives or modifications thereof) may not be: . (i) embedded on a non-Microchip microcontroller or digital signal controller; (ii) distributed (in Source Code or Object Code), except as described in Section 2(d) below. (c) Documentation License Grant. Subject to all of the terms and conditions of this Agreement, Microchip grants strictly to Licensee a perpetual, worldwide, non-exclusive license to use the Documentation in support of Licensee’s use of the Software. (d) Sublicense Grants. Subject to terms of this Agreement, Licensee may grant a limited sublicense to a Third Party to use the Software as described below only if such Third Party expressly agrees to be bound by terms of confidentiality and limited use that are no broader in scope and duration than the confidentiality and limited use terms of this Agreement: . (i) Third Party may modify Source Code for Licensee, except as limited by Section 1(f) below. (ii) Third Party may program Software into Microchip Products for Licensee. (iii) Third Party may use Software to develop and/or manufacture Licensee Product. (iv) Third Party may use Software to develop and/or manufacture Third Party Products where either: (x) the sublicensed Software contains Source Code modified or otherwise optimized by Licensee for Third Party use; or (y) the sublicensed Software is programmed into Microchip Products by Licensee on behalf of such Third Party. (v) Third Party may use the Documentation in support of Third Party's authorized use of the Software in conformance with this Section 2(d). (e) Audit. Authorized representatives of Microchip shall have the right to reasonably inspect Licensee’s premises and to audit Licensee’s records and inventory of Licensee Products, whether located on Licensee’s premises or elsewhere at any time, announced or unannounced, and in its sole and absolute discretion, in order to ensure Licensee’s adherence to the terms of this Agreement. (f) License and Sublicense Limitation. This Section 1 does not grant Licensee or any Third Party the right to modify any dotstack™ Bluetooth® stack, profile, or iAP protocol included in the Software. | Third Party Requirements. Licensee acknowledges that it is Licensee’s responsibility to comply with any third party license terms or requirements applicable to the use of such third party software, specifications, systems, or tools, including but not limited to SEGGER Microcontroller GmbH &amp; Co. KG’s rights in the emWin software and certain libraries included herein. Microchip is not responsible and will not be held responsible in any manner for Licensee’s failure to comply with such applicable terms or requirements. | Open Source Components. Notwithstanding the license grants contained herein, Licensee acknowledges that certain components of the Software may be covered by so-called “open source” software licenses (“Open Source Components”). Open Source Components means any software licenses approved as open source licenses by the Open Source Initiative or any substantially similar licenses, including any license that, as a condition of distribution, requires Microchip to provide Licensee with certain notices and/or information related to such Open Source Components, or requires that the distributor make the software available in source code format. Microchip will use commercially reasonable efforts to identify such Open Source Components in a text file or “About Box” or in a file or files referenced thereby (and will include any associated license agreement, notices, and other related information therein), or the Open Source Components will contain or be accompanied by its own license agreement. To the extent required by the licenses covering Open Source Components, the terms of such licenses will apply in lieu of the terms of this Agreement, and Microchip hereby represents and warrants that the licenses granted to such Open Source Components will be no less broad than the license granted in Section 1(b). To the extent the terms of the licenses applicable to Open Source Components prohibit any of the restrictions in this Agreement with respect to such Open Source Components, such restrictions will not apply to such Open Source Components. | Licensee’s Obligations. (a) Licensee will ensure Third Party compliance with the terms of this Agreement. (b) Licensee will not: (i) engage in unauthorized use, modification, disclosure or distribution of Software or Documentation, or its derivatives; (ii) use all or any portion of the Software, Documentation, or its derivatives except in conjunction with Microchip Products; or (iii) reverse engineer (by disassembly, decompilation or otherwise) Software or any portion thereof; or (iv) copy or reproduce all or any portion of Software, except as specifically allowed by this Agreement or expressly permitted by applicable law notwithstanding the foregoing limitations. (c) Licensee must include Microchip’s copyright, trademark and other proprietary notices in all copies of the Software, Documentation, and its derivatives. Licensee may not remove or alter any Microchip copyright or other proprietary rights notice posted in any portion of the Software or Documentation. (d) Licensee will defend, indemnify and hold Microchip and its subsidiaries harmless from and against any and all claims, costs, damages, expenses (including reasonable attorney’s fees), liabilities, and losses, including without limitation product liability claims, directly or indirectly arising from or related to: (i) the use, modification, disclosure or distribution of the Software, Documentation or any intellectual property rights related thereto; (ii) the use, sale, and distribution of Licensee Products or Third Party Products, and (iii) breach of this Agreement. THE FOREGOING STATES THE SOLE AND EXCLUSIVE LIABILITY OF THE PARTIES FOR INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT. | Confidentiality. (a) Licensee agrees that the Software (including but not limited to the Source Code, Object Code and library files) and its derivatives, Documentation and underlying inventions, algorithms, know-how and ideas relating to the Software and the Documentation are proprietary information belonging to Microchip and its licensors (“Proprietary Information”). Except as expressly and unambiguously allowed herein, Licensee will hold in confidence and not use or disclose any Proprietary Information and shall similarly bind its employees and Third Party(ies) in writing. Proprietary Information shall not include information that: (i) is in or enters the public domain without breach of this Agreement and through no fault of the receiving party; (ii) the receiving party was legally in possession of prior to receiving it; (iii) the receiving party can demonstrate was developed by it independently and without use of or reference to the disclosing party’s Proprietary Information; or (iv) the receiving party receives from a third party without restriction on disclosure. If Licensee is required to disclose Proprietary Information by law, court order, or government agency, such disclosure shall not be deemed a breach of this Agreement provided that Licensee gives Microchip prompt notice of such requirement in order to allow Microchip to object or limit such disclosure, Licensee cooperates with Microchip to protect Proprietary Information, and Licensee complies with any protective order in place and discloses only the information required by process of law. (b) Licensee agrees that the provisions of this Agreement regarding unauthorized use and nondisclosure of the Software, Documentation and related Proprietary Rights are necessary to protect the legitimate business interests of Microchip and its licensors and that monetary damages alone cannot adequately compensate Microchip or its licensors if such provisions are violated. Licensee, therefore, agrees that if Microchip alleges that Licensee or Third Party has breached or violated such provision then Microchip will have the right to petition for injunctive relief, without the requirement for the posting of a bond, in addition to all other remedies at law or in equity. | Ownership of Proprietary Rights. (a) Microchip and its licensors retain all right, title and interest in and to the Software and Documentation (“Proprietary Rights”) including, but not limited to: (i) patent, copyright, trade secret and other intellectual property rights in the Software, Documentation, and underlying technology; (ii) the Software as implemented in any device or system, all hardware and software implementations of the Software technology (expressly excluding Licensee and Third Party code developed and used in conformance with this Agreement solely to interface with the Software and Licensee Products and/or Third Party Products); and (iii) all modifications and derivative works thereof (by whomever produced). Further, modifications and derivative works shall be considered works made for hire with ownership vesting in Microchip on creation. To the extent such modifications and derivatives do not qualify as a “work for hire,” Licensee hereby irrevocably transfers, assigns and conveys the exclusive copyright thereof to Microchip, free and clear of any and all liens, claims or other encumbrances, to the fullest extent permitted by law. Licensee and Third Party use of such modifications and derivatives is limited to the license rights described in Section 1 above. (b) Licensee shall have no right to sell, assign or otherwise transfer all or any portion of the Software, Documentation or any related intellectual property rights except as expressly set forth in this Agreement. | Termination of Agreement. Without prejudice to any other rights, this Agreement terminates immediately, without notice by Microchip, upon a failure by License or Third Party to comply with any provision of this Agreement. Further, Microchip may also terminate this Agreement upon reasonable belief that Licensee or Third Party have failed to comply with this Agreement. Upon termination, Licensee and Third Party will immediately stop using the Software, Documentation, and derivatives thereof, and immediately destroy all such copies, remove Software from any of Licensee’s tangible media and from systems on which the Software exists, and stop using, disclosing, copying, or reproducing Software (even as may be permitted by this Agreement). Termination of this Agreement will not affect the right of any end user or consumer to use Licensee Products or Third Party Products provided that such products were purchased prior to the termination of this Agreement. | Dangerous Applications. The Software is not fault-tolerant and is not designed, manufactured, or intended for use in hazardous environments requiring failsafe performance (“Dangerous Applications”). Dangerous Applications include the operation of nuclear facilities, aircraft navigation, aircraft communication systems, air traffic control, direct life support machines, weapons systems, or any environment or system in which the failure of the Software could lead directly or indirectly to death, personal injury, or severe physical or environmental damage. Microchip specifically disclaims (a) any express or implied warranty of fitness for use of the Software in Dangerous Applications; and (b) any and all liability for loss, damages and claims resulting from the use of the Software in Dangerous Applications. | Warranties and Disclaimers. THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. MICROCHIP AND ITS LICENSORS ASSUME NO RESPONSIBILITY FOR THE ACCURACY, RELIABILITY OR APPLICATION OF THE SOFTWARE OR DOCUMENTATION. MICROCHIP AND ITS LICENSORS DO NOT WARRANT THAT THE SOFTWARE WILL MEET REQUIREMENTS OF LICENSEE OR THIRD PARTY, BE UNINTERRUPTED OR ERROR-FREE. MICROCHIP AND ITS LICENSORS HAVE NO OBLIGATION TO CORRECT ANY DEFECTS IN THE SOFTWARE. LICENSEE AND THIRD PARTY ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE AND DOCUMENTATION PROVIDED UNDER THIS AGREEMENT. | Limited Liability. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL OR EQUITABLE THEORY FOR ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS. The aggregate and cumulative liability of Microchip and its licensors for damages hereunder will in no event exceed $1000 or the amount Licensee paid Microchip for the Software and Documentation, whichever is greater. Licensee acknowledges that the foregoing limitations are reasonable and an essential part of this Agreement. | General. (a) Governing Law, Venue and Waiver of Trial by Jury. THIS AGREEMENT SHALL BE GOVERNED BY AND CONSTRUED UNDER THE LAWS OF THE STATE OF ARIZONA AND THE UNITED STATES WITHOUT REGARD TO CONFLICTS OF LAWS PROVISIONS. Licensee agrees that any disputes arising out of or related to this Agreement, Software or Documentation shall be brought in the courts of State of Arizona. The parties agree to waive their rights to a jury trial in actions relating to this Agreement. (b) Attorneys’ Fees. If either Microchip or Licensee employs attorneys to enforce any rights arising out of or relating to this Agreement, the prevailing party shall be entitled to recover its reasonable attorneys’ fees, costs and other expenses. (c) Entire Agreement. This Agreement shall constitute the entire agreement between the parties with respect to the subject matter hereof. It shall not be modified except by a written agreement signed by an authorized representative of Microchip. (d) Severability. If any provision of this Agreement shall be held by a court of competent jurisdiction to be illegal, invalid or unenforceable, that provision shall be limited or eliminated to the minimum extent necessary so that this Agreement shall otherwise remain in full force and effect and enforceable. (e) Waiver. No waiver of any breach of any provision of this Agreement shall constitute a waiver of any prior, concurrent or subsequent breach of the same or any other provisions hereof, and no waiver shall be effective unless made in writing and signed by an authorized representative of the waiving party. (f) Export Regulation. Licensee agrees to comply with all export laws and restrictions and regulations of the Department of Commerce or other United States or foreign agency or authority. (g) Survival. The indemnities, obligations of confidentiality, and limitations on liability described herein, and any right of action for breach of this Agreement prior to termination shall survive any termination of this Agreement. (h) Assignment. Neither this Agreement nor any rights, licenses or obligations hereunder, may be assigned by Licensee without the prior written approval of Microchip except pursuant to a merger, sale of all assets of Licensee or other corporate reorganization, provided that assignee agrees in writing to be bound by the Agreement. (i) Restricted Rights. Use, duplication or disclosure by the United States Government is subject to restrictions set forth in subparagraphs (a) through (d) of the Commercial Computer-Restricted Rights clause of FAR 52.227-19 when applicable, or in subparagraph (c)(1)(ii) of the Rights in Technical Data and Computer Software clause at DFARS 252.227-7013, and in similar clauses in the NASA FAR Supplement. Contractor/manufacturer is Microchip Technology Inc., 2355 W. Chandler Blvd., Chandler, AZ 85225-6199. | . If Licensee has any questions about this Agreement, please write to Microchip Technology Inc., 2355 W. Chandler Blvd., Chandler, AZ 85224-6199 USA, ATTN: Marketing. Microchip MPLAB Harmony Integrated Software Framework. Copyright © 2015 Microchip Technology Inc. All rights reserved. License Rev. 11/2015 . Copyright © 2015 Qualcomm Atheros, Inc. All Rights Reserved. Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies. THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/mplab_harmony_license.html",
    "relUrl": "/mplab_harmony_license.html"
  },"17": {
    "doc": "Secured TCP Client",
    "title": "Secured TCP Client",
    "content": "The PIC32MZW1 example application acts as a TCP Client to connect to Secured TCP Server and exchange data in a non-blocking manner. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html#secured-tcp-client",
    "relUrl": "/apps/tcp_client/readme.html#secured-tcp-client"
  },"18": {
    "doc": "Secured TCP Client",
    "title": "Description",
    "content": "This application demonstrates how a user can use a TLS TCP client to connect to a TCP server. The user would need to configure the Wi-Fi credentials for the Home AP and the TLS server details. The default application will try to establish a TLS connection with www.google.com and send a POST request to it. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html#description",
    "relUrl": "/apps/tcp_client/readme.html#description"
  },"19": {
    "doc": "Secured TCP Client",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/tcp_client/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/tcp_client/readme.html#downloading-and-building-the-application"
  },"20": {
    "doc": "Secured TCP Client",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/tcp_client/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"21": {
    "doc": "Secured TCP Client",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure home AP credentials for STA Mode. | Currently Net Service is configured to run a TCP Client in Secured mode to connect to www.google.com on the https port (443). In case the user wants to change this config, please make the changes in the Net Service Module configurations as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will connect to the Home AP and print the IP address obtained. | The Board will connect to Home AP and then as per the default Net Service configuration, it shall connect to www.google.com and do data exchange: | . Note: The secured tcp connection may require the user to modify WolfSSL component settings in MHC depending on the security settings of the site/ server he is trying to access. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html#running-the-application",
    "relUrl": "/apps/tcp_client/readme.html#running-the-application"
  },"22": {
    "doc": "Secured TCP Client",
    "title": "Secured TCP Client",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html",
    "relUrl": "/apps/tcp_client/readme.html"
  },"23": {
    "doc": "TCP Server",
    "title": "TCP Server",
    "content": "This example application acts as a TCP Server to which a TCP Client can connect and exchange data in a non-blocking manner. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html#tcp-server",
    "relUrl": "/apps/tcp_server/readme.html#tcp-server"
  },"24": {
    "doc": "TCP Server",
    "title": "Description",
    "content": "This application demonstrates how a user can start a TCP server on the device which is running in Soft AP Mode. The user would need to configure the Wi-Fi settings for the Soft AP and the port number at which the TCP server would be started. The Application will come up as a Soft AP with the TCP Server running on it. A third-party STA will connect to the Soft AP device. The TCP Client on the third -party STA will then connect to the TCP Server running on the device. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html#description",
    "relUrl": "/apps/tcp_server/readme.html#description"
  },"25": {
    "doc": "TCP Server",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/tcp_server/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/tcp_server/readme.html#downloading-and-building-the-application"
  },"26": {
    "doc": "TCP Server",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Laptop/ Mobile (to run the TCP Client on it) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/tcp_server/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"27": {
    "doc": "TCP Server",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Currently Net Service is configured to run as TCP Server running on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module as shown below: . | Currently Net Service is configured to run a TCP Server which awaits connection from a TCP Client on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module configurations as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device shall come up as SoftAP and then as per the default Net Service configuration, the TCP Server shall come up, awaiting a connection from a TCP Client. | Connect a Laptop to the Soft AP (with ssid DEMO_AP_SOFTAP) running on the DUT | Start a TCP Client (python script) on laptop, giving the server IP as the IP of the Board, and the port as 5555: | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html#running-the-application",
    "relUrl": "/apps/tcp_server/readme.html#running-the-application"
  },"28": {
    "doc": "TCP Server",
    "title": "TCP Server",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html",
    "relUrl": "/apps/tcp_server/readme.html"
  },"29": {
    "doc": "UDP Client",
    "title": "UDP Client",
    "content": "This example application acts as a UDP Client which connects to a UDP Server and exchanges data in a non-blocking manner. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html#udp-client",
    "relUrl": "/apps/udp_client/readme.html#udp-client"
  },"30": {
    "doc": "UDP Client",
    "title": "Description",
    "content": "This application demonstrates how a user can use a UDP client to connect to a UDP server. The user would need to configure the Wi-Fi credentials for the Home AP and the UDP server details. The default application will send a string to the UDP server. Any standard UDP server can be used to enable the device to connect as a UDP client. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html#description",
    "relUrl": "/apps/udp_client/readme.html#description"
  },"31": {
    "doc": "UDP Client",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/udp_client/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/udp_client/readme.html#downloading-and-building-the-application"
  },"32": {
    "doc": "UDP Client",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Laptop/ Mobile (to run the UDP Server on it) | Access Point | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/udp_client/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"33": {
    "doc": "UDP Client",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure Home AP credentials for STA Mode. | Currently Net Service is configured to run as UDP Client, which connects to a UDP Server running on port 4444. In case the user wants to change this config, please make the changes in the Net Service Module as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | User can run any third party utility or a udp_server python script to act as UDP Server running on the Laptop . | The Board shall connect to Home AP and then as per the default Net Service configuration, it shall bring up the UDP Client which shall connect to a UDP Server. | . Note: TLS Connection is not supported for UDP Client configuration . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html#running-the-application",
    "relUrl": "/apps/udp_client/readme.html#running-the-application"
  },"34": {
    "doc": "UDP Client",
    "title": "UDP Client",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html",
    "relUrl": "/apps/udp_client/readme.html"
  },"35": {
    "doc": "UDP Server",
    "title": "UDP Server",
    "content": "This example application acts as a UDP Server to which a UDP Client can connect and exchange data in a non-blocking manner. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html#udp-server",
    "relUrl": "/apps/udp_server/readme.html#udp-server"
  },"36": {
    "doc": "UDP Server",
    "title": "Description",
    "content": "This application demonstrates how a user can start a UDP server on the device. The user would need to configure the Wi-Fi credentials for the Home AP and the port number at which the UDP server should be started. Any standard UDP client can be used to connect to the device which is operating as a UDP server. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html#description",
    "relUrl": "/apps/udp_server/readme.html#description"
  },"37": {
    "doc": "UDP Server",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/udp_server/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/udp_server/readme.html#downloading-and-building-the-application"
  },"38": {
    "doc": "UDP Server",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Laptop/ Mobile (to run the UDP Client on it) | Home AP (Wi-Fi Access Point) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/udp_server/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"39": {
    "doc": "UDP Server",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure Home AP credentials for STA Mode. | Currently Net Service is configured to run as UDP Server running on port 3333. In case the user wants to change this config, please make the changes in the Net Service Module as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The Board shall connect to Home AP and then as per the default Net Service configuration, it shall bring up the UDP Server which shall await connection from a UDP Client.. | User can run any third party utility or a udp_client python script to act as UDP Client running on the Laptop | . Note: TLS Connection is not supported for UDP Server configuration . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html#running-the-application",
    "relUrl": "/apps/udp_server/readme.html#running-the-application"
  },"40": {
    "doc": "UDP Server",
    "title": "UDP Server",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html",
    "relUrl": "/apps/udp_server/readme.html"
  },"41": {
    "doc": "Weather Client",
    "title": "Weather Client",
    "content": "This example demonstrates the use of the PIC32MZW1 to retrieve weather information from a weather server (api.openweathermap.org). ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html#weather-client",
    "relUrl": "/apps/weather_client/readme.html#weather-client"
  },"42": {
    "doc": "Weather Client",
    "title": "Description",
    "content": "This application demonstrates how a user can use a TCP client to connect to a Weather server. The user would need to configure the Wi-Fi credentials for the Home AP.The default application will try to establish a TCP Client connection with api.openweathermap.org and send a POST request to it. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html#description",
    "relUrl": "/apps/weather_client/readme.html#description"
  },"43": {
    "doc": "Weather Client",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/weather_client/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/weather_client/readme.html#downloading-and-building-the-application"
  },"44": {
    "doc": "Weather Client",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/weather_client/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"45": {
    "doc": "Weather Client",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure home AP credentials for STA Mode. | Currently Net Service is configured to run a TCP Client in non-Secured mode to connect to api.openweathermap.org on the http port (80). In case the user wants to change this config, please make the changes in the Net Service Module configurations as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will connect to the Home AP and print the IP address obtained. | The Board will connect to Home AP and then as per the default Net Service configuration, it shall connect to api.openweathermap.org and do data exchange: | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html#running-the-application",
    "relUrl": "/apps/weather_client/readme.html#running-the-application"
  },"46": {
    "doc": "Weather Client",
    "title": "Weather Client",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html",
    "relUrl": "/apps/weather_client/readme.html"
  },"47": {
    "doc": "Paho MQTT TLS Client",
    "title": "Paho MQTT TLS Client",
    "content": "This example application acts as a Paho MQTT TLS Client to connect to Secured MQTT Broker and publishes and subscribes to data on subtopics. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html#paho-mqtt-tls-client",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html#paho-mqtt-tls-client"
  },"48": {
    "doc": "Paho MQTT TLS Client",
    "title": "Description",
    "content": "This application demonstrates how a user can use the Paho MQTT Client to connect to a Secured MQTT Broker. The application shall come up in AP Provisioing mode, and user will need to configure the Wi-Fi credentials for the Home AP. The default application will try to establish MQTT TLS connection with test.mosquitto.org and publishes message to a topic every 30 sec. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html#description",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html#description"
  },"49": {
    "doc": "Paho MQTT TLS Client",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/paho_mqtt_tls_client/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html#downloading-and-building-the-application"
  },"50": {
    "doc": "Paho MQTT TLS Client",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"51": {
    "doc": "Paho MQTT TLS Client",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Currently MQTT Service is configured to run a MQTT Client in Secured mode to connect to test.mosquitto.org on the mqtt secured port (8883). In case the user wants to change this config, please make the changes in the MQTT Service Module configurations as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will come up in AP Provisioning mode, and wait for Home AP credentials to be configured into it. | Provision the device using the following CLI Command “wifiprov set 0 1 “GEN” 0 1 3 “DEMO_AP” “password”” for Home AP with SSID DEMO_AP, and password as ‘password’. More details on the CLI command can be found here. | The Board will connect to Home AP and then as per the default MQTT Service configuration, it shall connect to test.mosquitto.org and publish messages on the topic ‘MCHP/Sample/a’ periodically every 30 sec. It also subscribes to topic ‘MCHP/Sample/b’ . | If any third party MQTT Client connects to test.mosquitto.org and subscribes to the topic ‘MCHP/Sample/a’, it will receive the periodic messages being published by the MQTT Client running on the PIC32MZW1. | In case the Third Party MQTT Client publishes onto the topic ‘MCHP/Sample/b’, the Paho MQTT Client running on the PIC32MZW1 shall receive the messages being published onto this topic since it has subscribed to it. | . Note: The secured tcp connection may require the user to modify WolfSSL component settings in MHC depending on the security settings of the site/ server he is trying to access. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html#running-the-application",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html#running-the-application"
  },"52": {
    "doc": "Paho MQTT TLS Client",
    "title": "Paho MQTT TLS Client",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html",
    "relUrl": "/apps/paho_mqtt_tls_client/readme.html"
  },"53": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Wi-Fi Access Point(AP) Mode",
    "content": "This example application acts as a Wi-Fi Access point(AP) to connect to Laptop/Mobile(STA) and exchange data. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html#wi-fi-access-pointap-mode",
    "relUrl": "/apps/wifi_ap/readme.html#wi-fi-access-pointap-mode"
  },"54": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Description",
    "content": "This application demonstrates how a user can configure the device to operate in SoftAP mode.The device will come up in SoftAP mode and any other device(STA) can connect as a STA to the SoftAP. The third party device should be assigned an IP by the SoftAP. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html#description",
    "relUrl": "/apps/wifi_ap/readme.html#description"
  },"55": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_ap/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_ap/readme.html#downloading-and-building-the-application"
  },"56": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_ap/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"57": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure softAP credentials as required. | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will boot in softAP mode. | Connect to the softAP from a laptop or a Mobile Phone. User may have to enter password on laptop or mobile phone for connecting to device SoftAP based on security type selected in MHC. As soon as the client connects, the device will print the IP address leased to it . | From DUT, user can ping the connected device. | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html#running-the-application",
    "relUrl": "/apps/wifi_ap/readme.html#running-the-application"
  },"58": {
    "doc": "Wi-Fi Soft AP (Service mode)",
    "title": "Wi-Fi Soft AP (Service mode)",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html",
    "relUrl": "/apps/wifi_ap/readme.html"
  },"59": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Wi-Fi Easy Configuration",
    "content": "This example showcase Wi-Fi AP application to illustrate SoftAP mode Wi-Fi provisioning via command line(CLI),Socket and HTTP. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html#wi-fi-easy-configuration",
    "relUrl": "/apps/wifi_easy_config/readme.html#wi-fi-easy-configuration"
  },"60": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Description",
    "content": "This application demonstrates how a user can configure the device with the credentials of the Home AP. The device will come up in SoftAP mode, the user will connect to the SoftAP via a third party STA. User will then send the Home AP credentials to the device and the device will store the credentials in the NVM. The device will auto reboot. Upon reboot, the device will come up in STA mode, it will connect to the Home AP and acquire an IP address. DUT will act either in AP mode or STA mode. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html#description",
    "relUrl": "/apps/wifi_easy_config/readme.html#description"
  },"61": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_easy_config/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX FreeRTOS project for PIC32MZ W1 Curiosity Board | . | pic32mz_w1_curiosity | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_easy_config/readme.html#downloading-and-building-the-application"
  },"62": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_easy_config/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"63": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure SoftAP credentials as required. | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will boot in SoftAP mode. | . Provisioning using TCP Socket . | Connect to the SoftAP from a laptop or mobile phone.User may have to enter password on laptop or mobile phone for connecting to device SoftAP based on security type selected in MHC. As soon as the client connects, the device will print the IP address leased to it. | Using a TCP Client tool like Packet Sender installed in the laptop connected to the softAP, send the following JSON file to AP’s provisioning port (192.168.1.1:6666) . | Note: User would need to edit the default JSON file as per their home AP settings. | . { \\\"mode\\\": 0, \\\"save_config\\\": 1,\\\"countrycode\\\":\\\"GEN\\\", \\\"STA\\\": { \\\"ch\\\": 0, \\\"auto\\\": 1, \\\"auth\\\": 3, \\\"SSID\\\": \\\"dlink\\\", \\\"PWD\\\":\\\"password\\\"}, \\\"AP\\\": {\\\"ch\\\": 2, \\\"ssidv\\\": 1, \\\"auth\\\": 4, \\\"SSID\\\": \\\"DEMO_AP_SOFTAP\\\", \\\"PWD\\\": \\\"password\\\" } } . | For more details follow the steps available in page: * Wi-Fi provisioning Using JSON Format | . Provisioning using Mobile Application . | Connect to the SoftAP from a mobile phone.User may have to enter password on mobile phone for connecting to device SoftAP based on security type selected in MHC. As soon as the client connects, the device will print the IP address leased to it. | For more details follow the steps available in page: Wi-Fi provisioning Using Mobile Application . | . Provisioning using HTTP(webpage) . | Connect to the SoftAP from a laptop or mobile phone.User may have to enter password on laptop or mobile phone for connecting to device SoftAP based on security type selected in MHC. As soon as the client connects, the device will print the IP address leased to it. | For more details follow the steps available in page:Wi-Fi provisioning using HTTP . | . Provisioning using Command line . | User can enter the below command to switch mode to STA and set desired SSID,passphase and security . | wifiprov set 0 1 &lt;country_code&gt; &lt;channel&gt; &lt;authtype&gt; &lt;ssid_name&gt; &lt;psk_name&gt; . | For more details follow the steps available in page: Wi-Fi provisioning using CLI . | . The device will switch from AP mode to STA mode. The device will now connect to the configured AP. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html#running-the-application",
    "relUrl": "/apps/wifi_easy_config/readme.html#running-the-application"
  },"64": {
    "doc": "Wi-Fi Easy Configuration",
    "title": "Wi-Fi Easy Configuration",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html",
    "relUrl": "/apps/wifi_easy_config/readme.html"
  },"65": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Wifi-ethernet dual interface",
    "content": "This example application acts as a Soft AP with both wifi and ethernet interface enabled. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html#wifi-ethernet-dual-interface",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html#wifi-ethernet-dual-interface"
  },"66": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Description",
    "content": "This application demonstrates how a user can enable both wifi and ethernet interface. The system shall run in freertos mode, and the application shall come up in AP mode. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html#description",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html#description"
  },"67": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_eth_dual_interface/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html#downloading-and-building-the-application"
  },"68": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Laptop (connect the ethernet cable between laptop and curiosity board) | Mobile (wifi-interface) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"69": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure credentials for AP Mode. | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The Board will come up in AP mode with AP mode IP address: . | Connect wifi interface to “pic32mzw1” AP. Once wifi interface is connected, the assigned IP address will be visible in UART console: . | User can verify the connectivity status by pinging pic32mzw1 from laptop or mobile to respective interface ( ethernet or wifi ) : | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html#running-the-application",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html#running-the-application"
  },"70": {
    "doc": "Wi-Fi ethernet dual interface",
    "title": "Wi-Fi ethernet dual interface",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html",
    "relUrl": "/apps/wifi_eth_dual_interface/readme.html"
  },"71": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Over The Air (OTA) Programming Using Wi-Fi",
    "content": "This example application acts as a Wi-Fi Station(STA) to connect to Access point(AP) and perform OTA application update process to download an image present in the user defined OTA-HTTP server. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#over-the-air-ota-programming-using-wi-fi",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#over-the-air-ota-programming-using-wi-fi"
  },"72": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Description",
    "content": "This application demonstrates how a user can perform OTA application upgrade using Wi-Fi. The user would need to configure the Home AP credentials (like SSID and security items). The Wi-Fi service will use the credentials to connect to the Home AP and acquire an IP address. Once the IP address is obtained application will perform OTA update process. Application will try to connect with the defined server address and download the new image. The downloaded image will be stored in the external flash (sst26vf) initially. Application will reset the device to run new image once image is successfully downloaded from server. When device is reset ,bootloader will try to program the image from the external flash and if programming is successful ,downloaded image from the server will be executed . It is required to build “ota_bootloader” project located in the apps folder of wireless_apps_pic32mzw1_wfi32e01 repo first before building this application as the image of the bootloader application will be used to integrate with the “wifi_ota_app_upgrade” application image. A unified hex file will be built using Hexmate tool and the unified HEX image will be loaded to the device. More details about this can be found in “Running Application” section below. This application uses File System, a harmony component which internally use SPI protocol to place the newly downloaded image to the external flash. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#description",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#description"
  },"73": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "OTA Application framework Architecture",
    "content": "Over the Air (OTA) firmware upgrade feature is designed with a two step process, Image Downloading and Image Programming process. Image Downloading process is done by OTA SERVICE, harmony componet and ota_bootloader will take care of programming process. Abstraction model: . User Application: This is where the customer application logic is built. OTA Service: This layer includes the service level logic implementation. This is a Harmony component which provides certain user configurable parameters(ex- Version, Periodic update check etc.) . Based on user configuration, generated code will be activated with required functionalities. OTA software platform / OTA Core : This is the platform layer that consist of the main OTA logic implementation. When OTA process is triggered , this layer will communicate with the transport layer to connect to OTA server. If new image is available , it will initiate download using transport layer. If successfully downloaded, it will store the new image into the File System . File System : The architecture is designed to provide flexibility for the customer to choose the storage medium (ex- SST26 SPI flash, SD card, USB MSD in host mode etc.). Any medium supported by the Harmony3 file system can be used with the OTA service. Bootloader : This layer consists of the logic to safely program images from the file system (external) into program the program memory (NVM) of the device. At device boot, the bootloader will check if a new image is available in the external image store and transfer it to the NVM. For more details , Please follow documentation provided in link . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#ota-application-framework-architecture",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#ota-application-framework-architecture"
  },"74": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "OTA server JSON manifest",
    "content": "The Application expects the HTTP based OTA server to provide metadata of images available in the server in json format. During update checks, the OTA service will download and parse this manifest file. Each entry in the manifest file should include the following fields : . | Version indicates the application version number. It is a integer value. | URL contains the image path from which the application image can be downloaded. It is a string variable. | Digest contain the SHA256 digest of image to be downloaded. It is a 64 byte string variable an should not include whitespaces . | EraseVer This optional field provides a capability to trigger an erase of an version which was downloaded earlier. Customer may want to remove an image from the image store due to various reason, application with bug, may be one of them. It is a bool variable. | If user configures this field as “true”, OTA serice will delete image version mentioned in “Version” field. | If user configures this field as “false”, OTA service will follow image downwload logic. | . | . Sample JSON . { \\\"ota\\\": [ { \\\"Version\\\": 3, \\\"URL\\\": \\\"http://192.168.43.173:8000/wifi_ota103.bin\\\", \\\"Digest\\\": \\\"745189cbb24b752a0175de1f9d5d61433ba47d89aff5b5a3686f54ca2d5dfb22\\\", \\\"EraseVer\\\": false }, { \\\"Version\\\": 6, \\\"URL\\\": \\\"http://192.168.43.173:8000/wifi_ota100.bin\\\", \\\"Digest\\\": \\\"885189cbb24b7b1a0175deef9d5d61f53c247d89a095b5a3686f54ca2d5dfbaa\\\", \\\"EraseVer\\\": false } ] } . OTA service will download json file from server first when update check process is triggered by application, try to fetch information and proceed further . For more details , Please follow documentation provided in link . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#ota-server-json-manifest",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#ota-server-json-manifest"
  },"75": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "OTA image generation",
    "content": "Please refer Generating OTA image section in usage manual present in link . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#ota-image-generation",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#ota-image-generation"
  },"76": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_ota_app_upgrade/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#downloading-and-building-the-application"
  },"77": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | HTTP server. | python 3.9.0 . | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"78": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Running the Application",
    "content": ". | Open the project “wifi_ota_app_upgrade”. | Demo is configured with the default wifi credentials shown in the image below. To change the configurations launch the Harmony 3 configurator and update the home AP credentials for STA Mode and generate code. | Configuring server url : . | As a part of OTA process device will try to connect to user defined HTTP server. If device is able to connect to server without any error, it will try to fetch json manifest information . | User can use any HTTP server. | User may also use python command to create a local http server using below steps: . | Open command prompt and change driectory to the folder where ota image is present. | Use below python command in command prompt: . python -m http.server 8000 . | . | . | Generate the code using MHC. | To create factory reset image , It is required to integrate the bootloader and ota application image and create a single unified HEX file. To integrate 2 images we can use hexmate tool, which is readily available with MPLABX package as part of the standard installation. To combine the hex files - . | User should load the “ota_bootloader” project located in the apps folder of “wireless_apps_pic32mzw1_wfi32e01” repo and include it into “wifi_ota_app_upgrade” project as a “Loadable” component. For this, right click on the “wifi_ota_app_upgrade” project, click on “properties” and select “ota_bootloader” project. User need to make sure that the steps mentioned in “ota_bootloader” reference document is followed, before this step. | Click on “Apply” button to make the applied changes effective: . | . | It is required to perform a “post-build” step to create ota image with file extension “.bin” (which can be placed in the server and downloaded during OTA process). | All required files for post-build process will be generated (during step 5), mentioned above) automatically in “tools” folder, created inside project folder. | Right click on the “wifi_ota_app_upgrade” project and click on properties. | Select “building”, insert below command and click “OK”: ../../tools/hex2bin/hex2bin.exe . Note: python should be present in the system variable path. For more details , Please follow documentation provided in link . | . | Build and program the application. | Place the .bin image into the HTTP server. The .bin image can be found in below path which is generated during “wifi_ota_app_upgrade” project build : ..\\firmware\\wifi_ota_app_upgrade.X\\\\dist\\\\pic32mz_w1_curiosity_freertos\\\\production . | Connect to a USB to UART converter to UART1 and Open a Terminal application (Ex.:Tera term) on the computer. Configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will connect to the Home AP and print the IP address obtained. | Once IP address is obtained the device will initiate OTA process and try to fetch json manifest content, periodically for every 60 sec . | User should make sure that both HTTP server and the PIC32MZW1 device are part of same wifi network (or connected to same Home AP). | User can see periodical messages as shown in belo screen shot : . | Once image is downloaded successfully, the application will print a message in the console. User need to reset the device to load the new image. | If OTA upgrade fails, user need to reset the device to initiate OTA process again. | During reset, device will check if any newly downloaded image is available in the external flash(sst26vf): . | if yes, bootloader will program new image to program-flash area of the device from external flash. | if programming is successful bootloader will hand over control and application will start executing. | . | if no new image is available then bootloader will hand over control without programming any image and application image already present in the program-flash area will start executing | . | User will come to know if new image is running by checking the version number in console print | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html#running-the-application",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html#running-the-application"
  },"79": {
    "doc": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "title": "Over The Air (OTA) Upgrade Using Wi-Fi",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html",
    "relUrl": "/apps/wifi_ota_app_upgrade/readme.html"
  },"80": {
    "doc": "Paho MQTT Client",
    "title": "Paho MQTT Client",
    "content": "This example application acts as a Paho MQTT Client to connect to unsecured MQTT Broker and publishes and subscribes to data on topics. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html#paho-mqtt-client",
    "relUrl": "/apps/paho_mqtt_client/readme.html#paho-mqtt-client"
  },"81": {
    "doc": "Paho MQTT Client",
    "title": "Description",
    "content": "This application demonstrates how a user can use the Paho MQTT Client to connect to a MQTT Broker. The system shall run in bare metal mode, and the application shall come up in STA mode. User will need to configure the Wi-Fi credentials for the Home AP and MQTT Service. The default application will try to establish MQTT connection with test.mosquitto.org and publishes message to a topic every 30 sec. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html#description",
    "relUrl": "/apps/paho_mqtt_client/readme.html#description"
  },"82": {
    "doc": "Paho MQTT Client",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/paho_mqtt_client/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/paho_mqtt_client/readme.html#downloading-and-building-the-application"
  },"83": {
    "doc": "Paho MQTT Client",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/paho_mqtt_client/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"84": {
    "doc": "Paho MQTT Client",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure home AP credentials for STA Mode. | Currently MQTT Service is configured to run a MQTT Client in unsecured mode to connect to test.mosquitto.org on the mqtt port (1883). In case the user wants to change this config, please make the changes in the MQTT Service Module configurations as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The Board will connect to Home AP and then as per the default MQTT Service configuration, it shall connect to test.mosquitto.org and publish messages on the topic ‘MCHP/Sample/a’ periodically every 30 sec. It also subscribes to topic ‘MCHP/Sample/b’ . | If any third party MQTT Client connects to test.mosquitto.org and subscribes to the topic ‘MCHP/Sample/a’, it will receive the periodic messages being published by the MQTT Client running on thr PIC32MZW1. | In case the Third Party MQTT Client publishes onto the topic ‘MCHP/Sample/b’, the Paho MQTT Client running on the PIC32MZW1 shall receive the messages being published onto this topic since it has subscribed to it. | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html#running-the-application",
    "relUrl": "/apps/paho_mqtt_client/readme.html#running-the-application"
  },"85": {
    "doc": "Paho MQTT Client",
    "title": "Paho MQTT Client",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html",
    "relUrl": "/apps/paho_mqtt_client/readme.html"
  },"86": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Wi-Fi Station(STA) Mode",
    "content": "This example application acts as a Wi-Fi Station(STA) to connect to Access point(AP) and exchange data. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html#wi-fi-stationsta-mode",
    "relUrl": "/apps/wifi_sta/readme.html#wi-fi-stationsta-mode"
  },"87": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Description",
    "content": "This application demonstrates how a user can connect to the Home AP. The user would need to configure the Home AP credentials (like SSID and security items). The Wi-Fi service will use the credentials to connect to the Home AP and acquire an IP address.The default application will try to establish a connection to AP “DEMO_AP” with WPA2 security and password as a “password”. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html#description",
    "relUrl": "/apps/wifi_sta/readme.html#description"
  },"88": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_sta/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | pic32mz_w1_curiosity_freertos.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_sta/readme.html#downloading-and-building-the-application"
  },"89": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | Home AP (Wi-Fi Access Point with internet connection) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_sta/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"90": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Configure home AP credentials for STA Mode. | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device will connect to the Home AP and print the IP address obtained. | From the DUT(Device Under Test), user can ping the Gateway IP address. | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html#running-the-application",
    "relUrl": "/apps/wifi_sta/readme.html#running-the-application"
  },"91": {
    "doc": "Wi-Fi STA (service mode)",
    "title": "Wi-Fi STA (service mode)",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html",
    "relUrl": "/apps/wifi_sta/readme.html"
  },"92": {
    "doc": "WiFi Touch Demo",
    "title": "Wi-Fi Touch Demo",
    "content": "This example application acts as a TCP Server to which a TCP Client can connect and visualize QT7 Touch Xpro data. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#wi-fi-touch-demo",
    "relUrl": "/apps/wifi_touch_demo/readme.html#wi-fi-touch-demo"
  },"93": {
    "doc": "WiFi Touch Demo",
    "title": "Description",
    "content": "This application demonstrates how a user can implement a Wi-Fi based Touch application using PIC32MZ-W1’s internal hardware CVD Controller and QT7 Xplained Pro self-capacitance touch evaluation kit. This demo will start a TCP server on the device which is running in Soft AP Mode. The Application will come up as a Soft AP and wait for a device to connect. Afterwards the TCP Server will start running on it. A python script to emulate a dummy TCP client is provided which will display real-time sensor operations on QT7 Xpro board. ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#description",
    "relUrl": "/apps/wifi_touch_demo/readme.html#description"
  },"94": {
    "doc": "WiFi Touch Demo",
    "title": "Downloading and building the application",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the application within the repository is apps/wifi_touch_demo/firmware . To build the application, refer to the following table and open the project using its IDE. | Project Name | Description | . | PIC32_WFI32E_Curiosity_QT7_Touch_Example.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#downloading-and-building-the-application",
    "relUrl": "/apps/wifi_touch_demo/readme.html#downloading-and-building-the-application"
  },"95": {
    "doc": "WiFi Touch Demo",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect a programmer tools such as SNAP, ICD4, etc to ICSP header. | On the GPIO Header (J207), connect U1RX (PIN 13) and U1TX (PIN 23) to TX and RX pin of any USB to UART converter | QT7 Xpro board is connected to Curiosity Xpro header | Laptop / Mobile (to run the TCP Client on it) | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/wifi_touch_demo/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"96": {
    "doc": "WiFi Touch Demo",
    "title": "Jumper &amp; Resistor Configuration",
    "content": ". | Jumper J211 is connected between pins 2-3 (closer to xpro header) | Jumper J209 is disconnected | Jumpers J301 are disconnected (to use external programmer tool) | Remove resistor R220 as it is pulling up the pin used by the 3rd slider segment on the QT7 | . &lt;img src=\\\"images/hardware_setup.jpg\\\" width=400&gt; . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#jumper--resistor-configuration",
    "relUrl": "/apps/wifi_touch_demo/readme.html#jumper--resistor-configuration"
  },"97": {
    "doc": "WiFi Touch Demo",
    "title": "Running the Application",
    "content": ". | Open the project and launch Harmony3 configurator. | Currently the Wi-Fi Service is configured as AP mode with default values for SSID, Auth type, etc. | . | Currently Net Service is configured to run as TCP Server running on port 5555. In case the user wants to change this config, please make the changes in the Net Service Module as shown below: . | Save configurations and generate code via MHC | Build and program the generated code into the hardware using its IDE | Open the Terminal application (Ex.:Tera term) on the computer | Connect to the “USB to UART” COM port and configure the serial settings as follows: . | Baud : 115200 | Data : 8 Bits | Parity : None | Stop : 1 Bit | Flow Control : None | . | The device shall come up as a Soft AP and then as per the Net Service configuration, the TCP Server shall come up, awaiting a connection from a TCP Client. | Connect a PC to this Soft AP (using params from step 2) and run the provided python script. (tested with Python v3.8 and Windows 10) | Connect the python client giving the server IP (the IP of the Board) and the port as 5555. All operations from the QT7 Xpro board will now be visible real-time on the GUI: | . &lt;img src=\\\"images/sample_demo_run.PNG\\\" width=600&gt; . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#running-the-application",
    "relUrl": "/apps/wifi_touch_demo/readme.html#running-the-application"
  },"98": {
    "doc": "WiFi Touch Demo",
    "title": "Known Issues:",
    "content": " ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html#known-issues",
    "relUrl": "/apps/wifi_touch_demo/readme.html#known-issues"
  },"99": {
    "doc": "WiFi Touch Demo",
    "title": "WiFi Touch Demo",
    "content": " ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html",
    "relUrl": "/apps/wifi_touch_demo/readme.html"
  },"100": {
    "doc": "OTA Boot Loader",
    "title": "OTA Boot Loader",
    "content": "This example acts as loader of OTA image (downloaded using OTA process) to load it to device memory . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html#ota-boot-loader",
    "relUrl": "/apps/ota_bootloader/readme.html#ota-boot-loader"
  },"101": {
    "doc": "OTA Boot Loader",
    "title": "Description",
    "content": "This bootloader project can be used by user to program an image , which is downloaded through OTA process. It is required to build this project first using its IDE (MPLABX) before building OTA application, as the image of the bootloader will be integrated with the “wifi_ota_app_upgrade” application image. During boot-up, bootloader will check if any new valid image available in the external flash. If available, it chooses the newly (latest downloaded) available image in the External flash (sst26vf) and program it to Program-Flash area of the device. Bootloader uses SPI protocol to program the image from the external flash. If there is no new downloaded image present in the external flash, bootloader will handover the control to current application present in the program-flash area and application will start executing. For more details , Please follow documentation provided in link . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html#description",
    "relUrl": "/apps/ota_bootloader/readme.html#description"
  },"102": {
    "doc": "OTA Boot Loader",
    "title": "Downloading and Generating Bootloader Code",
    "content": "To download or clone this application from Github, go to the top level of the repository . Path of the project within the repository is apps/ota_bootloader/firmware. To genearte code, refer to the following table and open the project using its IDE. | Project Name | Description | . | ota_bootloader.X | MPLABX project for PIC32MZ W1 Curiosity Board | . |   |   | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html#downloading-and-generating-bootloader-code",
    "relUrl": "/apps/ota_bootloader/readme.html#downloading-and-generating-bootloader-code"
  },"103": {
    "doc": "OTA Boot Loader",
    "title": "Setting up PIC32MZ W1 Curiosity Board",
    "content": ". | Connect the Debug USB port on the board to the computer using a micro USB cable | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html#setting-up-pic32mz-w1-curiosity-board",
    "relUrl": "/apps/ota_bootloader/readme.html#setting-up-pic32mz-w1-curiosity-board"
  },"104": {
    "doc": "OTA Boot Loader",
    "title": "Generating BootLoader Code",
    "content": ". | Open “ota_bootloader” project and launch Harmony3 configurator. Path of the application within the repository is apps/ota_bootloader/firmware. | Generate code via MHC. | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html#generating-bootloader-code",
    "relUrl": "/apps/ota_bootloader/readme.html#generating-bootloader-code"
  },"105": {
    "doc": "OTA Boot Loader",
    "title": "OTA Boot Loader",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html",
    "relUrl": "/apps/ota_bootloader/readme.html"
  },"106": {
    "doc": "Release notes",
    "title": "Microchip MPLAB® Harmony 3 Release Notes",
    "content": " ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/release_notes.html#microchip-mplab-harmony-3-release-notes",
    "relUrl": "/release_notes.html#microchip-mplab-harmony-3-release-notes"
  },"107": {
    "doc": "Release notes",
    "title": "Harmony 3 Wireless application examples for PIC32MZ W1 family  v3.5.0",
    "content": "Development kit and demo application support . Following table provides number of Wireless examples available for different development kits. | Development Kits | MPLABx applications | . | PIC32MZ W1 Curiosity Board | 17 | . New Features . | Please refer to the system services release notes. | . Bug fixes and Improvements . | Please refer to the system services release notes. | UART3 is now configured to use RK13/RK14 pins. | . Known Issues . | Due to an underlying issue in the DHCP code in net repo, compiler optimization should be turned off in the dual interface project. | L2 Bridging is known to have some instabilities when there is high network data traffic. | . Development Tools . | MPLAB® X IDE v5.50 | MPLAB® X IDE plug-ins: . | MPLAB® Harmony Configurator (MHC) v3.8.0 | . | MPLAB® XC32 C/C++ Compiler v3.01 | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/release_notes.html#harmony-3-wireless-application-examples-for-pic32mz-w1-family--v350",
    "relUrl": "/release_notes.html#harmony-3-wireless-application-examples-for-pic32mz-w1-family--v350"
  },"108": {
    "doc": "Release notes",
    "title": "Harmony 3 Wireless application examples for PIC32MZ W1 family  v3.4.1",
    "content": "Development kit and demo application support . Following table provides number of Wireless examples available for different development kits. | Development Kits | MPLABx applications | . | PIC32MZ W1 Curiosity Board | 17 | . New Features . | Split wireless apps into a seperate repo | . Bug fixes . | None | . Known Issues . | None | . Development Tools . | MPLAB® X IDE v5.40 | MPLAB® X IDE plug-ins: . | MPLAB® Harmony Configurator (MHC) v3.6.2 | . | MPLAB® XC32 C/C++ Compiler v2.50 | . ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/release_notes.html#harmony-3-wireless-application-examples-for-pic32mz-w1-family--v341",
    "relUrl": "/release_notes.html#harmony-3-wireless-application-examples-for-pic32mz-w1-family--v341"
  },"109": {
    "doc": "Release notes",
    "title": "Release notes",
    "content": ". ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/release_notes.html",
    "relUrl": "/release_notes.html"
  },"110": {
    "doc": "Harmony 3 Wireless application examples for PIC32MZ W1 family",
    "title": "Harmony 3 Wireless application examples for PIC32MZ W1 family",
    "content": "[![MCHP](https://www.microchip.com/ResourcePackages/Microchip/assets/dist/images/logo.png)](https://www.microchip.com) # Harmony 3 Wireless application examples for PIC32MZ W1 family MPLAB® Harmony 3 is an extension of the MPLAB® ecosystem for creating embedded firmware solutions for Microchip 32-bit SAM and PIC® microcontroller and microprocessor devices. Refer to the following links for more information. - [Microchip 32-bit MCUs](https://www.microchip.com/design-centers/32-bit) - [Microchip 32-bit MPUs](https://www.microchip.com/design-centers/32-bit-mpus) - [Microchip MPLAB X IDE](https://www.microchip.com/mplab/mplab-x-ide) - [Microchip MPLAB® Harmony](https://www.microchip.com/mplab/mplab-harmony) - [Microchip MPLAB® Harmony Pages](https://microchip-mplab-harmony.github.io/) This repository contains the MPLAB® Harmony 3 Wireless application examples for PIC32MZ W1 family - [Release Notes](/wireless_apps_pic32mzw1_wfi32e01/release_notes.html) - [MPLAB® Harmony License](/wireless_apps_pic32mzw1_wfi32e01/mplab_harmony_license.html) To clone or download these applications from Github, go to the [main page of this repository](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01) and then click **Clone** button to clone this repository or download as zip file. This content can also be downloaded using content manager by following these [instructions](https://github.com/Microchip-MPLAB-Harmony/contentmanager/wiki). ## Contents Summary | Folder | Description | --- | --- | apps | Contains Wireless service example applications. | docs | Contains documentation in html format for offline viewing (to be used only after cloning this repository onto a local machine). Use [github pages](https://microchip-mplab-harmony.github.io/wireless_apps_pic32mzw1_wfi32e01/) of this repository for viewing it online. | ## Code Examples The following applications are provided to demonstrate the typical or interesting usage models of one or more wireless usecases. | Name | Description | ---- | ----------- |[OTA Bootloader](/wireless_apps_pic32mzw1_wfi32e01/apps/ota_bootloader/readme.html)|OTA bootloader for PIC32MZW1 devices|[Wi-Fi OTA upgrade](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ota_app_upgrade/readme.html)|Wi-Fi OTA image upgrade demo|[Paho MQTT Client](/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_client/readme.html)|Paho MQTT client example application|[Paho MQTT TLS Client](/wireless_apps_pic32mzw1_wfi32e01/apps/paho_mqtt_tls_client/readme.html)|Paho MQTT TLS client example application|[TCP Client](/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_client/readme.html)|TCP Client example application|[TCP Server](/wireless_apps_pic32mzw1_wfi32e01/apps/tcp_server/readme.html)|TCP Server example application|[UDP Client](/wireless_apps_pic32mzw1_wfi32e01/apps/udp_client/readme.html)|UDP Client example application|[UDP Server](/wireless_apps_pic32mzw1_wfi32e01/apps/udp_server/readme.html)|UDP Server example application|[Weather Client](/wireless_apps_pic32mzw1_wfi32e01/apps/weather_client/readme.html)|Weather Client example application|[Wi-Fi Soft AP (driver mode)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/config_driver_readme.html)|Wi-Fi Soft AP driver mode example aplication|[Wi-Fi Soft AP (service mode)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_ap/readme.html)|WiFi Soft AP service mode example|[Wi-Fi easy config (bare metal)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html)|Bare metal HTTP server / web-page based Wi-Fi provisioning example.|[Wi-Fi easy config (RTOS)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_easy_config/readme.html)|RTOS based HTTP server / web-page based Wi-Fi provisioning example.|[Dual interface (Wi-Fi + Ethernet)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_eth_dual_interface/readme.html)|Dual interface example application with Wi-Fi and Ethernet interfaces|[Wi-Fi STA (driver mode)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/config_driver_readme.html)|Wi-Fi STA driver mode example aplication|[Wi-Fi STA (service mode)](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_sta/readme.html)|WiFi STA service mode example|[Wi-Fi touch demo](/wireless_apps_pic32mzw1_wfi32e01/apps/wifi_touch_demo/readme.html)|Wi-Fi + CVD based touch demo| ____ [![License](https://img.shields.io/badge/license-Harmony%20license-orange.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/blob/master/mplab_harmony_license.md) [![Latest release](https://img.shields.io/github/release/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/releases/latest) [![Latest release date](https://img.shields.io/github/release-date/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/releases/latest) [![Commit activity](https://img.shields.io/github/commit-activity/y/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg)](https://github.com/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01/graphs/commit-activity) [![Contributors](https://img.shields.io/github/contributors-anon/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg)]() ____ [![Follow us on Youtube](https://img.shields.io/badge/Youtube-Follow%20us%20on%20Youtube-red.svg)](https://www.youtube.com/user/MicrochipTechnology) [![Follow us on LinkedIn](https://img.shields.io/badge/LinkedIn-Follow%20us%20on%20LinkedIn-blue.svg)](https://www.linkedin.com/company/microchip-technology) [![Follow us on Facebook](https://img.shields.io/badge/Facebook-Follow%20us%20on%20Facebook-blue.svg)](https://www.facebook.com/microchiptechnology/) [![Follow us on Twitter](https://img.shields.io/twitter/follow/MicrochipTech.svg?style=social)](https://twitter.com/MicrochipTech) [![](https://img.shields.io/github/stars/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg?style=social)]() [![](https://img.shields.io/github/watchers/Microchip-MPLAB-Harmony/wireless_apps_pic32mzw1_wfi32e01.svg?style=social)]() ",
    "url": "http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/",
    "relUrl": "/"
  }
}
`;
var data_for_search

var repo_name = "wireless_apps_pic32mzw1_wfi32e01";
var doc_folder_name = "docs";
var localhost_path = "http://localhost:4000/";
var home_index_string = "Harmony 3 Wireless application examples for PIC32MZ W1 family";

(function (jtd, undefined) {

// Event handling

jtd.addEvent = function(el, type, handler) {
  if (el.attachEvent) el.attachEvent('on'+type, handler); else el.addEventListener(type, handler);
}
jtd.removeEvent = function(el, type, handler) {
  if (el.detachEvent) el.detachEvent('on'+type, handler); else el.removeEventListener(type, handler);
}
jtd.onReady = function(ready) {
  // in case the document is already rendered
  if (document.readyState!='loading') ready();
  // modern browsers
  else if (document.addEventListener) document.addEventListener('DOMContentLoaded', ready);
  // IE <= 8
  else document.attachEvent('onreadystatechange', function(){
      if (document.readyState=='complete') ready();
  });
}

// Show/hide mobile menu

function initNav() {
  jtd.addEvent(document, 'click', function(e){
    var target = e.target;
    while (target && !(target.classList && target.classList.contains('nav-list-expander'))) {
      target = target.parentNode;
    }
    if (target) {
      e.preventDefault();
      target.parentNode.classList.toggle('active');
    }
  });

  const siteNav = document.getElementById('site-nav');
  const mainHeader = document.getElementById('main-header');
  const menuButton = document.getElementById('menu-button');

  jtd.addEvent(menuButton, 'click', function(e){
    e.preventDefault();

    if (menuButton.classList.toggle('nav-open')) {
      siteNav.classList.add('nav-open');
      mainHeader.classList.add('nav-open');
    } else {
      siteNav.classList.remove('nav-open');
      mainHeader.classList.remove('nav-open');
    }
  });
}
// Site search

function initSearch() {

    data_for_search = JSON.parse(myVariable);
    lunr.tokenizer.separator = /[\s/]+/

    var index = lunr(function () {
        this.ref('id');
        this.field('title', { boost: 200 });
        this.field('content', { boost: 2 });
        this.field('url');
        this.metadataWhitelist = ['position']

        var location = document.location.pathname;
        var path = location.substring(0, location.lastIndexOf("/"));
        var directoryName = path.substring(path.lastIndexOf("/")+1);

        var cur_path_from_repo = path.substring(path.lastIndexOf(repo_name));

        // Decrement depth by 2 as HTML files are placed in repo_name/doc_folder_name
        var cur_depth_from_doc_folder = (cur_path_from_repo.split("/").length - 2);

        var rel_path_to_doc_folder = "";

        if (cur_depth_from_doc_folder == 0) {
            rel_path_to_doc_folder = "./"
        }
        else {
            for (var i = 0; i < cur_depth_from_doc_folder; i++)
            {
                rel_path_to_doc_folder = rel_path_to_doc_folder + "../"
            }
        }

        for (var i in data_for_search) {

            data_for_search[i].url = data_for_search[i].url.replace(localhost_path + repo_name, rel_path_to_doc_folder);

            if (data_for_search[i].title == home_index_string)
            {
                data_for_search[i].url = data_for_search[i].url + "index.html"
            }

            this.add({
                id: i,
                title: data_for_search[i].title,
                content: data_for_search[i].content,
                url: data_for_search[i].url
            });
        }
    });

    searchLoaded(index, data_for_search);
}function searchLoaded(index, docs) {
  var index = index;
  var docs = docs;
  var searchInput = document.getElementById('search-input');
  var searchResults = document.getElementById('search-results');
  var mainHeader = document.getElementById('main-header');
  var currentInput;
  var currentSearchIndex = 0;

  function showSearch() {
    document.documentElement.classList.add('search-active');
  }

  function hideSearch() {
    document.documentElement.classList.remove('search-active');
  }

  function update() {
    currentSearchIndex++;

    var input = searchInput.value;
    if (input === '') {
      hideSearch();
    } else {
      showSearch();
      // scroll search input into view, workaround for iOS Safari
      window.scroll(0, -1);
      setTimeout(function(){ window.scroll(0, 0); }, 0);
    }
    if (input === currentInput) {
      return;
    }
    currentInput = input;
    searchResults.innerHTML = '';
    if (input === '') {
      return;
    }

    var results = index.query(function (query) {
      var tokens = lunr.tokenizer(input)
      query.term(tokens, {
        boost: 10
      });
      query.term(tokens, {
        wildcard: lunr.Query.wildcard.TRAILING
      });
    });

    if ((results.length == 0) && (input.length > 2)) {
      var tokens = lunr.tokenizer(input).filter(function(token, i) {
        return token.str.length < 20;
      })
      if (tokens.length > 0) {
        results = index.query(function (query) {
          query.term(tokens, {
            editDistance: Math.round(Math.sqrt(input.length / 2 - 1))
          });
        });
      }
    }

    if (results.length == 0) {
      var noResultsDiv = document.createElement('div');
      noResultsDiv.classList.add('search-no-result');
      noResultsDiv.innerText = 'No results found';
      searchResults.appendChild(noResultsDiv);

    } else {
      var resultsList = document.createElement('ul');
      resultsList.classList.add('search-results-list');
      searchResults.appendChild(resultsList);

      addResults(resultsList, results, 0, 10, 100, currentSearchIndex);
    }

    function addResults(resultsList, results, start, batchSize, batchMillis, searchIndex) {
      if (searchIndex != currentSearchIndex) {
        return;
      }
      for (var i = start; i < (start + batchSize); i++) {
        if (i == results.length) {
          return;
        }
        addResult(resultsList, results[i]);
      }
      setTimeout(function() {
        addResults(resultsList, results, start + batchSize, batchSize, batchMillis, searchIndex);
      }, batchMillis);
    }

    function addResult(resultsList, result) {
      var doc = docs[result.ref];

      var resultsListItem = document.createElement('li');
      resultsListItem.classList.add('search-results-list-item');
      resultsList.appendChild(resultsListItem);

      var resultLink = document.createElement('a');
      resultLink.classList.add('search-result');
      resultLink.setAttribute('href', doc.url);
      resultsListItem.appendChild(resultLink);

      var resultTitle = document.createElement('div');
      resultTitle.classList.add('search-result-title');
      resultLink.appendChild(resultTitle);

      var resultDoc = document.createElement('div');
      resultDoc.classList.add('search-result-doc');
      resultDoc.innerHTML = '<svg viewBox="0 0 24 24" class="search-result-icon"><use xlink:href="#svg-doc"></use></svg>';
      resultTitle.appendChild(resultDoc);

      var resultDocTitle = document.createElement('div');
      resultDocTitle.classList.add('search-result-doc-title');
      resultDocTitle.innerHTML = doc.doc;
      resultDoc.appendChild(resultDocTitle);
      var resultDocOrSection = resultDocTitle;

      if (doc.doc != doc.title) {
        resultDoc.classList.add('search-result-doc-parent');
        var resultSection = document.createElement('div');
        resultSection.classList.add('search-result-section');
        resultSection.innerHTML = doc.title;
        resultTitle.appendChild(resultSection);
        resultDocOrSection = resultSection;
      }

      var metadata = result.matchData.metadata;
      var titlePositions = [];
      var contentPositions = [];
      for (var j in metadata) {
        var meta = metadata[j];
        if (meta.title) {
          var positions = meta.title.position;
          for (var k in positions) {
            titlePositions.push(positions[k]);
          }
        }
        if (meta.content) {
          var positions = meta.content.position;
          for (var k in positions) {
            var position = positions[k];
            var previewStart = position[0];
            var previewEnd = position[0] + position[1];
            var ellipsesBefore = true;
            var ellipsesAfter = true;
            for (var k = 0; k < 5; k++) {
              var nextSpace = doc.content.lastIndexOf(' ', previewStart - 2);
              var nextDot = doc.content.lastIndexOf('. ', previewStart - 2);
              if ((nextDot >= 0) && (nextDot > nextSpace)) {
                previewStart = nextDot + 1;
                ellipsesBefore = false;
                break;
              }
              if (nextSpace < 0) {
                previewStart = 0;
                ellipsesBefore = false;
                break;
              }
              previewStart = nextSpace + 1;
            }
            for (var k = 0; k < 10; k++) {
              var nextSpace = doc.content.indexOf(' ', previewEnd + 1);
              var nextDot = doc.content.indexOf('. ', previewEnd + 1);
              if ((nextDot >= 0) && (nextDot < nextSpace)) {
                previewEnd = nextDot;
                ellipsesAfter = false;
                break;
              }
              if (nextSpace < 0) {
                previewEnd = doc.content.length;
                ellipsesAfter = false;
                break;
              }
              previewEnd = nextSpace;
            }
            contentPositions.push({
              highlight: position,
              previewStart: previewStart, previewEnd: previewEnd,
              ellipsesBefore: ellipsesBefore, ellipsesAfter: ellipsesAfter
            });
          }
        }
      }

      if (titlePositions.length > 0) {
        titlePositions.sort(function(p1, p2){ return p1[0] - p2[0] });
        resultDocOrSection.innerHTML = '';
        addHighlightedText(resultDocOrSection, doc.title, 0, doc.title.length, titlePositions);
      }

      if (contentPositions.length > 0) {
        contentPositions.sort(function(p1, p2){ return p1.highlight[0] - p2.highlight[0] });
        var contentPosition = contentPositions[0];
        var previewPosition = {
          highlight: [contentPosition.highlight],
          previewStart: contentPosition.previewStart, previewEnd: contentPosition.previewEnd,
          ellipsesBefore: contentPosition.ellipsesBefore, ellipsesAfter: contentPosition.ellipsesAfter
        };
        var previewPositions = [previewPosition];
        for (var j = 1; j < contentPositions.length; j++) {
          contentPosition = contentPositions[j];
          if (previewPosition.previewEnd < contentPosition.previewStart) {
            previewPosition = {
              highlight: [contentPosition.highlight],
              previewStart: contentPosition.previewStart, previewEnd: contentPosition.previewEnd,
              ellipsesBefore: contentPosition.ellipsesBefore, ellipsesAfter: contentPosition.ellipsesAfter
            }
            previewPositions.push(previewPosition);
          } else {
            previewPosition.highlight.push(contentPosition.highlight);
            previewPosition.previewEnd = contentPosition.previewEnd;
            previewPosition.ellipsesAfter = contentPosition.ellipsesAfter;
          }
        }

        var resultPreviews = document.createElement('div');
        resultPreviews.classList.add('search-result-previews');
        resultLink.appendChild(resultPreviews);

        var content = doc.content;
        for (var j = 0; j < Math.min(previewPositions.length, 3); j++) {
          var position = previewPositions[j];

          var resultPreview = document.createElement('div');
          resultPreview.classList.add('search-result-preview');
          resultPreviews.appendChild(resultPreview);

          if (position.ellipsesBefore) {
            resultPreview.appendChild(document.createTextNode('... '));
          }
          addHighlightedText(resultPreview, content, position.previewStart, position.previewEnd, position.highlight);
          if (position.ellipsesAfter) {
            resultPreview.appendChild(document.createTextNode(' ...'));
          }
        }
      }
      var resultRelUrl = document.createElement('span');
      resultRelUrl.classList.add('search-result-rel-url');
      resultRelUrl.innerText = doc.relUrl;
      resultTitle.appendChild(resultRelUrl);
    }

    function addHighlightedText(parent, text, start, end, positions) {
      var index = start;
      for (var i in positions) {
        var position = positions[i];
        var span = document.createElement('span');
        span.innerHTML = text.substring(index, position[0]);
        parent.appendChild(span);
        index = position[0] + position[1];
        var highlight = document.createElement('span');
        highlight.classList.add('search-result-highlight');
        highlight.innerHTML = text.substring(position[0], index);
        parent.appendChild(highlight);
      }
      var span = document.createElement('span');
      span.innerHTML = text.substring(index, end);
      parent.appendChild(span);
    }
  }

  jtd.addEvent(searchInput, 'focus', function(){
    setTimeout(update, 0);
  });

  jtd.addEvent(searchInput, 'keyup', function(e){
    switch (e.keyCode) {
      case 27: // When esc key is pressed, hide the results and clear the field
        searchInput.value = '';
        break;
      case 38: // arrow up
      case 40: // arrow down
      case 13: // enter
        e.preventDefault();
        return;
    }
    update();
  });

  jtd.addEvent(searchInput, 'keydown', function(e){
    switch (e.keyCode) {
      case 38: // arrow up
        e.preventDefault();
        var active = document.querySelector('.search-result.active');
        if (active) {
          active.classList.remove('active');
          if (active.parentElement.previousSibling) {
            var previous = active.parentElement.previousSibling.querySelector('.search-result');
            previous.classList.add('active');
          }
        }
        return;
      case 40: // arrow down
        e.preventDefault();
        var active = document.querySelector('.search-result.active');
        if (active) {
          if (active.parentElement.nextSibling) {
            var next = active.parentElement.nextSibling.querySelector('.search-result');
            active.classList.remove('active');
            next.classList.add('active');
          }
        } else {
          var next = document.querySelector('.search-result');
          if (next) {
            next.classList.add('active');
          }
        }
        return;
      case 13: // enter
        e.preventDefault();
        var active = document.querySelector('.search-result.active');
        if (active) {
          active.click();
        } else {
          var first = document.querySelector('.search-result');
          if (first) {
            first.click();
          }
        }
        return;
    }
  });

  jtd.addEvent(document, 'click', function(e){
    if (e.target != searchInput) {
      hideSearch();
    }
  });
}

// Switch theme

jtd.getTheme = function() {
  var cssFileHref = document.querySelector('[rel="stylesheet"]').getAttribute('href');
  return cssFileHref.substring(cssFileHref.lastIndexOf('-') + 1, cssFileHref.length - 4);
}

jtd.setTheme = function(theme) {
  var cssFile = document.querySelector('[rel="stylesheet"]');
  cssFile.setAttribute('href', 'http://localhost:4000/wireless_apps_pic32mzw1_wfi32e01/assets/css/just-the-docs-' + theme + '.css');
}

// Document ready

jtd.onReady(function(){
  initNav();
  initSearch();
});

})(window.jtd = window.jtd || {});


