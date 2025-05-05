#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
static int globalVar = 42;

int main(void){
    static int localVar = 10;
    printf("globVaraddr:%p\n", &globalVar);
    printf("localVaraddr:%p\n", &localVar);
    printf("globVar:%d\n", globalVar);
    printf("localVar:%d\n", localVar);
    pid_t pid_ = getpid();
    printf("PID:%d\n", pid_);
    pid_t pid = fork();
    if(pid == -1){
        printf("ERROR CREATE PROCESS");
    }
    if(pid ==0){
        pid_t child = getpid();
        pid_t parent = getppid();
        printf("Child PID:%d Parent PID:%d\n", child, parent);
        printf("child process: globVaraddr:%p\n", &globalVar);
        printf("child process: localVaraddr:%p\n", &localVar);
        printf("child process: globVar:%d\n", globalVar);
        printf("child process: localVar:%d\n", localVar);
        sleep(15);
        localVar =100;
        globalVar = 1000;
        printf("changed child process: globVaraddr:%p\n", &globalVar);
        printf("changed child process: localVaraddr:%p\n", &localVar);
        printf("changed child process: globVar:%d\n", globalVar);
        printf("changed child process: localVar:%d\n", localVar);
        sleep(15);
        exit(5);
    }
    if(pid>0){
        sleep(10);
        printf("parent process: globVar:%d\n", globalVar);
        printf("parent process: localVar:%d\n", localVar);
        sleep(30);
        int status;
        int flag = waitpid(pid, &status, 0);
        if (flag == -1) {
            perror("waitpid failed");
            return 1;
        }
        if (WIFEXITED(status)) {
            printf("EXIT %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("SIGNAL %d\n", WTERMSIG(status));
        } else if(WIFSTOPPED(status)){
            printf("STOP %d\n", WSTOPSIG(status));
        }
    }
}