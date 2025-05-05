#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE_ARR 24000


int * mat_init(int* a, int n1, int n2){
    a = calloc(n1*n2, sizeof(int));
    return a;
}

void print_result(int* x, int n1,int n2,int myrank,int i){
    printf("myrank:%d\n", myrank);
    printf("i:%d\n", i);
    for(int i =0;i<n1;i++){
        for(int j = 0; j< n2;j++){
            printf("%d ", x[i*n2+j]);
        }
        printf("\n");
    }
    printf("\n");
}

void glider_gen(int* field,int n2){
    field[1]= 1;
    field[n2+2] = 1;
    field[n2*2]= 1;
    field[n2*2+1]= 1;
    field[n2*2+2]= 1;
}

int get_count_neighbors(int x, int y, int* field, int n1,int n2, int offset){
    int count=0;
    for(int i=-1;i<=1;i++){
        for(int j=-1;j<=1;j++){
            if(i==0 && j==0){
                continue;
            }
            int neigbourX = x+i;
            int neigbourY = (y+j+n2)%n2;
            if(field[neigbourX*n2 + neigbourY] == 1){
                count++;
            }
        }

    }
    return count;
}

int is_equals_field(int* old_field, int* new_field, int n1, int n2,int offset){
    for(int i=0;i<n1;i++){
        for(int j=0;j<n2;j++){
            if((old_field+offset)[i*n2 +j]!= (new_field+offset)[i*n2+j]){
                return 0;
            }
        }

    }
    return 1;
}

void calculate_flags(int length,int* flags,int*** process, int n1, int n2, int myrank,int offset){
    int* currentField = process[myrank][length];
    for(int i=0;i<length;i++){
        flags[i] = is_equals_field(process[myrank][i], currentField, n1,n2,offset); 
    }
}



int calculate_cell_state(int old_state, int neighbours){
    if (old_state == 1) {
        if((neighbours < 2) || (neighbours > 3)) {
            return 0;
        }
    }
    else {
        if(neighbours == 3) {
            return 1;
        }
    }
    return old_state;
}

void calculate_next_state(int length,int*** process,int myrank,int* nextField, int n1,int n2,int offset){
    int* currentField = process[myrank][length-1];
    for(int i=0;i<n1;i++){
        for(int j=0;j<n2;j++){
            int count_neighbors = get_count_neighbors(i+offset/n2,j,currentField,n1,n2, offset);
            (nextField+offset)[i*n2+j] = calculate_cell_state((process[myrank][length-1] + offset)[i*n2+j], count_neighbors);
        }
    }
    process[myrank][length] = nextField;
}

void compare_flags(int colls, int rows, int*flag_vec, int idx,int myrank,int* return_flag){
    int count = idx/colls;
    int mod = idx%colls;
    if(mod>0){
        count++;
    }
    if(myrank >= count){
        return;
    }
    int compare_counter =0;
    for(int j=0;j<colls;j++){
        compare_counter =0;
        for(int i=0;i<rows;i++){
            if(flag_vec[i*colls+j]==0){
                break;
            }else{
                compare_counter++;
            }
        }
        if(compare_counter == rows){
            *return_flag =1;
        }
    }

}

int main(int argc, char** argv){
    int N1=6;
    int N2=6;
    int* field;
    int myrank;
    int size = strtol(argv[1],NULL,10);
    printf("%d\n",size);
    int*** process = malloc(sizeof(int**)*size);
    for(int i=0;i<size;i++){
        process[i] = malloc(sizeof(int*)*SIZE_ARR);
    }
    double finalTime =0;
    MPI_Init(&argc, &argv);
    int return_flag =0;
    int final_flag =0;
    double currTime =0;
    double locStart = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    printf("myrank: %d\n", myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int *sendcnts = malloc(sizeof(int)*size);
    int *displs =  malloc(sizeof(int)*size);
    if(myrank == 0){
        field = mat_init(field,N1,N2);
        glider_gen(field,N2);
    }
    int countRows = N1/size;
    int additionRows = N1 - countRows*size;
    int localdispls =0;
    for(int i=0;i<size;i++){
        if(additionRows>0){
            sendcnts[i] = (countRows +1)*N2;
            additionRows --;
        }else{
            sendcnts[i] = countRows*N2; 
        }
        displs[i] = localdispls;
        localdispls+=sendcnts[i];
    }
    int* recField = calloc(sendcnts[myrank]+2*N2, sizeof(int));
    MPI_Scatterv(field,sendcnts,displs,MPI_INT,recField+N2,sendcnts[myrank],MPI_INT, 0, MPI_COMM_WORLD);
    process[myrank][0] = recField;
    int* flags = calloc(SIZE_ARR,sizeof(int));
    int* recBuff = calloc(SIZE_ARR, sizeof(int));
    for(int i=0; i< SIZE_ARR; i++){
        int* newField = calloc(sendcnts[myrank]+2*N2,sizeof(int));
        MPI_Request request1;
        MPI_Request request2;
        MPI_Request request3;
        MPI_Request request4;
        MPI_Request request5;
        MPI_Isend(process[myrank][i]+N2, N2, MPI_INT, (myrank-1+size)%size, 66, MPI_COMM_WORLD, &request1);
        MPI_Isend(process[myrank][i]+sendcnts[myrank], N2, MPI_INT, (myrank+1)%size, 77, MPI_COMM_WORLD, &request2);
        MPI_Irecv(process[myrank][i], N2, MPI_INT,(myrank-1+size)%size ,77,MPI_COMM_WORLD, &request3);
        MPI_Irecv(process[myrank][i]+N2+sendcnts[myrank], N2, MPI_INT, (myrank+1)%size, 66, MPI_COMM_WORLD, &request4);
        if(i!= 0){
            calculate_flags(i,flags,process,sendcnts[myrank]/N2,N2,myrank,N2);
            MPI_Ialltoall(flags,SIZE_ARR/size,MPI_INT,recBuff,SIZE_ARR/size,MPI_INT,MPI_COMM_WORLD,&request5);
        }
        calculate_next_state(i+1,process,myrank,newField,(sendcnts[myrank]/N2)-2,N2,N2+N2);
        MPI_Wait(&request1, MPI_STATUS_IGNORE);
        MPI_Wait(&request3, MPI_STATUS_IGNORE);
        calculate_next_state(i+1,process,myrank,newField,1,N2,N2);
        MPI_Wait(&request2, MPI_STATUS_IGNORE);
        MPI_Wait(&request4, MPI_STATUS_IGNORE);
        calculate_next_state(i+1,process,myrank,newField,1,N2,sendcnts[myrank]);
        if(i!=0){
            MPI_Wait(&request5, MPI_STATUS_IGNORE);
            compare_flags(SIZE_ARR/size,size,recBuff,i,myrank,&return_flag);
        }
        MPI_Allreduce(&return_flag,&final_flag,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
        if(final_flag == 1){
            break;
        }
    }
    double locEnd = MPI_Wtime();
    currTime = locEnd - locStart;
    MPI_Reduce(&currTime, &finalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(myrank ==0){
        printf("time:%lf\n", finalTime);
    }
    MPI_Finalize();
    return 0;
}
