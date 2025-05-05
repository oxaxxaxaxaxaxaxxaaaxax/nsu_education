#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = getpid();
    printf("PID: %d\n", pid);
    sleep(5);
    if (execl("/proc/self/exe", "self", NULL) == -1) {
        perror("fail exec");
        return 1;
    }
    printf("Hello world\n");
    return 0;
}