#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


//x86_64-w64-mingw32-gcc threading.c  -o thread.exe
typedef struct data {
    int x;
    int y;
} DATA, *Pdata;

DWORD WINAPI t1(void* data){
    

    for(int i = 0 ; i < 10000; i++){
        printf("X", i);
    }

    return 0;

}


int main(){

    HANDLE hThread;
    DWORD dwThreadID;

    hThread = CreateThread(NULL, 0, t1, NULL, 0, &dwThreadID);

    for (int i = 0 ; i < 10000; i++){
        printf("O");
    }
    // Wait for threads 
    WaitForMultipleObjects(1, hThread, TRUE, INFINITE);
    // Free memory alloc and wait
    CloseHandle(hThread);
    return 0;

}

