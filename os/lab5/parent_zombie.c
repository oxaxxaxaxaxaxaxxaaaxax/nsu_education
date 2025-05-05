#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork();

    if (child_pid == -1) {
        perror("fork failed");
        return 1;
    }

    if (child_pid == 0) {
        printf("Child process: PID = %d, Parent PID = %d\n", getpid(), getppid());
        pid_t childchild_pid = fork();
        if (childchild_pid == -1) {
            perror("fork failed");
            return 1;
        }

        if (childchild_pid == 0) {
            printf("childchild process: PID = %d, Parent PID = %d\n", getpid(), getppid());
            sleep(40);
            exit(0);
        } 
        if(childchild_pid >0){
            exit(0);
        }
    } else {
        printf("Parent process: PID = %d\n", getpid());
        sleep(80);
    }

    return 0;
}