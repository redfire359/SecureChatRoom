#!/bin/bash
x86_64-w64-mingw32-gcc sourceCode/server.c -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
x86_64-w64-mingw32-gcc sourceCode/client.c -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
