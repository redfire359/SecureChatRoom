#!/bin/bash
x86_64-w64-mingw32-g++ sourceCode/server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32 ; x86_64-w64-mingw32-g++ sourceCode/client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
