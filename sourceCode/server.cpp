/*
x86_64-w64-mingw32-g++ server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/  -lssl -lcrypto -lws2_32 -lcrypt32
*/

// windows headers
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <winsock.h>
#include <pthread.h>

// openssl headers
#include <openssl/ssl.h>
#include <openssl/err.h>


// SSL functions 
static SSL_CTX* create_context(){
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = TLS_server_method();

    ctx = SSL_CTX_new(method);

    if(ctx == NULL){
        printf("[!] Unable to create SSL context...\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }

    return ctx;
}

static void configure_server_context(SSL_CTX *ctx){
    if(SSL_CTX_use_certificate_chain_file(ctx, "./certs/cert.pem") <= 0){
        printf("[!] Failed to use cert.pem file...\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, "./certs/key.pem", SSL_FILETYPE_PEM) <= 0){
        printf("[!] Failed to use key.pem file...\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        printf("[!] Private key does not match the public certificate...\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
}

struct SSLStruct{
    SSL *ssl;
};

DWORD WINAPI t1(void* data){

    // grabbing ssl instance from data 
    SSLStruct* sslInstance = (SSLStruct*)data;
    SSL *ssl = sslInstance->ssl;

    char rxbuf[128];
    size_t rxcap = sizeof(rxbuf);
    int rxlen;

    while(true){
        if((rxlen = SSL_read(ssl, rxbuf, rxcap)) >= 0){
            
            if(rxlen == 0 || strncmp(rxbuf, "kill", 4) == 0){
                printf("[!] Client closed the connection...\n");
                break;
            }
            else{
                rxbuf[rxlen] = 0; // Null byte
                fflush(stdout); 
                printf("\r[*] Message: %s\n", rxbuf);
                fflush(stdout);
                printf(">>");
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]){

     // User inputs 
    char *server_ip ;
    int port = -1;

    // Check user inputs 
    if(argc != 3){
        printf("[!] Incorrect number of arguments entered\nCorrect Usage: client.exe <IP> <PORT>\n\t IP = IP address of the interface you'd like to bind to\n\t PORT = Port number that you want listen for connections on\n[*] Goodbye...\n");
        exit(0);
    }
    else{
        server_ip = argv[1] ;
        int port = strtol(argv[2], NULL, 10);                       
    }


    // Variables
    int optval = 1;
    int result ; 

    SSL_CTX *ssl_ctx = NULL; 
    SSLStruct sslStr;

    SOCKET server ;
    SOCKET client ; 

    static volatile bool server_running = true; 

    WSADATA wsaData;

    char message[40];


    HANDLE hThread;
    DWORD dwThreadID;

    // Start the server context
    ssl_ctx = create_context();
    configure_server_context(ssl_ctx);

    // Initialize WSA startup
    int startupResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (startupResult != 0){
            printf("[!] WSAStartup Failed...\n");
            printf("[*] Reason: %i", WSAGetLastError());
            WSACleanup();
            exit(0);
        }

    // Create a socket
    int sok = socket(AF_INET, SOCK_STREAM, 0);
    if (sok < 0){
        printf("[!] Socket failed to initalize...\n");
        printf("[*] Reason: %i", WSAGetLastError());
        exit(0);
    }

    // Bind details 
    struct sockaddr_in ipInfo;
    int ipInfoLen = sizeof(ipInfo);
    ipInfo.sin_family = AF_INET;
    ipInfo.sin_port = htons(port);
    ipInfo.sin_addr.s_addr = inet_addr(server_ip);

    // Set sock options
    if(setsockopt(sok, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) < 0){
        printf("[!] Failed to set sock options...\n");
        exit(0);
    }

    // Bind to port and interface 
    if (bind(sok, (struct sockaddr*) &ipInfo, sizeof(ipInfo)) < 0 ){
        printf("[!] Failed to bind...\n");
        exit(0);
    }

    // Start listening for incoming connections
    if(listen(sok, 1) < 0){
        printf("[!] Failed to start listening...\n");
        exit(0);
    }

    while(server_running){
        
        printf("[*] Listening for connections...\n");

        // wait for tcp connect from client 
        client = accept(sok, (struct sockaddr*) &ipInfo, &ipInfoLen);
            if(client < 0){
                printf("[!] Unable to accept client connection...\n");
                printf("[*] Reason: %i", WSAGetLastError());
                exit(0);
            }

        printf("[*] Client connection accepted...\n");

        // create new SSL structure with the new client
        sslStr.ssl = SSL_new(ssl_ctx);
        if(!SSL_set_fd(sslStr.ssl, client)){
            printf("[!] Unable to create new SSL structure with client\n");
            ERR_print_errors_fp(stderr);
            exit(0);
        }

        // wait for client to connect through SSL
        
        if (SSL_accept(sslStr.ssl) <= 0){
            printf("[!] Client unable to accept SSL connection...\n");
            ERR_print_errors_fp(stderr);
            exit(0);
        }  
        else{
            printf("[*] Client SSL connection accepted...\n");

            // Start the recieve thread
            hThread = CreateThread(NULL, 0, t1, &sslStr, 0, NULL);

            while (true){
            
                printf(">> ");
                fgets(message, sizeof(message), stdin); 

                // Exit upon empty message being sent
                if(strncmp(message, "kill", 4) == 0){
                    printf("[*] kill command recieved, exiting...\n");
                    break;
                }

                if ((result = SSL_write(sslStr.ssl, message, (int)strlen(message))) <= 0){
                    printf("[!] Connection has been closed...\n");
                    ERR_print_errors_fp(stderr);
                    break;
                }

            }
                      
        }
        if(!server_running){
            // Cleanup 
            CloseHandle(hThread);
            SSL_shutdown(sslStr.ssl);
            SSL_free(sslStr.ssl);
            close(client);
        }
         
    }

    printf("[!] Server exiting...\n");

    return 0;
}