#include <omp.h>
#include "gtmp.h"
#include <stdlib.h>
#include <stdio.h>

static struct barrier_t
{
    int num_threads;
    int count;
    int sense;
    int *local_sense;
} barrier;
void gtmp_init(int num_threads)
{
    barrier.num_threads = num_threads;
    barrier.count = 0;
    barrier.local_sense = malloc(sizeof(int) * num_threads);
    barrier.sense = 1;
#pragma omp parallel num_threads(num_threads) shared(barrier)
    {
        int x = omp_get_thread_num();
        barrier.local_sense[x] = 1;
    }
}

void gtmp_barrier()
{

    int x = omp_get_thread_num();
    barrier.local_sense[x] = 1 - barrier.local_sense[x];
#pragma omp atomic update relaxed
    barrier.count++;

#pragma omp critical
    {
        if (barrier.count == barrier.num_threads)
        {
            barrier.count = 0;
            barrier.sense = barrier.local_sense[x];
        }
    }
    while (barrier.sense != barrier.local_sense[x])
    {
    }
}

void gtmp_finalize()
{
    free(barrier.local_sense);
}
