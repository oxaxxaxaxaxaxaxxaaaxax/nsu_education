#include <sys/types.h> //pid
#include <fcntl.h>//ocreat owronly
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdint.h>

#define PATH_SIZE 1024
#define PAGE_SIZE 8
#define V_PAGE_SIZE 4096
#define MAPS_SIZE 4096

void printPIDInfo(char* filePath, int desk,long long int startAdr,long long int endAdr){
    long int pagemapInfo;
    for (unsigned long i = startAdr; i < endAdr; i += V_PAGE_SIZE) {
        unsigned long offset = i / V_PAGE_SIZE;
        long int pagePtr = offset * PAGE_SIZE;
        int rflag = pread(desk,&pagemapInfo, PAGE_SIZE,pagePtr);
        if((rflag ==-1)){
            printf("read ERROR");
            return;
        }
        if(rflag ==0){
            printf("end of file");
            return;
        }
        int flag = pagemapInfo& (1ULL<< 63)!=0;
        if(flag == 1){
            printf("RAM FLAG: %d\n", flag);
            long int infBits = pagemapInfo& (1ULL << 55) - 1;
            printf("Bits: %ld\n", infBits);
        }else{
            printf("PAGE NOT IN RAM\n");
            //страница не в рам
        }
    }
}


int main(int argc, char** argv){
    char *endptr;
    long int pid = strtol(argv[1], &endptr, 10);
    printf("%ld\n", pid);
    char* filePath = malloc(1024);
    int bytes = snprintf(filePath, PATH_SIZE, "/proc/%ld/pagemap", pid);//в функции самостоятельно добавится '\0'
    if(bytes>= PATH_SIZE){
        //буфер слишком маленький для аткой строки
    }
    char* filePathMaps = malloc(1024);
    bytes = snprintf(filePathMaps, PATH_SIZE, "/proc/%ld/maps", pid);
    printf("%s\n", filePathMaps);
    FILE* file = fopen(filePathMaps, "r");
    if(file == NULL){
        perror("Cannot open file\n");
    }
    int desk = open(filePath, O_RDONLY);
    if (desk == -1) {
        printf("%s\n", filePath);
        perror("ERROR DESK OPEN\n");
        return 0;
    }
    char* mapsBuff = malloc(MAPS_SIZE);
    long long int startAdr;
    long long int endAdr;
    while(fgets(mapsBuff, MAPS_SIZE, file)!= NULL){
        if(sscanf(mapsBuff, "%llx-%llx", &startAdr, &endAdr)!=2){
            printf("ERROR SCANF\n");
        }
        printPIDInfo(filePath,desk,startAdr,endAdr);
    }
    close(desk);
    return 0;
}

//off_t     st_size;     // Размер файла в байтах
//Младшие 55 битов содержат номер физической страницы (PFN, Page Frame Number).