#include <unistd.h>

const char message[] = "Hello, world!\n";

int main()
{
    asm volatile (
        "movq $1, %%rax\n"        
        "movq $1, %%rdi\n"        
        "lea message(%%rip), %%rsi\n"  
        "movq $14, %%rdx\n"          
        "syscall\n"
        :
        : 
        : "rax", "rdi", "rsi", "rdx"
    );

    return 0;
}