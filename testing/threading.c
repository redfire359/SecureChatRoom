#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//x86_64-w64-mingw32-gcc threading.c  -o thread.exe
typedef struct data {
    int x;
    int y;
} DATA, *Pdata;

DWORD WINAPI t1(void* data){
    
int count = 0; 

    while(count < 5){
        count++;
        Sleep(4000) ;
        printf("\nRecv: Hello there\n");
        printf(">>");
    }

    printf("\n THREAD ENDING");
    return 0;
}


int main(){

    HANDLE hThread;
    DWORD dwThreadID;

    hThread = CreateThread(NULL, 0, t1, NULL, 0, &dwThreadID);

    while (1){
        char message[30] ;
        printf("\n>> ");
        fgets(message, sizeof(message), stdin); 

        printf("MSG: %s\n" , message);
        if(strncmp(message, "exit", 4) == 0){

            printf("EXIT\n");
            break;
        }
    }
    
    // Wait for threads 
    WaitForMultipleObjects(1, hThread, TRUE, INFINITE);
    // Free memory alloc and wait
    CloseHandle(hThread);
    return 0;

}

