#!/usr/bin/env python
################################################################################
# Copyright 2020 Microchip Technology Inc. and its subsidiaries.  You may use this
# software and any derivatives exclusively with Microchip products.
#
# THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER EXPRESS,
# IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES
# OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE, OR
# ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR
# USE IN ANY APPLICATION.
#
# IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
# INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER
# RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF
# THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT ALLOWED
# BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS
# SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY
# TO MICROCHIP FOR THIS SOFTWARE.
#
# MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
# TERMS.
################################################################################
import struct
import hashlib
import sys

def firmware_image_header(img, boot_addr, factory_reset):
    hdr = bytearray(256)

    for i in range(0, len(hdr)):
        hdr[i] = 0xFF;
    #status
    #hdr[0] = F8

    # Version = 1
    hdr[1]  = 0x01

    # sz
    sz = len(img)
    hdr[4] = (sz) & 0xFF
    hdr[5] = (sz >> 8) & 0xFF
    hdr[6] = (sz >> 16)& 0xFF
    hdr[7] = (sz >> 24)& 0xFF

    # boot address
    hdr[12] = (boot_addr )      & 0xFF
    hdr[13] = (boot_addr >> 8)  & 0xFF
    hdr[14] = (boot_addr >> 16) & 0xFF
    hdr[15] = (boot_addr >> 24) & 0xFF

    img[4095] = 0xFF;

    sha256 = hashlib.sha256()
    sha256.update(hdr)
    sha256.update(img[256:])
    digest = sha256.digest()

    if factory_reset:
        slot = 0
        hdr[0]  = 0xF8
        hdr[2]  = 0
        hdr[3]  = 3
        hdr[8]  = (slot)       & 0xFF
        hdr[9]  = (slot >> 8 ) & 0xFF
        hdr[10] = (slot >> 16) & 0xFF
        hdr[11] = (slot >> 24) & 0xFF

    for i in range(0, len(digest)):
        hdr[16+i] = digest[i]
    return hdr

def boot_control(input_file, output_file, app_img_slot_addr, factory_reset):
    boot_addr = (app_img_slot_addr + 4096) | 0xB0000000

    img = bytearray()
    with open(input_file, mode='rb') as file:
        if sys.version_info[0] == 2:
            for i in file.read():
                img.append(ord(i))
        else:
            for i in file.read():
                img.append(i)
        
    hdr = firmware_image_header(img, boot_addr, factory_reset)

    with open(output_file, mode='wb') as file:
        for b in hdr:
            file.write(struct.pack('B', b))
