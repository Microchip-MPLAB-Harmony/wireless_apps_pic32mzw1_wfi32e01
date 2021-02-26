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
import sys
import os
import re
import getopt

from tools import hex2bin
from tools import boot_control
from tools import bin2hex

# post_build.py ${Device} ${ProjectDir} ${ConfName} ${ImagePath} ${ImageDir} ${ImageName} ${IsDebug}

#------------------------------------------------------------------------------#
#                                    main                                      #
#------------------------------------------------------------------------------#
def main(argv):
    Device     = None
    ProjectDir = None
    ConfName   = None
    ImagePath  = None

    try:
        opts, unused = getopt.getopt(
            argv, 'hd:p:c:i:', ['device=', 'projdir=', 'conf=', 'image='])
    except getopt.GetoptError:
        sys.exit(1)

    for opt, arg in opts:
        if opt == '-h':
            sys.exit()
        elif opt in ('-d', '--device'):
            Device = arg
        elif opt in ('-p', '--projdir'):
            ProjectDir = arg
        elif opt in ('-c', '--conf'):
            ConfName = arg
        elif opt in ('-i', '--image'):
            ImagePath = arg

    if not Device or not ProjectDir or not ConfName or not ImagePath:
        sys.exit(-1)

    ImageDir, ImageName = os.path.split(arg)

    print("*******************************************************************")
    print("Device=    : " + Device)
    print("ProjectDir : " + ProjectDir)
    print("ConfName   : " + ConfName)
    print("ImagePath  : " + ImagePath)
    print("ImageDir   : " + ImageDir)
    print("ImageName  : " + ImageName)    #Not used
    print("*******************************************************************")

    production_hex           = os.path.join(ProjectDir, ImagePath)
    production_bin           = os.path.splitext(production_hex)[0] + ".bin"
    production_ota           = os.path.splitext(production_hex)[0] + ".ota"
    production_unified_hex   = os.path.splitext(production_hex)[0] + ".unified.hex"

    boot_control_hex         = os.path.join(ProjectDir, ImageDir, "boot_control.hex")
    boot_control_bin         = os.path.join(ProjectDir, ImageDir, "boot_control.bin")
    boot_control_unified_bin = os.path.join(ProjectDir, ImageDir, "boot_control_unified.bin")
    boot_control_unified_hex = os.path.join(ProjectDir, ImageDir, "boot_control_unified.hex")

    # ----------------------------------------------------------------------- #
    #                    Find Slot Size from OTA_CONFIG.h                     #
    # ----------------------------------------------------------------------- #
    ##ota_config = os.path.join(
    ##                os.path.dirname(os.path.dirname(ProjectDir)),
    ##                "firmware", "src", "config", ConfName, "ota", "ota_config.h")
    
    ota_config = os.path.join(
                    os.path.dirname(os.path.dirname(ProjectDir)),
                    "firmware", "src", "ota", "ota_config.h")

    re_IMAGESTORE_SLOT_SIZE = re.compile("^\s*#define\s+IMAGESTORE_SLOT_SIZE\s+(0x[a-f|A-F|0-9]+)")
    IMAGESTORE_SLOT_SIZE = 0
    with open(ota_config) as file:
        for line in file:
            m = re_IMAGESTORE_SLOT_SIZE.search(line)
            if m:
                IMAGESTORE_SLOT_SIZE = int(m.group(1), 16)
                break
    if IMAGESTORE_SLOT_SIZE == 0:
        printf("ERROR: Can not find IMAGESTORE_SLOT_SIZE")
        raise

    APP_IMG_SLOT_START = 0x10010000
    APP_IMG_SLOT_END   = 0x10010000 + IMAGESTORE_SLOT_SIZE

    # ----------------------------------------------------------------------- #
    #                              OTA Image                                  #
    # ----------------------------------------------------------------------- #
    # Application.X.production.hex -> Application.X.production.bin
    hex2bin.hex2bin(production_hex, production_bin, APP_IMG_SLOT_END, APP_IMG_SLOT_START, 1)

    #boot_control_hex
    boot_control.boot_control(production_bin, boot_control_bin, APP_IMG_SLOT_START, 0)

    #boot_control.bin -> boot_control.hex
    bin2hex.bin2hex(boot_control_bin, boot_control_hex, APP_IMG_SLOT_START)

    #Application.X.production.hex += ota_boot_control.hex
    os.system("hexmate -O%s %s +%s" % (production_hex, production_hex, boot_control_hex))

    #Application.X.production.hex -> ota.bin
    hex2bin.hex2bin(production_hex, production_ota, APP_IMG_SLOT_END, APP_IMG_SLOT_START, 1)

    # ----------------------------------------------------------------------- #
    #                         Factory Reset Image                             #
    # ----------------------------------------------------------------------- #
    # ota_boot_control_unified.hex
    boot_control.boot_control(production_bin, boot_control_unified_bin, APP_IMG_SLOT_START, 1)

    # ota_boot_control_unified.bin -> ota_boot_control_unified.hex
    bin2hex.bin2hex(boot_control_unified_bin, boot_control_unified_hex, APP_IMG_SLOT_START)

    # Application.X.production_unified.hex += ota_boot_control_unified.hex
    os.system("hexmate -O%s %s +%s" % (production_unified_hex, production_unified_hex, boot_control_unified_hex))

#------------------------------------------------------------------------------#
#                                 Entry Point                                  #
#------------------------------------------------------------------------------#
if __name__ == "__main__":
    main(sys.argv[1:])
