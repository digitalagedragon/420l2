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

    struct matrix A;
    initMatrix(&A, 3, 4);
    printMatrix(&A);

    free(A.arr);
    
    MPI_Finalize();
    return 0;
}