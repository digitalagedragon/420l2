#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

int main(int argc, char** argv){
  if(argc < 2){
    puts("Usage: ./bin N\n");
    return 1;
  }

  MPI_Init(NULL,NULL);

  int ping_pong_limit = atoi(argv[1]);

  MPI_Comm world = MPI_COMM_WORLD;
  int rank, world_size;
  MPI_Comm_rank(world, &rank);
  MPI_Comm_size(world, &world_size);

  /* (big) assumption is that we will only have two
   * processors doing this
   */

  int token = 0;
  // condition to test token parity vs my rank
  // if same, increment and send to the other process
  // if not, wait for a message from the other process
  MPI_Status status;
  while( token < ping_pong_limit ){
    if( token % 2 == rank % 2 ){
      token++;
      printf("Processor %d sending %d to %d\n",
          rank, token, (rank+1)%2);
      MPI_Send(&token, 1, MPI_INT, (rank+1) % 2, 0, world);
      //printf("Processor %d sent %d to %d\n",
      //    rank, token, (rank+1)%2);
    } else {
      printf("Processor %d waiting for %d\n",
          rank, (rank+1)%2);

      MPI_Recv(&token, 1, MPI_INT, (rank+1) % 2, 0, world, &status);
      printf("Status tag is %d\n", status.MPI_TAG);
      printf("Status source is %d\n", status.MPI_SOURCE);

      //printf("Processor %d received %d from %d\n",
      //    rank, token, (rank+1)%2);
    }
  }

  MPI_Finalize();
  return 0;
}

