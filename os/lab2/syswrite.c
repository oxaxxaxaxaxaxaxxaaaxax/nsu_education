#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>

int main() {
    const char * message = "Hello, world!\n";
    syscall(SYS_write, 1, message, strlen(message)); //возвращает колво байт 
    return 0;
}