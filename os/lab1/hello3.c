#include <stdio.h>
#include <dlfcn.h>

int main(){
    void* ptr = dlopen("./libruntimehello.so", RTLD_LAZY);//символы разрешаются при первом использовании
    if(ptr == NULL){
        return 1;
    }
    void (*func)() = dlsym(ptr, "hello_from_dyn_runtime_lib");
    if(func == NULL){
        return 1;
    }
    func();
    dlclose(ptr);
    //hello_from_dyn_runtime_lib();
    return 0;
}