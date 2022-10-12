# Author : Vysakh P Pillai
# Date : 18/12/2021
# Version : 1.0
# Description : Generate / Verify ECDSA signature

# Accepts a PEM encoded key and a data file and generates a signature. 
# The signature is the raw 64 bytes (r|s) in base64 encoded format. It is not ASN.1 encoded (DER/PEM)
# If a signature is provided, the tool verifies the signature. 
# A web version of this tool writen using webcrypto APIs can be found at https://vppillai.github.io/cryptoScript/FileSigner.html


def main():
    import base64
    import ecdsa
    import hashlib
    import argparse
    import colorama

    colorama.init(autoreset=True)
    parser = argparse.ArgumentParser(description='Generate or verify ECDSA signature. The signature should/will be in base64(raw) format')
    parser.add_argument('-f', '--file', help='file to sign', required=True)
    parser.add_argument('-k', '--key', help='PEM encoded public or private key', required=True)
    parser.add_argument('-p', '--passphrase', help='passphrase for the private key', required=False)
    # argument to accept base64 encoded signature
    parser.add_argument('-s', '--signature', help='base64 encoded signature', required=False)

    args = parser.parse_args()

    with open(args.file, 'rb') as f:
        data = f.read()

    with open(args.key, 'r') as f:
        key = f.read()

    if key.find('-----BEGIN PUBLIC KEY-----') != -1:
        is_private = False
    else:
        is_private = True

    #check if key is private or public using ecdsa library 
    if is_private:
        # private key
        if args.passphrase:
            key = ecdsa.SigningKey.from_pem(key, passphrase=args.passphrase)
        else:
            key = ecdsa.SigningKey.from_pem(key)
        vKey = key.verifying_key
        #print public key in PEM format
        print(colorama.Fore.GREEN+ colorama.Style.BRIGHT +"\nPublic Key extracted from the Pivate key:")
        print(vKey.to_pem().decode('utf-8'))
    else:
        # public key
        vKey = ecdsa.VerifyingKey.from_pem(key)
    
    if args.signature:
        signature = base64.b64decode(args.signature)
        try:
            vKey.verify(signature, data, hashfunc=hashlib.sha256, sigdecode=ecdsa.util.sigdecode_string)
            print(colorama.Fore.GREEN + colorama.Style.BRIGHT + 'Signature verified\n\n')
        except ecdsa.BadSignatureError:
            print(colorama.Fore.RED + colorama.Style.BRIGHT + 'Signature is invalid\n\n')
    else:
        signature = key.sign(data, hashfunc=hashlib.sha256, sigencode=ecdsa.util.sigencode_string) 
        print(colorama.Fore.GREEN + colorama.Style.BRIGHT +  "Signature generated:")
        print(base64.b64encode(signature).decode('utf-8'))

if __name__ == '__main__':
    main()