/*
x86_64-w64-mingw32-g++ server.cpp -o server.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/  -lssl -lcrypto -lws2_32 -lcrypt32
*/

// windows headers
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <winsock.h>

// openssl headers
#include <openssl/ssl.h>
#include <openssl/err.h>

////////////////// CHANGE THIS //////////////////

int port = 4433;                   

char ipaddress[16] = "192.168.56.7";

////////////////// CHANGE THIS //////////////////

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



int main(){

    // Variables
    int optval = 1;
    int result ; 

    SSL_CTX *ssl_ctx = NULL; 
    SSL *ssl = NULL;

    SOCKET server ;
    SOCKET client ; 

    char rxbuf[128];
    size_t rxcap = sizeof(rxbuf);
    int rxlen;

    static volatile bool server_running = true; 

    WSADATA wsaData;

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
    ipInfo.sin_addr.s_addr = inet_addr(ipaddress);

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
        ssl = SSL_new(ssl_ctx);
        if(!SSL_set_fd(ssl, client)){
            printf("[!] Unable to create new SSL structure with client\n");
            ERR_print_errors_fp(stderr);
            exit(0);
        }

        // wait for client to connect through SSL
        
        if (SSL_accept(ssl) <= 0){
            printf("[!] Client unable to accept SSL connection...\n");
            ERR_print_errors_fp(stderr);
            exit(0);
        }  
        else{
            printf("[*] Client SSL connection accepted...\n");

            // continue to get messages from the connection 
            while (true){
                if((rxlen = SSL_read(ssl, rxbuf, rxcap)) >= 0){
                    if(rxlen == 0){
                        printf("[!] Client closed the connection...\n");
                    }
                    else{
                        /*  Prints out message length if needed
                        printf("[*] SSL_read returned: %d\n", rxlen);
                        */
                        rxbuf[rxlen] = 0; // null byte
                        printf("[*] Message: %s\n", rxbuf);
                    }
                    
                    // kill switch          NOT WORKING RN 
                    if(strcmp(rxbuf, "kill\n") == 0){
                        printf("[!] Server recieved kill signal, shutting off...\n");
                        server_running = false; 
                        break; 
                    }
                }
                
            }
        }
        if(!server_running){
            // Cleanup 
            SSL_shutdown(ssl);
            SSL_free(ssl);
            close(client);
        }
    }

    printf("[!] Server exiting...\n");

    return 0;
}