#include <math.h>
#include "mpi.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>



double * mat_init(double* a, int n){
    a =(double*) calloc(n*n, sizeof(double));
    return a;
}

double * vec_init(double* a, int n){
    a = (double*)calloc(n, sizeof(double));
    return a;
}

void print_matrix(double* a, size_t na, size_t nb){
    for(int j=0;j<na;j++){
        for(int i=0;i<nb;i++){
            printf("%lf ",a[j*na +i]); 
        }
        printf("\n");
    }
    printf("\n");
}


void print_vec(double* a, size_t n){
    for(int j=0;j<n;j++){
        //printf("%lf ",a[j]); 
    }
    //printf("\n");
}

void fill_matrix(double *a, size_t n){
    //printf("\nmatrix:\n");
    for(int i = 0; i< n;i++){
        for(int j = 0; j< n;j++){
            a[i*n +j] = 0.0001;
            if(i==j){
                a[i*n +j] = i%n*2.0;
            }
        }
    }
    //print_matrix(a,n,n);
    return;
}


double* mult_mat_vec(double* res, double* vec, double *A, int na, int nb){
    //print_matrix(A,na, nb);
    //print_vec(vec,nb);
    for(int i=0;i<na;i++){
        res[i] =0;
        for(int j=0;j<nb;j++){
            res[i]+= A[nb*i+j] * vec[j]; 
        }
    }
    return res;
}

void fill_b(double* b, double* A, size_t n){
    for(int i = 0; i< n;i++){
        b[i] = i;
        //printf("%lf ", u[i]);
    }
}

// void fill_b(double* b, double* A, size_t n){
//     //printf("u:\n");
//     double *u ;
//     u = vec_init(u, n);
//     for(int i = 0; i< 10;i++){
//         u[i] = sin(2*3.14*i/n);
//         printf("u:%lf ", u[i]);
//     }
//     //printf("u:%lf ", u[5]);
//     //printf("\n");
//     b = mult_mat_vec(b, u,A, n, n);
//     //printf("\nb:\n");
//     //print_vec(b, n);

// }

void fill_x(double *x, int n){
    //printf("\nx:\n");
    for(int i = 0; i< n;i++){
        x[i] = 0;
        //printf("%lf ", x[i]);
    }
    //printf("\n");
}

double calculate_scal_pr(double* a, double* b, int n){
    double res=0;
    for(int i=0;i<n;i++){
        //printf("a: %lf\n", a[i]);
        //printf("b: %lf\n", b[i]);
        res+= a[i]*b[i];
    }
    return res;
}

double* calculate_yn(double* Axn, double *yn, double* b, double* A, double* x,int na, int nb, int size, int myrank){
    Axn = mult_mat_vec(Axn, x,A,na, nb);
    // printf("%d\n", myrank);
    // printf("\nAxn:\n");
    // print_vec(Axn, na);
    for(int i=0;i<na;i++){
        yn[i] = Axn[i]-b[i+ myrank*na];
    }
    // printf("\nyn:\n");
    // print_vec(yn, na);
    return yn;
}

double* calculate_xn1(double* xn1, double* xn, double tn, double* yn, int na , int nb, int myrank){
    for(int i=0;i<na;i++){
        //printf("myrank:%d %lf %lf %lf\n",myrank, xn[i+ myrank*na], yn[i+ myrank*na], tn);
        xn1[i] = xn[i+ myrank*na] - tn*yn[i+ myrank*na];
    }
    return xn1;
}

double calculate_norm(double* yn, int n){
    double norm_1= 0.0;
    //double norm_2 =0.0;
    for(int i=0;i<n;i++){
        norm_1+= yn[i]*yn[i];
        //norm_2+= b[i]*b[i];
    }
    //return (norm_1)/(norm_2);
    return norm_1;
}

void print_result(double* x, int n){
    for(int i =0;i<10;i++){
        printf("%f ", x[i]);
    }
    //printf("%f ", x[5]);
}


int main(int argc, char **argv){
    int N=1008*2;
    int flag_to_exit = 0;
    double *A;
    double *b;
    double *x;
    int myrank, size, len;
    //scanf("%d", &N);
    A = mat_init(A, N);
    b = vec_init(b,N);
    x= vec_init(x,N);
    double* yn;
    double* curr_res; 
    double* xn1;
    double * yn_gen;
    double * xn1_gen;
    double* curr_res_gen;
    double e = 1.0/100000.0;
    int uuy =0;   
    double finalTime=0.0; 
    MPI_Init(&argc, &argv);
    int counter=0;
    double sc1_gen =0.0;
    double sc2_gen=0.0;
    double norm1=0;
    double norm2=0;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double tn =0.0;
    double currTime = 0;
    double totalTime = 87654345678;
    for(int i =0; i<1;i++){
        double locStart = MPI_Wtime();
        double sqrtE = e*e;
        counter =0;
        flag_to_exit =0;
        double*recA =(double*) malloc(sizeof(double)*N*N/(size));
        curr_res = vec_init(curr_res,N/size);
        yn = vec_init(yn,N/size);
        xn1 = vec_init(xn1,N/size);
        yn_gen = vec_init(yn_gen,N);
        xn1_gen = vec_init(xn1_gen,N); 
        curr_res_gen = vec_init(curr_res_gen, N);
        if(myrank == 0){
            fill_matrix(A, N);
            fill_b(b,A,N);
            fill_x(x, N);
        }
        MPI_Scatter(A, N*N/size, MPI_DOUBLE, recA, N*N/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(b, N, MPI_DOUBLE, 0 ,MPI_COMM_WORLD);
        norm2 = calculate_norm(b,N);
        MPI_Bcast(x, N, MPI_DOUBLE, 0 ,MPI_COMM_WORLD);
        for(int i=0;i< 10000;i++){
            printf("iter:\n");
            printf("%d\n", counter);
            //printf("i: %d rank: %d\n", i,myrank);
            //printf("calc yn %d\n", myrank);
            yn = calculate_yn(curr_res,yn,b,recA,x,N/size,N,size, myrank);
            // printf("y print\n");
            // print_vec(yn,2);
            // printf("\n");
            MPI_Allgather(yn, N/size, MPI_DOUBLE, yn_gen, N/size, MPI_DOUBLE, MPI_COMM_WORLD);
            // MPI_Gather(yn, N/size, MPI_DOUBLE, yn_gen, N/size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            // MPI_Bcast(yn_gen, N, MPI_DOUBLE, 0 ,MPI_COMM_WORLD);
            //print_vec(yn_gen, 2);
            //printf("calc tn %d\n", myrank);
            curr_res = mult_mat_vec(curr_res,yn_gen,recA,N/size, N);
            //print_vec(curr_res, N/size);
            //printf("curr:%lf %lf\n", curr_res[1], curr_res[2] );
            //printf("yn:%lf %lf\n", yn[1] , yn[2]);
            //MPI_Gather(curr_res, N/size, MPI_DOUBLE, curr_res_gen, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            double sc1 = calculate_scal_pr(yn, curr_res,N/size);
            double sc2 = calculate_scal_pr(curr_res, curr_res,N/size);
            //printf("sc1%lf \n", sc1);
            MPI_Allreduce(&sc1, &sc1_gen, 1, MPI_DOUBLE, MPI_SUM,MPI_COMM_WORLD);
            MPI_Allreduce(&sc2, &sc2_gen, 1, MPI_DOUBLE, MPI_SUM ,MPI_COMM_WORLD);
            //if(myrank == 0){
                //printf("GEN %lf\n", sc1_gen);
            tn = sc1_gen/sc2_gen;
                //printf("tn:%lf \n", tn);
            //}
            //MPI_Bcast(&tn, 1, MPI_DOUBLE, 0 ,MPI_COMM_WORLD);
            //tn = calculate_scal_pr(yn_gen, curr_res_gen,N)/calculate_scal_pr(curr_res_gen, curr_res_gen,N);
            //     MPI_Bcast(&tn, 1, MPI_DOUBLE, 0 ,MPI_COMM_WORLD);
        
            //printf("tn:%lf rank:%d\n", tn, myrank);
            //printf("calc x1n %d\n", myrank);
            //if(myrank ==0){
            counter++;
                //print_vec(yn_gen, 2);
                //print_vec(b, 2);
            norm1 =calculate_norm(yn_gen,N);
                //printf("norm %lf %lf\n", norm, e);
            if( norm1 <= sqrtE*norm2){
                break;
                    //printf("YEEEEEEEEEEEEEEEES");
                    //flag_to_exit = 1;
                    
                //}
                
            //}
            //MPI_Bcast(&flag_to_exit, 1, MPI_INT, 0, MPI_COMM_WORLD);
            //printf("flag %d\n", flag_to_exit);
            //if(flag_to_exit== 1){
                //printf("SOOOOOOOOOOOS");
                //break;
            }
            xn1 = calculate_xn1(xn1, x, tn, yn_gen,N/size, N, myrank);
            //print_vec(xn1, N/size);
            MPI_Allgather(xn1, N/size, MPI_DOUBLE, xn1_gen, N/size, MPI_DOUBLE, MPI_COMM_WORLD);
            x = xn1_gen;
            //printf("x:\n");
            //print_vec(x, N);
        }
        double locEnd = MPI_Wtime();
        currTime = locEnd - locStart;
        if(currTime < totalTime){
            totalTime = currTime;
        }
        MPI_Reduce(&totalTime, &finalTime, 1, MPI_DOUBLE, MPI_MAX,0, MPI_COMM_WORLD);
    }
    printf("res:\n");
    print_result(x,N);
    if(myrank ==0){
        printf("%lf\n", finalTime);
        printf("iter%d\n", counter);
        //print_result(x,N);
    }
    MPI_Finalize();
    free(A);
    free(b);
    free(x);
    free(yn);
    free(curr_res); 
    free (xn1);
    free (yn_gen);
    //free (xn1_gen);
    free (curr_res_gen);
    //printf("aaaaa\n");
    printf("\n");
    //printf("%d\n", counter);
    return 0;
}
