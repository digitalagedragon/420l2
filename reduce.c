//Vector addition using Send and Recv
//Works for mpiexec -n 2./Reduce

#include<stdlib.h> // rand, srand
#include<stdio.h> // puts, printf
#include<time.h> // time
#include<mpi.h> // mpi stuff

int main(int argc, char** argv){
  MPI_Init(NULL,NULL);

  MPI_Comm world = MPI_COMM_WORLD;

  // get number of nodes
  int nprocs, me;
  MPI_Comm_size(world, &nprocs);
  MPI_Comm_rank(world, &me);
  MPI_Status stat;

  srand(time(0) + me);

  float* myNumber=malloc(2*sizeof(int));
  float* myNumber2=malloc(2*sizeof(int));
  float* sum=malloc(2*sizeof(int));
  
  if(me==0){
    myNumber[0] = rand()%10;
    myNumber[1] = rand()%10;
    myNumber2[0] = rand()%10;
    myNumber2[1] = rand()%10;
    printf("Rank %d's numbers were: %2.0f %2.0f\n", me, myNumber[0], myNumber[1]);
    printf("Rank %d's numbers were: %2.0f %2.0f\n", me, myNumber2[0], myNumber2[1]);
  }
  // int MPI_Allreduce(
  //   const void* sendbuf,
  //         void* recvbuf,
  //         int count,
  //         MPI_Datatype datatype,
  //         MPI_Op op,
  //         MPI_Comm comm)

  //printf("Rank %d's numbers were: %2.0f %2.0f\n", me, myNumber[0], myNumber[1]);
  // sum will have the sum of all nodes' numbers
  //MPI_Allreduce(&myNumber[0], &sum[0], 2, MPI_FLOAT, MPI_SUM, world);
  
  if(me==0){
 //   printf("1Hi I am node %d\n",me);
    MPI_Send(&myNumber[1], 1, MPI_INT, 1, 123, world);
    MPI_Send(&myNumber2[1], 1, MPI_INT, 1, 321, world);
    sum[0]=myNumber[0]+myNumber2[0];
   // printf("2Hi I am node %d\n",me);
    
  }
  else{
  //  printf("3Hi I am node %d\n",me);
    MPI_Recv(&myNumber[1], 1, MPI_INT, 0, 123, world, &stat);
    MPI_Recv(&myNumber2[1], 1, MPI_INT, 0, 321, world, &stat);
    sum[1]=myNumber[1]+myNumber2[1];
  //  printf("4Hi I am node %d\n",me);
    printf("myNumber[0][1]= %2.0f\n",myNumber[1]);
    printf("myNumber2[0][1]=%2.0f\n",myNumber2[1]);
    printf("sum[0][1]= %2.0f\n",sum[1]);
    
  }  // calc the average

  if(me==1){
   //   printf("5Hi I am node %d\n",me);
      MPI_Send(&sum[1], 1, MPI_INT, 0, 456, world);
   //   printf("6Hi I am node %d\n",me);
  }
  else{
    //  printf("7Hi I am node %d\n",me);
    MPI_Recv(&sum[1], 1, MPI_INT, 1, 456, world, &stat);
    printf("Rank %d's numbers were: %2.0f %2.0f\n", me, sum[0], sum[1]);
    //printf("8Hi I am node %d\n",me);
  }
  /* if( me == 0 ){
    printf("The average was %2.1f\n", avg);
  } */
  
  MPI_Finalize();

  // free(myNumber);
  // free(myNumber2);
  // free(sum);
  return 0;
}