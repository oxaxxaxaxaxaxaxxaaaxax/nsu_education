#include <dirent.h>
#include <fcntl.h>//ocreat owronly
#include <sys/stat.h>//mkdir
#include <stdio.h>
#include <string.h>
#include <unistd.h>//rmdir unlink() symlink readlink
#include <stdlib.h>


void makeDir(const char* dirName){
    if(mkdir(dirName, 0755) == -1){
        perror("ERROR CREATE DIR\n");
    }
}

void remDir(const char* dirName){
    if (rmdir(dirName) == -1) {
        perror("ERROR REMOVE DIR");
    }
}

void writeDir(const char* dirName){
    DIR * dir = opendir(dirName);
    if(chdir(dirName) == -1){
        printf("NOT CHANGE DIR");
    }
    for(int i=0;;i++){
        struct dirent* fileData = readdir(dir);
        if(fileData == NULL){
            break;
        }
        printf("%s\n", fileData->d_name);
    }
}

void makeFile(const char* fileName){
    int desk = open(fileName, O_CREAT, 0755);
    if(desk == -1){
        perror("ERROR DESK OPEN");
    }
}

void remFile(const char* fileName){
    int flag = unlink(fileName);
    if(flag == -1){
        perror("ERROR CLOSE FILE");
    }
}

void writeFile(const char* fileName){
    int desk= open(fileName, O_RDONLY);
    if( desk== -1){
        printf("ERROR DESK OPEN");
    }
    struct stat fileData;
    if(stat(fileName,&fileData) == -1){
        printf("ERROR WRITE DATA");
    }
    off_t sizeFile = fileData.st_size; //long
    char* buff = malloc((size_t)sizeFile+1);
    if(buff == NULL){
        printf("malloc error");
    }
    if(read(desk, buff, sizeFile)==-1){
        printf("read ERROR");
    }
    buff[sizeFile] = '\0';
    printf("%s\n", buff);
}

void makeSym(const char* fileName){
    int flag = symlink(fileName, "SymlinkFunction");
    if(flag == -1){
        perror("ERROR MAKE SYMLINK");
    }
}

void writeSym(const char* symName){
    char* buff = malloc(256);
    int len = readlink(symName, buff, 256);
    if( len == -1){
        perror("ERROR READ SYMLINK");
    }
    buff[len]= '\0';
    printf("%s\n", buff);
}

void makeHard(const char* fileName){
    int flag = link(fileName, "HardlinkFunction");
    if(flag == -1){
        perror("ERROR MAKE HARDLINK");
    }
}

void writeMode(const char* fileName){
    struct stat fileData;
    if(stat(fileName,&fileData) == -1){
        printf("ERROR WRITE DATA");
        return;
    }
    printf("%o\n", fileData.st_mode);//& 0777
    printf("%d\n", (int)fileData.st_nlink);
}

void changeMode(const char* fileName){
    int flag = chmod(fileName, 0777);
    if(flag == -1){
        perror("ERROR CHANGE MODE");
    }
}

int main(int argc, char** argv){
    const char* command = argv[0];
    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    if(strcmp(command, "./make_dir")==0){
        makeDir(argv[1]);
    }
    if(strcmp(command, "./rem_dir")==0){
        remDir(argv[1]);
    }
    if(strcmp(command, "./write_dir")==0){
        writeDir(argv[1]);
    }
    if(strcmp(command, "./make_file")==0){
        makeFile(argv[1]);
    }
    if(strcmp(command, "./rem_file")==0 || strcmp(command, "./rem_sym")==0 || strcmp(command, "./rem_hard")==0){
        remFile(argv[1]);
    }
    if(strcmp(command, "./write_file")==0 ||strcmp(command, "./write_sym_data")==0 ){
        writeFile(argv[1]);
    }
    if(strcmp(command, "./make_sym")==0){
        makeSym(argv[1]);
    }
    if(strcmp(command, "./write_sym")==0){
        writeSym(argv[1]);
    }
    if(strcmp(command, "./make_hard")==0){
        makeHard(argv[1]);
    }
    if(strcmp(command, "./write_mode")==0){
        writeMode(argv[1]);
    }
    if(strcmp(command, "./change_mode")==0){
        changeMode(argv[1]);
    }
    return 0;
}