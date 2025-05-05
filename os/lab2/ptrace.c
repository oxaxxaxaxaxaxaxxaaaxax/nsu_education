#include <unistd.h>
#include <sys/types.h> //pid_t
//#include <stdlib.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/user.h>

int main(void){
    pid_t pid  = fork();
    if(pid == -1){//не создал
        perror("FORK FAILED");
    }
    if(pid == 0){//мы в дочернем процессе
        //long ptrace(enum __ptrace_request request, pid_t pid, void *addr, void *data);
        ptrace(PTRACE_TRACEME,0,NULL,NULL);//все параментры в режиме TRACEME игнорируются
        if (execl("/home/oksana/OS/lab2/syswrite", "syswrite", NULL) == -1) {
            perror("fail exec");
            return 1;
        }
    }
    if(pid >0){//в родительском процессе
        int status;
        while(1){
            int flag = waitpid(pid,&status,0);//игнорируем options
            if(flag ==-1){
                perror("error waitpid()\n");
                break;
            }
            if(WIFEXITED(status)){
                printf("process exited\n");
                break;
            }
            if(WIFSTOPPED(status)){
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                long syscall_number = regs.orig_rax;
                printf("Process stopped at a system call: %ld\n", syscall_number);
            }
            ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
        }
    }
    return 0;
}



