**Todo**:
- clean up code 
- automate cert generation 
- server handle multiple connections 
- Add more output (who is sending message, etc)

## Simple Chat Messaging Server/Client written in C/C++

This was created in linux and then cross compiled for windows. Build steps are as follows: 
1. Install mingw
` sudo apt install mingw-w64 `


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

# SecureChatRoom
Secure Chat room written in c/c++ 
https://gist.github.com/aa6my/c8e088b0ee3b24a56d4e0ebc974055a4

./Configure mingw64 --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/path/to/your/desired/install/directory

