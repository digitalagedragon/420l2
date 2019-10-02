//set up a C macro to calculate the location of an element (i,j)
#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define EXTERN
#include "mfunctions.h"
#undef EXTERN

#define INDEX(n, m, i, j) m *i + j
#define ACCESS(A, i, j) (A)->arr[INDEX((A)->rows, (A)->cols, (i), (j))]

void initMatrix(matrix *A, int r, int c)
{
    A->rows = r;
    A->cols = c;
    A->arr = calloc(r * c, sizeof(A->arr[0]));

    int i, j;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            ACCESS(A, i, j) = rand() % 10 + 1;
}

void printMatrix(matrix *A)
{
    int i, j;
    for (i = 0; i < A->rows; i++)
    {
        for (j = 0; j < A->cols; j++)
        {
            printf("%lf ", ACCESS(A, i, j));
        }
        //printf("\n");
        puts("");
    }
}

int index_calc(matrix *A, int i, int j)
{
    return INDEX(A->rows, A->cols, i, j);
}

void log_init() {
  _rank = -1;
  char buf[256] = {0};
  sprintf(buf, "mpi_%d.log", getpid());
  _log = fopen(buf, "w");
  if(!_log) {
    fprintf(stderr, "Couldn't open '%s': %s\n", buf, strerror(errno));
    exit(1);
  }
}

void log_set_rank(int rank) {
  _rank = rank;
}

void gauss_mv(MPI_Comm world, int rank, int cores, struct matrix A, double *ROOT_b) {
  int pivot;
  MPI_Status status;
  double *ROOT_l_k = 0;
  for(pivot = 0; pivot < A.rows; pivot++) {
    LOG("pivot: %d", pivot);
    
    // Master - send fragments of A, l_k to each process
    if(rank == 0) {
      ROOT_l_k = calloc(A.rows, sizeof(double));
      int i;
      for(i = 0; i < A.rows; i++) {
	ROOT_l_k[i] = A.arr[i * A.cols + pivot] / A.arr[pivot * A.cols + pivot];
      }
      
      int n;
      int rpc = ceil((float) A.rows / cores);
      LOG("Sending - have %d (%d) rows", A.rows, rpc);
      double *lbuf = malloc(sizeof(double) * rpc);
      // last element of bbuf is the pivot element
      double *bbuf = malloc(sizeof(double) * (rpc + 1));
      // last row of Abuf is the pivot row
      double *Abuf = malloc(sizeof(double) * (rpc + 1) * A.cols);
      for(n = 0; n < cores; n++) {
	// handle the case where n does not divide A.rows
	int rows = rpc;
	if((n + 1) * rpc > A.rows) {
	  rows = A.rows - n * rpc;
	}

	MPI_Send(&rows, 1, MPI_INT, n, 0, world);
	
	if(rows > 0) {
	  memcpy(lbuf, ROOT_l_k + n * rpc, sizeof(double) * rows);
	  memcpy(bbuf, ROOT_b + n * rpc, sizeof(double) * rows);
	  bbuf[rpc] = ROOT_b[pivot];
	  memcpy(Abuf, A.arr + n * rpc * A.cols, sizeof(double) * rows * A.cols);
	  memcpy(Abuf + rpc * A.cols, A.arr + pivot * A.cols, sizeof(double) * A.cols);

	  MPI_Send(lbuf, rows, MPI_DOUBLE, n, 0, world);
	  MPI_Send(bbuf, rows + 1, MPI_DOUBLE, n, 0, world);
	  MPI_Send(Abuf, (rows + 1) * A.cols, MPI_DOUBLE, n, 0, world);
	}
      }

      free(lbuf);
      free(bbuf);
      free(Abuf);
    }

    int rows;
    MPI_Recv(&rows, 1, MPI_INT, 0, 0, world, &status);
    LOG("Received rows = %d", rows);
    
    if(rows > 0) {
      double *l_k = malloc(sizeof(double) * rows);
      double *A_slice = malloc(sizeof(double) * (rows + 1) * A.cols);
      double *b_slice = malloc(sizeof(double) * (rows + 1));

      MPI_Recv(l_k, rows, MPI_DOUBLE, 0, 0, world, &status);
      MPI_Recv(b_slice, (rows + 1), MPI_DOUBLE, 0, 0, world, &status);
      MPI_Recv(A_slice, (rows + 1) * A.cols, MPI_DOUBLE, 0, 0, world, &status);

      int rpc = ceil((float) A.rows / cores);

      int i, j;
      for(i = 0; i < rows; i++) {
	if(rank * rpc + i == pivot) continue; /* skip r = k row */
	for(j = 0; j < A.cols; j++) {
	  A_slice[i * A.cols + j] = A_slice[i * A.cols + j] - l_k[i] * A_slice[rows * A.cols + j];
	  LOG("%d %d %lf", i + rank * rpc, j, A_slice[i * A.cols + j]);
	}
	b_slice[i] = b_slice[i] - l_k[i] * b_slice[rows];
	LOG("%d b %lf", i + rank * rpc, b_slice[i]);
	LOG("%d l_k %lf", i + rank * rpc, l_k[i]);
      }

      // no point in sending the pivot row back
      MPI_Send(A_slice, rows * A.cols, MPI_DOUBLE, 0, 0, world);
      MPI_Send(b_slice, rows, MPI_DOUBLE, 0, 0, world);
      
      free(l_k);
      free(A_slice);
      free(b_slice);
    }
    
    if(rank == 0) {
      int rpc = ceil((float) A.rows / cores);
      int n;
      for(n = 0; n < cores; n++) {
	int rows = rpc;
	if((n + 1) * rpc > A.rows) {
	  rows = A.rows - n * rpc;
	}
	if(rows > 0) {
	  MPI_Recv(A.arr + n * rpc * A.cols, rows * A.cols, MPI_DOUBLE, n, 0, world, &status);
	  MPI_Recv(ROOT_b + n * rpc, rows, MPI_DOUBLE, n, 0, world, &status);
	}
      }
      
      LOG("freeing l_k");
      free(ROOT_l_k);
    }
  }

  if(rank == 0) {
    int i, j;
    for(i = 0; i < A.rows; i++) {
      ROOT_b[i] /= A.arr[i * A.cols + i];
      for(j = 0; j < A.cols; j++) {
	A.arr[i * A.cols + j] /= A.arr[i * A.cols + i];
      }
    }
  }
  
  LOG("leaving gauss_mv");
}
