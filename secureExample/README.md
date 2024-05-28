Todo :
- clean up code 
- automate cert generation 
- server handle multiple connections 
- Add more output (who is sending message, etc)



All SSL realted stuff implemented from OpenSSL's example code : 

https://github.com/openssl/openssl/blob/master/demos/sslecho/main.c#L139



Steps to generate self signed cert/pem key files 
(You can skip all values besides 'common name', enter 'localhost' or something like that)

```
openssl req -newkey rsa:4096 -x509 -sha256 -days 3650 -nodes -out cert.pem -keyout key.pem

openssl req -new -x509 -key key.pem -out cert.pem -days 365 -subj "/C=US/ST=State/L=City/O=Organization/OU=Unit/CN=localhost" -addext "subjectAltName=DNS:localhost,IP:127.0.0.1"


```

To compile 

```
x86_64-w64-mingw32-g++ sourceCode/client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
x86_64-w64-mingw32-g++ sourceCode/server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
```