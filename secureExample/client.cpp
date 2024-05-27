#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

int main(){

    // Declare variables
    WSADATA wsaData; 
    SOCKET sok; 

    int result ;
    struct sockaddr_in ipInfo; 

    char IPaddress[16] = "192.168.56.3";

    char message[40];
    char recvbuf[512];
    int recvbuflen = sizeof(recvbuf);

    // Initialize WSA startup 
    
    result = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (result != 0 ){
        WSACleanup();
        printf("WSA error %i\n", result);
        return 0;
    }

    // Setup socket config
    sok = socket(2,1,6);

    // add ip information 
    ipInfo.sin_port = htons(443);
    int ipInfoSize = sizeof(ipInfo);
    const sockaddr *name = (SOCKADDR *)&ipInfo;
    ipInfo.sin_family = 2;

    ipInfo.sin_addr.s_addr = inet_addr(IPaddress);

    // set sock options
    BOOL bOptVal = TRUE;
    int bOptLen = sizeof (BOOL);

    setsockopt(sok, SOL_SOCKET, SO_KEEPALIVE, (char *) &bOptVal, bOptLen);

    // start the connecton
    connect(sok, name, ipInfoSize);

    // Send message
    do {
        printf("Send: ");
        scanf("%s", &message);

        int res_send = send(sok, message, (int)strlen(message), 0 );

        if(res_send == SOCKET_ERROR){
            printf("Send failed with error: %d\n", WSAGetLastError())   ;
            closesocket(sok);
            WSACleanup();
            return 1;
        }

/*  recieve portion 
        printf("Waiting for a message...\n");
        result = recv(sok, recvbuf, recvbuflen, 0);
                recvbuf[result] = '\0';
        if(result > 0){
            printf("Recieved: %s\n", recvbuf);
        }
*/
    }while ((int)strlen(message) != 1);//change this later


    closesocket(sok);
    WSACleanup();
    return 0;
}