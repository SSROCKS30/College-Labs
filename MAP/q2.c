#include<stdio.h>
#include<omp.h>
#include<stdlib.h>
#define N 100
#define CHUNKS 10

int main(){
    int i, tid, chunks, nthreads;
    int a[N], b[N], c[N];

    for(i = 0; i < N; i++){
        a[i] = b[i] = 2 * i;
    }

    chunks = CHUNKS;

    #pragma omp parallel shared(nthreads, chunks, a, b, c) private(tid, i)
    {
        tid = omp_get_thread_num();
        if(tid == 0){
            nthreads = omp_get_num_threads();
            printf("Total Number of Threads: %d\n", nthreads);
        }

        printf("Current thread: %d\n", tid);
        #pragma omp for schedule(static, chunks)
        for(i = 0; i < N; i++){
            c[i] = a[i] + b[i];
            printf("Thread: %d, c[%d] = %d\n", tid, i, c[i]);
        }
    }
}