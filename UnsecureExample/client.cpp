
#include <windows.h>
#include <winsock2.h>

int main (){

    // Declare variables
    WSADATA wsaData; 
    SOCKET sok; 

    int result ;
    struct sockaddr_in ipInfo; 

    char IPaddress[16] = "192.168.56.3";

    const char *message = "Hello world";
    char recvbuf[1024];
    int recvbuflen = sizeof(recvbuf);

    // Initialize WSA startup 
    
    result = WSAStartup(MAKEWORK(2,2), &wsaData);
    if (result != 0 ){
        WSACleanup();
        printf("WSA error %i\n", result);
        return 0;
    }

    // Setup socket config
    sok = socket(2,1,6);

    // add ip information 
    ipInfo.sin_port = htons(5252);
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
        cout << "Message: ";
        cin >> message; 
        send(sok, message, (int)strlen(message), 0 );

        result = recv(sok, recvbuf, recvbuflen, 0);
        
        if(result > 0){
            printf("Recieved: %d\n", result)
        }

    }while (result > 0);


    closeSocket(sok);
    WSACleanup();
    return 0;
}