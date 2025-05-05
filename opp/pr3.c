#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>


double * mat_init(double* a, int n){
    a = (double*)calloc(n*n, sizeof(double));
    return a;
}

double * vec_init(double* a, int n){
    a = (double*)calloc(n, sizeof(double));
    return a;
}

void print_result(double* x, int n){
    for(int i =0;i<n;i++){
        if(i==444){
            printf("%f\n ", x[i]);
        }
    }
}

void fill_matrix(double *a, size_t n){
    for(int i = 0; i< n;i++){
        for(int j = 0; j< n;j++){
            a[i*n +j] =  0.0001;
            if(i==j){
                a[i*n +j] = i% n*2.0;
            }
        }
    }
    return;
}


double* mult_mat_vec(double* res, double* vec, double *A, int n){
    #pragma omp for schedule(runtime)
    for(int i=0;i<n;i++){
        res[i] =0;
        for(int j=0;j<n;j++){
            res[i]+= A[n*i+j] * vec[j]; 
        }
    }
    return res;
}

void fill_b(double* b, double* A, size_t n){
    for(int i = 0; i< n;i++){
        b[i] = i;
    }
}

void fill_x(double *x, int n){
    for(int i = 0; i< n;i++){
        x[i] = 0;
    }
}

double* calculate_yn(double* Axn, double *yn, double* b, double* A, double* x,int n){
    Axn = mult_mat_vec(Axn,x,A,n);
    #pragma omp for schedule(runtime)
    for(int i=0;i<n;i++){
        yn[i] = Axn[i]-b[i];
    }
    return yn;
}

double* calculate_xn1(double* xn1, double* xn, double tn, double* yn, int n){
    #pragma omp for schedule(runtime)
    for(int i=0;i<n;i++){
        xn1[i] = xn[i] - tn*yn[i];
    }
    return xn1;
}

double calculate_norm(double* yn, int n){
    double norm_1=0;
    for(int i=0;i<n;i++){
        norm_1+= yn[i]*yn[i];
    }
    return (norm_1);
}

int main(int argc, char **argv){
    int N=1008*2;
    double *A;
    double *b;
    double *x;
    A= mat_init(A, N);
    b = vec_init(b,N);
    x= vec_init(x,N);
    double* yn;
    yn = vec_init(yn,N);
    double* curr_res; 
    curr_res = vec_init(curr_res,N);
    double tn;
    double* xn1;
    xn1 = vec_init(xn1,N);
    double e = 1.0/100000.0;
    int uuy =0;
    double totalTime = 87654345678;
    double currTime = 0;
    double sqrtE = e*e;
    double norm2=0;
    fill_matrix(A, N);
    fill_b(b,A,N);
    int counter =0;
    norm2 = calculate_norm(b,N);
    double res1 = 0;
    double res2 = 0;
    double norm1=0;
    double indicator =sqrtE * norm2;
    #pragma omp parallel  
    {
        #pragma omp reduction(max:totalTime)
        for(int i=0;i<3;i++){
            counter=0;
            fill_x(x, N);
            double locStart = omp_get_wtime();
            for(int i=0;i< 50000;i++){
                yn = calculate_yn(curr_res,yn,b,A,x,N);
                curr_res = mult_mat_vec(curr_res,yn,A,N);
                #pragma omp single
                {
                    res1=0;
                    res2=0;
                    norm1=0;
                }
                #pragma omp for reduction(+:res1)
                for(int i=0;i<N;i++){
                    res1+= curr_res[i]*yn[i];
                }
                #pragma omp for reduction(+:res2)
                for(int i=0;i<N;i++){
                    res2+= curr_res[i]*curr_res[i];
                }
                #pragma omp single
                {
                    tn = res1/res2;
                    counter++;
                }
                xn1 = calculate_xn1(xn1, x, tn, yn,N);
                #pragma omp for reduction(+:norm1)
                for(int i=0;i<N;i++){
                    norm1+= yn[i]*yn[i];
                }
                if(norm1 <= indicator){
                    break;
                }
                x = xn1;
            }
            double locEnd= omp_get_wtime();
            print_result(x, N);
            double time = locEnd - locStart;
            if(time < totalTime){
                totalTime = time;
            }
        }   
    }
    printf("time:%lf\n", totalTime);
    printf("iterations:%d\n", counter);
    free(A);
    free(b);
    free(x);
    free(yn);
    free(curr_res);
    return 0;
}