#ifndef MFUNCTIONS_H //Header protection
#define MFUNCTIONS_H

typedef struct matrix
{
    int rows, cols;
    double *arr;
} matrix;

void initMatrix(matrix *A, int r, int c);

void printMatrix(matrix *A);

int index_calc(matrix *A, int i, int j);

// probably should go in some kind of log.h but I'm lazy
#include <stdio.h>
void log_init();
#ifndef EXTERN
#define EXTERN extern
#endif
EXTERN FILE *_log;
EXTERN int _rank;
#undef EXTERN
#define LOG(fmt, ...) fprintf(_log, "[%s %d %d] " fmt "\n", __FILE__, __LINE__, _rank __VA_OPT__(,) __VA_ARGS__)

#endif
