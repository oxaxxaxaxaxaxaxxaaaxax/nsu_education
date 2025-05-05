#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "mpi.h"

int* vecInit(int N, int *a){
    a = (int*)malloc(sizeof(int)*N);
    if(a == NULL){
        return NULL;
    }
    return a;
}

void fillVec(int N, int *a, int *b){
    for(int i=0;i<N;i++){
        a[i] = rand()%10;
        //printf("%d ", a[i]);
        b[i] = rand()%10;
        //printf("%d ", b[i]);
    }
}

int calculateResu(int Na, int Nb, int *a, int*b){
    int s =0;
    for (int i=0;i<Na; i++){
        for(int j=0;j<Nb;j++){
            s += a[i] * b[j];
        }
    }

    return s;
}

void printRes(int s){
    printf("%dBBBBBBBBB\n", s);
}

int main(int argc, char **argv){
    int N =130000;
    int myrank, size, len;
    char machine[1024];
    int *a;
    a = vecInit(N,a);
    int *b;
    b = vecInit(N,b);
    int s=0;
    double finalTime = 0;
    
    MPI_Init(&argc, &argv); 
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank);
    MPI_Comm_size( MPI_COMM_WORLD, &size); 
    double totalTime = 87654345678;
    int*recA = malloc(sizeof(int)*N/(size));
    int recS =0;
    double result = 0;
    if(myrank == 0){
        fillVec(N, a, b);
    }
    MPI_Scatter(a, N/size, MPI_INT, recA, N/size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(b, N, MPI_INT, 0 ,MPI_COMM_WORLD);
    for(int i=0;i<3;i++){
        double locStart = MPI_Wtime();
        recS = calculateResu(N/(size), N,recA, b);
        double locEnd = MPI_Wtime();
        result = locEnd - locStart;
        if(totalTime>result){
            totalTime = result;
        }
    }
    printf("%d\n", recS);
    MPI_Reduce(&recS, &s, 1, MPI_LONG_LONG_INT, MPI_SUM, 0 ,MPI_COMM_WORLD);//co ro
    MPI_Reduce(&totalTime, &finalTime,1,MPI_DOUBLE, MPI_MAX,0, MPI_COMM_WORLD);
    if(myrank == 0){
        printf("\n total res: %d\n", s);
        printf("\n%f", finalTime);
    }
    free(recA);
    MPI_Finalize();
    free(a);
    free(b);
    return 0;
}