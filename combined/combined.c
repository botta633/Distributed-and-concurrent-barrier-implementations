#include "gtompi.h"
#include <stdlib.h>
#include <stdio.h>
#include "../mpi/gtmpi.h"
#include "../omp/gtmp.h"
#include <omp.h>
#include <mpi.h>

void gtompi_init(int processes, int threads)
{
    gtmp_init(threads);
#pragma omp master
    {
        gtmpi_init(processes);
    }
}

void gtompi_barrier()
{
    gtmp_barrier();

#pragma omp master
    {
        gtmpi_barrier();
    }
}

void gtompi_finalize()
{
    gtmp_finalize();
    gtmpi_finalize();
}