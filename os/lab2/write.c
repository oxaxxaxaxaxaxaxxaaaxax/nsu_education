#include <unistd.h> 

int main(void){
    const char *message = "Hello, World!\n";
    if( write(1, message, 14) == 14){
        //ok
    }
}