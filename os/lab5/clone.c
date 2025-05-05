#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
//#define REC_SIZE 10
#define FILE_SZ 4096

void hello_func(int rec_sz){
    char buff[14];
    strcpy(buff, "hello world");
    if(rec_sz>0){
        hello_func(rec_sz -1);
    }
}

int child_proc(){
    hello_func(10);
    return 0;
}

int main(void){
    const char *file = "stack";
    int fd = open(file, O_RDWR | O_CREAT, 0666);
    if (fd == -1) {
        perror("FAILED OPEN");
        return 1;
    }
    if (ftruncate(fd, FILE_SZ) == -1) {
        perror("FAILED FTRUNCATE");
        close(fd);
        return 1;
    }
    void *ptr = mmap(NULL, FILE_SZ, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED){
        perror("FAILED MAP");
        close(fd);
        return 1;
    }//выделили регион памяти под стек и адрес начала выделенной памяти передали в clone
    int pid_clone = clone(child_proc, ptr + FILE_SZ, CLONE_VM | SIGCHLD, NULL);
    if(pid_clone == -1){
        perror("FAILED CLONE");
        close(fd);
    }
    if(pid_clone > 0){
        int status;
        int flag = waitpid(pid_clone, &status, 0);
        if (flag == -1) {
            perror("waitpid failed");
            return 1;
        }
    }
    munmap(ptr, FILE_SZ);
    close(fd);
    return 0;
}