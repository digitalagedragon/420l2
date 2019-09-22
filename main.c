#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mfunctions.h"
#include <time.h>

int IP(int *array, int *array2, int num_elem)
{
    printf("%d is array1 element, %d is array2 element\n",array[0],array2[0]);
    int sum = 0;
    int i;
    for (i = 0; i <= num_elem; i++)
    {
        sum += array[i] * array2[i];
        printf("sum is %d\n", sum);
    }
    return sum;
}

int main(int argc, char **argv)
{
    MPI_Init(NULL, NULL);

    //init cores
    int cores;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &cores);
    int rank;

    //sets rank = to what core
    MPI_Comm_rank(world, &rank);

    srand(time(0));
    if(rank == 0){}
    int A1 = 4, A2 = 1, B1 = 4, B2 = 1;
    struct matrix A;
    struct matrix B;
    initMatrix(&A, A1, A2);
    printMatrix(&A);
    printf("\n");
    initMatrix(&B, B1, B2);
    printMatrix(&B);

    //might have rounding errors on odd cores
    //int numElem = (A1 * A2) / cores;
    
    int *collect = calloc(A1, sizeof(int));
    int *collect2 = calloc(B1, sizeof(int));
    int numElem = 1;
    MPI_Scatter(&A, numElem, MPI_INT, collect, numElem, MPI_INT, 0, world);
    MPI_Scatter(&B, numElem, MPI_INT, collect2, numElem, MPI_INT, 0, world);
    int processIP = 0;
    processIP = IP(collect, collect2, 1);
    printf("%d is processIP \n", processIP);
    int *innerp = NULL;
    if (rank == 0)
    {
        innerp = (int *)calloc(cores, sizeof(int));
    }
    MPI_Gather(&innerp, 1, MPI_INT, innerp, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0){
        int sum = 1; 
        printf("%d is sum\n", sum);
    }
    
    free(A.arr);
    free(B.arr);

    MPI_Finalize();
    return 0;
}