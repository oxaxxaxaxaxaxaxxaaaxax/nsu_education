#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define REC_COUNT 35
#define NUM_PAGES 10
#define PAGE_SIZE 4096

void* reg = NULL;
size_t size = NUM_PAGES * PAGE_SIZE;

void allocateMemory(int recComp){
    if(recComp <= 0){
        return;
    }
    printf("REC_COUNT: %d\n", recComp);
    char arr[4096];
    sleep(1);
    allocateMemory(recComp-1);
}

void handleSignal(int signal){
    
    if(signal == SIGSEGV){
        printf("MY SEGMENTATION FAULT\n");
        if (mprotect(reg, size, PROT_READ | PROT_WRITE |PROT_EXEC) == -1){
            perror("ERROR MAP IN SEG");
            _exit(1);
            return ;
        }
    }
}

int main(void){
    char** pointers = malloc(sizeof(char*)*10);
    pid_t pid = getpid();
    printf("PID: %d\n", pid);
    sleep(10);
    allocateMemory(REC_COUNT);
    char* arr2;
    for(int i=0;i<10;i++){
        sleep(1);
        pointers[i] = malloc(1024*1024*4);
    }
    for(int i=0;i<0;i++){
        free(pointers[i]);
    }
    free(pointers);
    struct sigaction sa;
    //void *memset(void *ptr, int value, size_t num);
    memset(&sa,0,sizeof(sa));
    void (*sa_ptr)(int) = &handleSignal;
    sa.sa_handler = sa_ptr;
    //int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
    if(sigaction(SIGSEGV,&sa,NULL)==-1){
        perror("ERROR REGISTRATION");
    }

    size_t size = NUM_PAGES * PAGE_SIZE;
    reg = mmap(NULL, size,PROT_READ | PROT_WRITE |PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE,-1,0);//Память не связана с файлом и инициализируется нулями. Создаёт приватную копию памяти. Изменения видны только этому процессу
    if(reg == MAP_FAILED){
        perror("ERROR MAP");
        return 1;
    }
    sleep(5);
    if(mprotect(reg,size,PROT_NONE)==-1){
        perror("ERROR MPROT READ");
    }
    sleep(5);
    char val = *(char*)reg;
    printf("Correct read\n");
    sleep(5);
    if(mprotect(reg,size,PROT_NONE)==-1){
        perror("ERROR WRITE");
    }
    sleep(5);
    *(char*)reg = '7';
    printf("Correct write\n");
    *(char*)reg = val;
    sleep(10);
    int lengthPage = 3* PAGE_SIZE;
    void * start = (char*)reg + 4*PAGE_SIZE;
    if(munmap(start,lengthPage) ==-1){
        perror("ERROR FREE PAGES");
    }
    sleep(10);
    //int munmap(void *addr, size_t length);
}
