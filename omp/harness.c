#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "gtmp.h"
#include <time.h>

int main(int argc, char **argv)
{
  int num_threads, num_iter = 10;

  if (argc < 2)
  {
    fprintf(stderr, "Usage: ./harness [NUM_THREADS]\n");
    exit(EXIT_FAILURE);
  }
  num_threads = strtol(argv[1], NULL, 10);

  omp_set_dynamic(0);
  if (omp_get_dynamic())
    printf("Warning: dynamic adjustment of threads has been set\n");

  omp_set_num_threads(num_threads);
  printf("%d\n", num_threads);
  gtmp_init(num_threads);
  double wall_start = omp_get_wtime();
  clock_t cpu_start = clock();

#pragma omp parallel shared(num_threads)
  {
    int i;
    for (i = 0; i < num_iter; i++)
    {
      gtmp_barrier();
    }
  }
  double wall_end = omp_get_wtime();
  clock_t cpu_end = clock();

  gtmp_finalize();
  // Compute and report
  double total_wall = wall_end - wall_start;
  double total_cpu = (double)(cpu_end - cpu_start) / CLOCKS_PER_SEC;

  printf("Threads: %d, Rounds: %d\n", num_threads, num_iter);
  printf("Total Wall Time: %.6f s, Avg/round: %.3f us\n",
         total_wall, 1e6 * total_wall / num_iter);
  printf("Total CPU Time : %.6f s, Avg/round: %.3f us\n",
         total_cpu, 1e6 * total_cpu / num_iter);

  return 0;
}
