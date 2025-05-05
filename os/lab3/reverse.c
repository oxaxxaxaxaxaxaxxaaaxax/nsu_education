#include <dirent.h>
#include <fcntl.h>//ocreat owronly
#include <unistd.h>//chdir
#include <libgen.h>//basename
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

const char* getName(const char *dirPath){
    char *pathCopy = malloc(strlen(dirPath) + 1);//0
    strcpy(pathCopy, dirPath);
    return basename(pathCopy);
}

char* reverseString(const char *str)
{
    int len = strlen(str);
    char* rev_str = malloc(len+1);
    for (size_t i = 0; i < len; i++)
    {
        rev_str[i] = str[len-i-1];
    }
    rev_str[len] ='\0';
    return rev_str;
}

void reverseDirName(const char *originDirPath){

}

int isRegularFile(char* fileName){
    struct stat fileData;
    if(stat(fileName,&fileData) == -1){
        printf("ERROR WRITE");
        perror("err");
    }
    return S_ISREG(fileData.st_mode) != 0; 
}

void revFile(char* fileName, const char* revDirName){
    char* revFileName = reverseString(fileName);
    int lenRev = strlen(revFileName);
    for(int i=0;i<lenRev;i++){
        printf("%c ",revFileName[i]);
    }
    int deskOrigin = open(fileName, O_RDONLY);
    if( deskOrigin== -1){
        printf("ERROR DESK OPEN");
    }
    struct stat fileData;
    if(stat(fileName,&fileData) == -1){
        printf("ERROR WRITE DATA");
    }
    off_t sizeFile = fileData.st_size; //long
    char* buff = malloc((size_t)sizeFile+1);
    if(read(deskOrigin, buff, sizeFile)==-1){
        printf("read ERROR");
    }
    buff[sizeFile] = '\0';
    char* revData = reverseString(buff);
    if(chdir("..") == -1){
        printf("NOT CHANGE DIR1");
    }
    if(chdir(revDirName) == -1){
        printf("NOT CHANGE DIR2");
    }
    int deskRev = open(revFileName,O_CREAT|O_WRONLY, 0755);
    if(deskRev == -1){
        printf("ERROR DESK OPEN");
    }
    if(write(deskRev, revData, sizeFile) == -1){
        printf("write ERROR");
    }

}

int main(int argc , char** argv){
    const char* originDirPath = argv[1];
    const char* originName = getName(originDirPath);
    const char* reverseName = reverseString(originName);
    mkdir(reverseName, 0755);
    DIR * dir = opendir(originName);
    if(chdir(originName) == -1){
        printf("NOT CHANGE DIR");
    }
    if(dir == NULL){
        printf("ERROR OPEN DIR");
    }
    //struct dirent *readdir(DIR *dirp);
    for(int i=0;;i++){
        struct dirent* fileData = readdir(dir);
        if(fileData == NULL){
            break;
        }
        if(!isRegularFile(fileData->d_name)){
            continue;
        }
        revFile(fileData->d_name, reverseName);
        if(chdir("..") == -1){
            printf("NOT CHANGE DIR1");
        }
        if(chdir(originName) == -1){
            printf("NOT CHANGE DIR2");
        }
    }
    if(closedir(dir) == -1){
        printf("ERROR CLOSE DIR");
    }
    return 0;
}