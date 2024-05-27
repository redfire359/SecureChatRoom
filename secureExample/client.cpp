/*
x86_64-w64-mingw32-g++ client.cpp -o client.exe -I/opt/openssl/include/ -L/opt/openssl/lib64/  -Wl,-Bstatic -lssl -lcrypto -Wl,-Bdynamic -lws2_32 -lcrypt32
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
    if (!SSL_CTX_load_verify_locations(ctx, "cert.pem", NULL)) {
        printf("[!] Unable to verify location of cert.pem\n");
        ERR_print_errors_fp(stderr);
        exit(0);
    }
}

int main(){
    
    // Some Variables 
    SSL_CTX *ssl_ctx = NULL;
    SSL *ssl = NULL;

///////may not need this section ///////////// 
    char buffer[1024];
    char *txbuf;

    char rxbuf[128];
    size_t rxcap = sizeof(rxbuf);
    int rxlen;
/////////////////////////////
    char server_ip[16] = "192.168.56.3";    // CHANGE THIS 
    int port = 4343;                        // CHANGE THIS 

    char message[40];
        int result ;

    struct sockaddr_in ipInfo ;
    int addr_len = sizeof(ipInfo);

    //Create context for client
    ssl_ctx = create_context();

    printf("[*] Setting up client...\n");

    configure_client_context(ssl_ctx);

    // Create our socket
    int client = socket(2, 1, 6);  // socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
        if(client < 0){
            printf("[!] Error creating socket! Result: %i\n", client);
            exit(0);
        }

    // Adding address to ipInfo
    ipInfo.sin_family = AF_INET;
    ipInfo.sin_addr.s_addr = inet_addr(server_ip);
    ipInfo.sin_port = htons(port);

    // Initalize the connection
    if(connect(client, (struct sockaddr*) &ipInfo, sizeof(ipInfo)) != 0){
        printf("[!] Unable to initalize TCP connection to the server...\n");
        goto exit;
    }
    else{
        printf("[*] Successfully connected to the server!\n");
    }

    // Now that we have connected, lets upgrade to SSL

    // Create the SSL structure 
    ssl = SSL_new(ssl_ctx);
    if(!SSL_set_fd(ssl, client)){
        printf("[!] Unable to create client SSL structre...\n");
        ERR_print_errors_fp(stderr);
        goto exit;
    }

    SSL_set_tlsext_host_name(ssl, server_ip); // set hostname
    if(!SSL_set1_host(ssl, server_ip)){     // checking hostname 
        printf("[!] Hostname check failed...\n");
        ERR_print_errors_fp(stderr);
        goto exit;
    }

    // Now we can connect 
    if (SSL_connect(ssl) == 1){
        printf("[*] Successful SSL connection innitiated\n");

        // Keep sending messages
        while (true)   {
            printf(">> ");
            scanf("%s", &message);

            //exit upon empty message being sent
            if(message == NULL || message[0] == '\n'){
                break;
            }

            if ((result = SSL_write(ssl, message, (int)strlen(message))) <= 0){
                printf("[!] Connection has been closed...\n");
                ERR_print_errors_fp(stderr);
                break;
            }

            //Can add read here later if we want
        }
    }

    printf("[!] Client Exiting...\n");

exit: 
    if(ssl != NULL){
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
    SSL_CTX_free(ssl_ctx);

    if(client != -1){
        close(client);
    }

    return 0; 
}

