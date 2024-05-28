# Secure Chat Messaging Server/Client written in C++

This was created in linux and then cross compiled to run on windows. It is a chat server program secured with OpenSSL and written in C++. 

Build steps are as follows: 

1. Install mingw

` sudo apt install mingw-w64 libssl-dev -y`

2. Download OpenSSL from github

`git clone https://github.com/openssl/openssl.git`

3. Configure OpenSSL for cross compile. (I chose to put the output in /opt/openssl, you can change this)

```
cd openssl
./Configure mingw64 --cross-compile-prefix=x86_64-w64-mingw32- --prefix=/opt/openssl
make
sudo make install
```

4. Now download this repo 

`git clone https://github.com/redfire359/SecureChatRoom.git`

5. Generate your certificates and create a password when prompted. Make sure you put the IP as the machine that will be running `server.exe`

```
cd SecureChatRoom/certs
./GenerateCertificate.sh 127.0.0.1
```

6. Compile the server and client. Ensure the IPs (located right above the main() function) in the source code are the same one you entered for step 5. Also, make sure to change the -I and -L directory in the below command if you changed it in step 3.

```
x86_64-w64-mingw32-g++ sourceCode/client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
x86_64-w64-mingw32-g++ sourceCode/server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
```

7. Execute the server first, then the client. **The .dll files must be in the same directory as the .exe** 

```
server.exe 10.10.10.1 4242  // Server is opening port 4242 
client.exe 10.10.10.1 4242 // Client is connecting to 10.10.10.1 on port 4242
```

7.1 **Optional:** If you dont want to rely on the .dll file, you can also statically compile the .exe's (They will be ~6000 KB bigger however)

```
x86_64-w64-mingw32-g++ server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/  -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic -lws2_32 -lcrypt32
x86_64-w64-mingw32-g++ client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/  -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic -lws2_32 -lcrypt32
```

## Credits 

- All SSL realted stuff implemented from OpenSSL's example code : 

https://github.com/openssl/openssl/blob/master/demos/sslecho/main.c#L139

- Shoutout chatgpt for fixing my cert. 


**Extra/To-Do**:

- clean up code 
- server handle multiple connections 
- Add more output (who is sending message, etc)
    - multithreading 