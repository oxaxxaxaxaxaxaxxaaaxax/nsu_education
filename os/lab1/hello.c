#include <stdio.h>

void hello_from_static_lib();

int main(void){
    printf("Hello, World!\n");
    hello_from_static_lib();
    return 0;
}