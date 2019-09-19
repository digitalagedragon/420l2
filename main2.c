#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mfunctions.h"
#include <time.h>

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
    int Ar = 1, Ac = 4, Br = 4, Bc = 1;
    struct matrix A;
    struct matrix B;
    initMatrix(&A, A1, A2);
    printMatrix(&A);
    initMatrix(&B, B1, B2);
    printMatrix(&B);
    int numElem = (A1*A2)+(B1*B2);
    if()

    MPI_Scatter(A,
    

    free(A.arr);
    
    MPI_Finalize();
    return 0;
}