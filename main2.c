#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "mfunctions.h"
#include <time.h>

int IP(int *array, int *array2, int num_elem, int rank)
{
	printf("%d is array1 element, %d is array2 element, this is rank:%d\n", array[0], array2[0], rank);
	int sum = 0;
	for (int i = 0; i < num_elem; i++)
	{
		sum += array[i] + array2[i];
	}
	printf("%d thinks the sum is %d\n", rank, sum);
	return sum;
}

int main(int argc, char **argv)
{
	int A1 = 20, A2 = 20, B1 = 20, B2 = 20;
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
	MPI_Status status;

	if (rank == 0)
	{
		//initialize data fields
		srand(time(0));
		initMatrix(&A, A1, A2);
		//printMatrix(&A);
		//printf("\n");
		initMatrix(&B, B1, B2);
		//printMatrix(&B);
	}

	int indexcol = 0;
	int indexrow = 0;
	int sendArows = A1 / cores;
	int collength = A2;
	unsigned int sendNum = collength * sendArows;
	//printf("%d is sendnum\n\n", sendNum);
	int *C = malloc(sizeof(int) * (sendNum));
	int *D = malloc(sizeof(int) * (sendNum));
	int n, i, j;
	if (rank == 0)
	{
		//loop 1
		printf("now entering loop 1\n");
		for (n = 0; n < cores; n++)
		{
			printf("preparing to package %d. \n", n);
			fflush(stdout);
			for (i = indexrow; i < (indexrow + sendArows); i++)
			{
				for (j = indexcol; j < collength; j++)
				{
					C[(sendArows * (i % sendArows)) + j] = A.arr[index_calc(&A, i, j)];
				}
			}
			indexrow += sendArows;
			MPI_Send(C, (sendNum), MPI_INT, n, 0, world);
		}
		//loop 2
		printf("now entering loop 2\n");
		indexcol = 0;
		indexrow = 0;
		for (n = 0; n < cores; n++)
		{
			printf("preparing to package %d loop 2. \n", n);
			fflush(stdout);
			for (i = indexrow; i < (indexrow + sendArows); i++)
			{
				for (j = indexcol; j < collength; j++)
				{
					C[(sendArows * (i % sendArows)) + j] = B.arr[index_calc(&B, i, j)];
				}
			}
			indexrow += sendArows;
			MPI_Send(C, (sendNum), MPI_INT, n, 0, world);
		}
	}
	MPI_Recv(C, (sendNum), MPI_INT, 0, 0, world, &status);
	MPI_Recv(D, (sendNum), MPI_INT, 0, 0, world, &status);
	int *E = malloc(sizeof(int) * (sendNum));
	for (i = 0; i < sendArows; i++)
	{
		for (j = 0; j < collength; j++)
		{
			E[(sendArows * i) + j] = C[(sendArows * i) + j] + D[(sendArows * i) + j];
		}
	}
	MPI_Send(E, (sendNum), MPI_INT, 0, 0, world);
	free(E);
	free(C);
	if (rank == 0)
	{
		indexcol = 0;
		indexrow = 0;
		struct matrix F;
		initMatrix(&F, A1, A2);
		for (n = 0; n < cores; n++)
		{
			MPI_Recv(D, (sendNum), MPI_INT, n, 0, world, &status);
			for (i = indexrow; i < (indexrow + sendArows); i++)
			{
				for (j = indexcol; j < collength; j++)
				{
					F.arr[index_calc(&F, i, j)] = D[(sendArows * (i % sendArows)) + j];
				}
			}
			indexrow += sendArows;
		}
		printMatrix(&F);
		free(F.arr);
	}
	free(D);
        if(rank == 0) {
          free(A.arr);
          free(B.arr);
        }

	MPI_Finalize();
	return 0;
}
