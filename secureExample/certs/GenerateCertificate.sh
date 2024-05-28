#!/bin/bash

openssl req -new -x509 -key key.pem -out cert.pem -days 365 -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=192.168.56.7" -addext "subjectAltName=IP:192.168.56.7"
