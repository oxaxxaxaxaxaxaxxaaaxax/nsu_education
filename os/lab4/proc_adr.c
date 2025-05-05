#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int globVar = 15;
int UnglobStVar;
const int globConstVar = 15;

void printLocal(){
    int locVar = 15;
    static int locStVar = 15;
    const int locConstVar = 15;
    printf("local: %p\n", &locVar);
    printf("local static: %p\n", &locStVar);
    printf("local const: %p\n", &locConstVar);
}

int* createLocVar(){
    int locVar = 64;
    return &locVar;
}

void heapOp() {
    char* buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }
    const char* phrase = "hello world";
    strcpy(buffer, phrase);
    printf("buffer string: %s\n", buffer);
    free(buffer);
    printf("buffer free: %s\n", buffer);
    buffer = malloc(1024);
    if (buffer == NULL) {
        return;
    }
    strcpy(buffer, phrase);
    printf("buffer 2 string: %s\n", buffer);
    char* middle = buffer + 512;
    //free(middle);
    //printf("Buffer 3: %s\n", buffer);
}

void envVar(){
    const char* env = "MY_VAR";
    char* value = getenv("MY_VAR");
    printf("%s \n", value);
    if(setenv("MY_VAR", "SET SUCCESS",1) == -1){
        printf("SETENV");
    }
    value = getenv("MY_VAR");
    printf("%s \n", value);

}

int main(void){
    pid_t pid = getpid();
    printf("PID:%d\n", pid);
    printLocal();
    printf("global: %p\n", &globVar);
    printf("global static: %p\n", &UnglobStVar);
    printf("global const: %p\n", &globConstVar);

    int* ptr = createLocVar();
    printf("Address of local variable: %p\n", ptr);

    heapOp();

    envVar();
    sleep(120);
    return 0;
}