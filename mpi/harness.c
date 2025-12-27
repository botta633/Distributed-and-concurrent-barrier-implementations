#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "gtmpi.h"
#include <time.h>

int main(int argc, char **argv)
{
  int num_processes, num_rounds = 10;

  MPI_Init(&argc, &argv);

  if (argc < 2)
  {
    fprintf(stderr, "Usage: ./harness [NUM_PROCS]\n");
    exit(EXIT_FAILURE);
  }

  num_processes = strtol(argv[1], NULL, 10);

  gtmpi_init(num_processes);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  clock_t cpu_start = clock();

  double wall_start = MPI_Wtime();

  for (int k = 0; k < num_rounds; k++)
  {
    gtmpi_barrier();
  }

  double wall_end = MPI_Wtime();

  clock_t cpu_end = clock();
  double total_wall = wall_end - wall_start;
  double total_cpu = (double)(cpu_end - cpu_start) / CLOCKS_PER_SEC;

  if (rank == 0)
  {
    printf("Processes: %d, Rounds: %d\n", num_processes, num_rounds);
    printf("Total Wall Time : %.6f s, Avg/round: %.3f us\n",
           total_wall, 1e6 * total_wall / num_rounds);
    printf("Total CPU Time  : %.6f s, Avg/round: %.3f us\n",
           total_cpu, 1e6 * total_cpu / num_rounds);
  }

  gtmpi_finalize();

  MPI_Finalize();

  return 0;
}
