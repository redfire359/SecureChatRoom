/*
x86_64-w64-mingw32-g++ sourceCode/client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/ -lssl -lcrypto -lws2_32 -lcrypt32
*/

// windows headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

    method = TLS_client_method();

    ctx = SSL_CTX_new(method);

    if(ctx == NULL){
        printf("[!] Unable to create SSL context\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }

    return ctx;
}

static void configure_client_context(SSL_CTX *ctx){
   
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
   
    // loading self signed certificate 
    if (!SSL_CTX_load_verify_locations(ctx, "./certs/cert.pem", NULL)) {
        printf("[!] Unable to verify location of cert.pem\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
}

// SSL Struct we can use to pass in SSL var to the thread

struct SSLStruct{
    SSL *ssl ;
};  


// Recv thread, will constantly read + print messages
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
                printf("[!] Server closed the connection...\n");
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
        printf("[!] Incorrect number of arguments entered\nCorrect Usage: client.exe <SERVER-IP> <PORT>\n\t SERVER-IP = IP address of the server you'd like to connect to\n\t PORT = Port number that the server has opened for you to connect to\n[*] Goodbye...\n");
        exit(0);
    }
    else{
        server_ip = argv[1] ;
        int port = strtol(argv[2], NULL, 10);                       
    }

    // Declaring Variables 
    SSL_CTX *ssl_ctx = NULL;
    SSLStruct sslStr;

    char message[40];
        int result ;

    WSADATA wsaData;

    struct sockaddr_in ipInfo ;
    int addr_len = sizeof(ipInfo);

    char buffer[1024];
    char *txbuf;

    HANDLE hThread;
    DWORD dwThreadID;

    // Create context for client
    ssl_ctx = create_context();

    printf("[*] Setting up client...\n");

    configure_client_context(ssl_ctx);

    // Initialize WSA startup
    int startupResult = WSAStartup(MAKEWORD(2,2), &wsaData);
        if (startupResult != 0){
            printf("[!] WSAStartup Failed...\n");
            printf("[*] Reason: %i", WSAGetLastError());
            WSACleanup();
            exit(0);
        }

    // Create our socket
    SOCKET client = socket(2, 1, 6);  // socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
        if(client < 0){
            printf("[!] Error creating socket! Result: %i\n", client);
            printf("[*] Reason: %i\n", WSAGetLastError());
            exit(0);
        }

    // Adding address to ipInfo
    ipInfo.sin_family = AF_INET;
    ipInfo.sin_addr.s_addr = inet_addr(server_ip);
    ipInfo.sin_port = htons(port);

    // Initalize the connection
    if(connect(client, (struct sockaddr*) &ipInfo, sizeof(ipInfo)) != 0){
        printf("[!] Unable to initalize TCP connection to the server...\n");
        printf("[*] Reason: %i\n", WSAGetLastError());
        goto exit;
    }
    else{
        printf("[*] Successfully connected to the server!\n");
    }

    // Now that we have connected, lets upgrade to SSL

    // Create the SSL structure 
    sslStr.ssl = SSL_new(ssl_ctx);
    if(!SSL_set_fd(sslStr.ssl, client)){
        printf("[!] Unable to create client SSL structre...\n");
        ERR_print_errors_fp(stderr);
        goto exit;
    }

    SSL_set_tlsext_host_name(sslStr.ssl, server_ip); // set hostname
    if(!SSL_set1_host(sslStr.ssl, server_ip)){     // checking hostname 
        printf("[!] Hostname check failed...\n");
        ERR_print_errors_fp(stderr);
        goto exit;
    }

    // Now we can connect 
    if (SSL_connect(sslStr.ssl) == 1){
        printf("[*] Successful SSL connection innitiated\n");

        // Start recieve thread
        hThread = CreateThread(NULL, 0, t1, &sslStr, 0, NULL);

        // Loop to get messages
        while (true)   {
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

        // Close thread
        CloseHandle(hThread);
    }
    
    printf("[!] Client Exiting...\n");
       

exit: 
    if(sslStr.ssl != NULL){
        SSL_shutdown(sslStr.ssl);
        SSL_free(sslStr.ssl);
    }
    SSL_CTX_free(ssl_ctx);

    if(client != -1){
        close(client);
    }

    return 0; 
}

