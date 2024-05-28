#!/bin/bash

if [ "$1" == "" ]; then
    echo -e "\n[!] Usage: \n\t./GenerateCertificate.sh <SERVER-IP>\n"
    exit

else
    openssl req -new -x509 -key key.pem -out cert.pem -days 365 -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=$1" -addext "subjectAltName=DNS:DESKTOP-53G6UUS,IP:$1"
    echo -e "\n[*] Done..."
    echo -e "[*] Reminder: if you move the certs out of this directory, make sure you change the path in the source code :)\n"
    ls -aspl key.pem cert.pem
fi