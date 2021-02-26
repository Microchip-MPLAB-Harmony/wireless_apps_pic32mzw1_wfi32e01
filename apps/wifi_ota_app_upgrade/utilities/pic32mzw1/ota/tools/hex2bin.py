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

def strtoul(str, base):
    if base == None:
        if len(str) >= 3 and str[0:2] == '0x':
            base = 16
        else:
            base = 10

    return int(str, base)


class INTEL_HEX(object):

    RECORD_TYPE_DATA                  = 0x00
    RECORD_TYPE_EOF                   = 0x01
    RECORD_TYPE_EXT_SEGMENT_ADDR      = 0x02
    RECORD_TYPT_START_SEGMENT_ADDR    = 0x03
    RECORD_TYPE_EXT_LINEAR_ADDR       = 0x04
    RECORD_TYPE_START_LINEAR_ADDR     = 0x05

    class recode(object):
        def __init__(self, line):
            self.valid = 1 if line[0] == ':' else 0
            bytes = [int(line[i:i+2], 16) for i in range(1, len(line), 2)]
            self.count   =  bytes[0]
            self.address = (bytes[1] << 8) + bytes[2]
            self.type    =  bytes[3]
            self.data    =  bytes[4:4+self.count]
            self.xsum    =  bytes[-1]
            self.len     = len(self.data)

            if sum(bytes) & 0xFF != 0:
                self.valid = 0
                print(bytes)
                raise()

            if self.type > 5:
                self.valid = 0

        def debug(self):
            print("$G%02x$c%04x$M%02X$C%s$w%02x" % (
                self.count,
                self.address,
                self.type,
                ''.join('{:02x}'.format(x) for x in self.data),
                self.xsum))

    def __init__(self, filename):
        self.parsing = 1
        self.file = open(filename, 'r')
        if self.file == None:
            print("Unable to open file: %s\n" % filename)
            self.parsing = 0

    def __del__(self):
        if self.file != None:
            self.file.close()
            self.file = None
        self.parsing = 0

    def get_record(self):
        r = None
        if self.parsing != 0:
            line = self.file.readline()
            if line  != "":
                r = self.recode(line.rstrip('\n'))

        if r == None or r.valid == 0:
           self.parsing = 0;
           r = None
        return r

def hex2bin(input_file, output_file, upper_limit, lower_limit, trim_tailing_space=0):
    intel_hex = INTEL_HEX(input_file)
    address_hi = 0
    address_highest = 0
    prog = []
    status = 0;

    if upper_limit <= lower_limit:
        print("Invalid Parameter uppler_limit < lower_limit\n");
        return -1;

    for i in range(0, (upper_limit - lower_limit)):
        prog.append(0xff)

    while 1:
        record = intel_hex.get_record()

        if record == None:
            break

        if record.type == INTEL_HEX.RECORD_TYPE_DATA:
            address = address_hi + record.address
            if (lower_limit <= address) and ((address + record.count) <= upper_limit):
                if address_highest < (address + record.count):
                    address_highest = address + record.count
                #print("%08X %s" % (address - lower_limit, ''.join('{:02x}'.format(x) for x in record.data)))
                for i in range(0, record.count):
                    prog[address - lower_limit + i] = record.data[i]

        elif record.type == INTEL_HEX.RECORD_TYPE_EXT_LINEAR_ADDR:
            address_hi = (record.data[0] << 24) | (record.data[1] << 16)

        elif record.type == INTEL_HEX.RECORD_TYPE_EOF:
            break

        else:
            print("Invalid Record Type(%02X)\n" % record.type)
            status = -1

    if status == 0:
        bin = open(output_file, "wb")
        if trim_tailing_space == 0:
            address_highest = upper_limit
        print("Generating %s (%08X ~ %08X %d bytes)\n" %
            (output_file, lower_limit, address_highest, (address_highest - lower_limit)))
        for i in range(0, address_highest - lower_limit):
            bin.write(struct.pack('B', prog[i]))
        bin.close()

    return status;
