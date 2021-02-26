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
import sys

def strtoul(str, base):
    if base == None:
        if len(str) >= 3 and str[0:2] == '0x':
            base = 16
        else:
            base = 10

    return int(str, base)


def _bin2hex(data, address=0):
    def record(address, record_type, data):
        xsum = len(data) + (address>>8) + address  + record_type + sum(data)
        xsum = 0x100 - xsum & 0xFF
        return ':' + '%02X%04X%02X' % (len(data), address & 0xFFFF, record_type & 0xFF) + ''.join('{:02X}'.format(x) for x in data) +  '%02X'%(xsum)

    def data_record(offset, data):
        return record(offset, 0, data)

    def address_record(address):
        return record(0, 4, [(address>>24)&0xFF, (address>>16)&0xFF])

    def eof():
        return ":00000001FF"

    lines = []
    n = 0
    m = 16
    offset = address & 0xFFFF
    lines.append(address_record(address))

    while n < len(data):
        if m > len(data):
            m = len(data);
        line = data_record(offset, data[n:m])
        lines.append(line)
        offset += 16

        if offset > 0x10000:
            address = address & 0xFFFF + offset;
            lines.append(address_record(address))
            offset &= 0xFFFF
        n += 16
        m += 16
    lines.append(eof())
    return '\n'.join(lines)

def bin2hex(input_file, output_file, address=0):
    data = []
    with open(input_file, mode='rb') as file:
        if sys.version_info[0] == 2:
            for i in file.read():
                data.append(ord(i))
        else:
            for i in file.read():
                data.append(i)
    
    lines = _bin2hex(data, address)

    with open(output_file, mode='w') as file:
        file.write(lines)
