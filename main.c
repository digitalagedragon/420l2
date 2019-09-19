#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mfunctions.h"
#include <time.h>

void IP(int *array, int *array2, int num_elem)
{
    int sum = 0;
    int i = 0;
    for(i; i < num_elem; i++){
        sum+=array[i]*array2[i];
    }
    return sum;
}

int main(int argc, char **argv){
    MPI_Init(NULL, NULL);

    //init cores
    int cores;
    MPI_Comm temp = MPI_COMM_WORLD;
    MPI_Comm_size(temp, &cores);
    int rank;

    //sets rank = to what core
    MPI_Comm_rank(temp, &rank);

    srand(time(0));
    int Ar = 4, Ac = 1, Br = 4, Bc = 1;
    struct matrix A;
    struct matrix B;
    initMatrix(&A, A1, A2);
    printMatrix(&A);
    initMatrix(&B, B1, B2);
    printMatrix(&B);
    int numElem = (A1*A2)/cores;
    int collect = calloc(A1, sizeof(int));
    int collect2 = calloc(B1, sizeof(int));

    MPI_Scatter(A,numElem, MPI_INT, collect, numElem, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B,numElem, MPI_INT, collect2, numElem, MPI_INT, 0, MPI_COMM_WORLD);

    int processIP = IP(collect, collect2, numElem);

    

    free(A.arr);
    
    MPI_Finalize();
    return 0;
}