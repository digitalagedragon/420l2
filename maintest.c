#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mfunctions.h"
#include <time.h>

int IP(int *array, int *array2, int num_elem, int rank)
{
    int sum = 0;
    for (int i = 0; i < num_elem; i++)
    {
        printf("%d is array1 element, %d is array2 element, this is rank:%d\n", array[i], array2[i], rank);
        sum += array[i] * array2[i];
        printf("%d thinks the sum is %d at loop %d\n\n", rank, sum, i);
    }
    printf("%d thinks the sum is %d\n", rank, sum);
    return sum;
}

int main(int argc, char **argv)
{
    int A1 = 4, A2 = 1, B1 = 4, B2 = 1;
    struct matrix A;
    struct matrix B;
    int *retbuf1;
    int *retbuf2;
    int retval = 0;
    int globalsum = 0;
    int disperse = 0;

    MPI_Init(NULL, NULL);

    //init cores
    int cores;
    MPI_Comm world = MPI_COMM_WORLD;
    MPI_Comm_size(world, &cores);
    int rank;

    //sets rank = to what core
    MPI_Comm_rank(world, &rank);

    if (rank == 0)
    {
        //initialize data fields
        srand(time(0));
        initMatrix(&A, A1, A2);
        printMatrix(&A);
        printf("\n");
        initMatrix(&B, B1, B2);
        printMatrix(&B);
    }

    retbuf1 = malloc(disperse * sizeof(int));
    retbuf2 = malloc(disperse * sizeof(int));
    if (cores == 1)
    {
        disperse = (A1 * A2) / cores;
    }
    else
    {
        disperse = (A1 * A2) / (cores - 1);
    }
    for (int i = 0; i < disperse; i++)
    {
        retbuf1[i] = 0;
    }
    for (int i = 0; i < disperse; i++)
    {
        retbuf2[i] = 0;
    }
    int *scatterto = malloc(cores * sizeof(int));
    int *displacement = malloc(cores * sizeof(int));
    for (int i = 0; i < cores; i++)
    {
        scatterto[i] = cores;
        displacement[i] = disperse;
    }
    if (cores != 1)
    {
        scatterto[0] = 0;
    }
    MPI_Barrier(world);
    MPI_Scatterv((A.arr), scatterto, displacement, MPI_INT, retbuf1, disperse, MPI_INT, 0, world);
    MPI_Scatterv((B.arr), scatterto, displacement, MPI_INT, retbuf2, disperse, MPI_INT, 0, world);
    retval = IP(retbuf1, retbuf2, disperse);
    MPI_Barrier(world);
    MPI_Reduce(&retval, &globalsum, 1, MPI_INT, MPI_SUM, 0, world);
    if (rank == 0)
    {
        printf("%d is sum\n", globalsum);
    }
    free(retbuf1);
    free(retbuf2);
    free(A.arr);
    free(B.arr);
    free(scatterto);
    free(displacement);

    MPI_Finalize();
    return 0;
}