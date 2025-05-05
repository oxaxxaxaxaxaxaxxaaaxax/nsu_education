#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

double * mat_init(double* a, int n1, int n2){
    a = calloc(n1*n2, sizeof(double));
    return a;
}

void print_result(double* x, int n1,int n2, int* coords, const char* str){
    printf("coord %s:%d %d\n",str, coords[0], coords[1]);
    for(int i =0;i<n1;i++){
        for(int j = 0; j< n2;j++){
            //if(i*n2+j== 24){
            printf("%f ", x[i*n2+j]);
            //}
        }
    }
}

void fill_matrix(double *a, size_t n1, size_t n2){
    for(int i = 0; i< n1;i++){
        for(int j = 0; j< n2;j++){
            a[i*n2 +j] =  1;
            if(i==j){
                a[i*n2 +j] = 2;
            }
        }
    }
    return;
}

void mult_matrix(double* A, double* B, double *C, int N1, int N2, int N3){
    for(int k=0;k<N3;k++){
        for(int j=0;j<N2;j++){
            for(int i=0;i<N1;i++){
                C[i*N3 + k] += A[i*N2 + j] * B[j * N3 + k];
            }
        }
    }
}

int main(int argc, char** argv){
    int N1 =4;
    int N2 =4;
    int N3 =4;
    double* A;
    double*B;
    double* C;
    int dims[2] = {1,2};
    int periods[2] = {0,0};
    int myrank;
    int size;
    int sizeCol;
    int sizeRow;
    double finalTime =0;
    const char* strRecA= "recA";
    const char* strRecB= "recB";
    const char* strA= "A";
    const char* strB= "B";
    const char* strRecC = "recC";
    const char* strC= "C";
    MPI_Init(&argc, &argv);
    double currTime = 0;
    double totalTime = 87654345678;
    double locStart = MPI_Wtime();
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    printf("myrank: %d\n", myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm_card;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0,&comm_card);
    int coords[2];
    MPI_Cart_coords(comm_card, myrank, 2, &coords[0]);
    int dims_col[2] = {0,1};
    int dims_row[2] = {1,0};
    MPI_Comm comm_col;
    MPI_Comm comm_row;
    MPI_Cart_sub(comm_card, dims_col, &comm_col);
    MPI_Cart_sub(comm_card, dims_row, &comm_row);
    MPI_Comm_size(comm_col, &sizeCol);
    MPI_Comm_size(comm_row, &sizeRow);
    printf("size row:%d\n",sizeRow);
    printf("size col:%d\n",sizeCol);
    if((coords[0] == 0) && (coords[1] ==0)){
        A = mat_init(A,N1,N2);
        B = mat_init(B,N2,N3);
        C = mat_init(C,N1,N3);
        fill_matrix(A,N1,N2);
        fill_matrix(B,N2,N3);
    }
    double *recA = calloc(N1 * N2 / (sizeRow),sizeof(double));
    if(coords[1]== 0){
        MPI_Scatter(A,N1*N2/sizeRow,MPI_DOUBLE, recA, N1 * N2 / (sizeRow), MPI_DOUBLE, 0,comm_row);
    }
    MPI_Bcast(recA, N1 * N2 / (sizeRow), MPI_DOUBLE, 0 , comm_col);
    MPI_Datatype col;
    MPI_Type_vector(N1, N3/sizeCol,N3, MPI_DOUBLE,&col);
    MPI_Type_commit(&col);
    double *recB = calloc(N2 * N3 / (sizeCol),sizeof(double));
    
    if(coords[0] == 0 && coords[1] ==0){
        for(int j =0;j<N2*N3;j+=N3){
            for(int k=0;k<N3/sizeCol;k++){
                recB[j/sizeCol +k] = B[j+k];
            }
        }
        for(int i=1;i<sizeCol;i++){
            MPI_Send(&B[i*N3/sizeCol], 1, col, i, 66, comm_col);
        }
    }
    //if(coords[0] == 0 && coords[1] ==1){
    if(coords[0] == 0 && coords[1]!= 0){
        printf("%d\n", myrank);
        MPI_Recv(recB, N2*N3/sizeCol, MPI_DOUBLE, 0, 66, comm_col ,MPI_STATUS_IGNORE);
    }
    MPI_Bcast(recB, N2*N3/sizeCol, MPI_DOUBLE, 0, comm_row);
    double* recC = calloc(N1*N3/(sizeRow*sizeCol),sizeof(double));
    mult_matrix(recA, recB, recC, N1/ (sizeRow), N2 , N3/sizeCol);
    MPI_Datatype col2;
    MPI_Type_vector(N1/sizeRow, N3/sizeCol, N3, MPI_DOUBLE, &col2);
    MPI_Type_commit(&col2);
    if(coords[0] == 0 && coords[1] ==0){
        for(int i=0; i<N1/sizeRow; i++){
            for(int j=0;j<N3/sizeCol;j++){
                C[i*N3 +j] = recC[i*N3/sizeCol +j];
            }
        }
        for(int i =0; i<sizeRow; i++){
            for(int j = 0; j< sizeCol;j++){
                if(i==0 && j==0){
                    continue;
                }
                MPI_Recv(&C[i*N3*N1/sizeRow +j*N3/sizeCol],N1*N3/(sizeRow*sizeCol),col2,i*sizeCol +j ,77, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
    }else{
        for(int i =1; i< size; i++){
            MPI_Send(recC,N1*N3/(sizeRow*sizeCol), MPI_DOUBLE, 0, 77,MPI_COMM_WORLD);
        }
    }
    double locEnd = MPI_Wtime();
    currTime = locEnd - locStart;
    MPI_Reduce(&currTime, &finalTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(myrank == 0){
        print_result(C, N1, N3,&coords[0], strC);
        printf("%lf\n", finalTime);
    }
    MPI_Finalize();
    return 0;
}