#!/usr/bin/env python
################################################################################
# Copyright (C) 2021 released Microchip Technology Inc.  All rights reserved.

# Microchip licenses to you the right to use, modify, copy and distribute
# Software only when embedded on a Microchip microcontroller or digital signal
# controller that is integrated into your product or third party product
#(pursuant to the sublicense terms in the accompanying license agreement).

# You should refer to the license agreement accompanying this Software for
# additional information regarding your rights and obligations.

# SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
# MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
# IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
# CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
# OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
# INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
# CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
# SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
# (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
################################################################################

from string import Template
import binascii
from cryptography import x509
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.backends import default_backend
import sys

def make_sublist_group(lst: list, grp: int) -> list:
    """
    Group list elements into sublists.
    make_sublist_group([1, 2, 3, 4, 5, 6, 7], 3) = [[1, 2, 3], [4, 5, 6], 7]
    """
    return [lst[i:i+grp] for i in range(0, len(lst), grp)]

def do_convension(content: bytes) -> str:
    hexstr = binascii.hexlify(content).decode("UTF-8")
    hexstr = hexstr.upper()
    array = ["0x" + hexstr[i:i + 2] + "" for i in range(0, len(hexstr), 2)]
    array = make_sublist_group(array, 10)
    
    return sum(len(a) for a in array ), "\n".join([", ".join(e) + ", " for e in array])


#Creating a Certificate


with open(sys.argv[1], "rb") as keyfile:
     private_key = serialization.load_pem_public_key(
        keyfile.read(),
        backend=default_backend()
    )
    
key_der = private_key.public_bytes(
        serialization.Encoding.DER,
        serialization.PublicFormat.SubjectPublicKeyInfo
    )
key_length , keyArray=do_convension(key_der)

keyHeaderTemplate=Template("""
/*GENERATED FILE*/
#ifndef _KEY_TEST_H_
#define _KEY_TEST_H_
static const unsigned char pubKey[] =
{
$privKey
};
/* size is $keySize */
static const int sizeof_pubKey = sizeof(pubKey);
#endif /*_KEY_TEST_H_*/
""")

with open('key_header.h', 'w') as file: 
    file.write(keyHeaderTemplate.substitute(privKey=keyArray,keySize=key_length))