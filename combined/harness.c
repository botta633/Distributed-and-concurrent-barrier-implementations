#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include "gtompi.h"

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <num_processes> <num_threads>\n", argv[0]);
        return 1;
    }

    int expected_procs = (int)strtol(argv[1], NULL, 10);
    int num_threads = (int)strtol(argv[2], NULL, 10);
    int rounds = 10;

    /* ---------- MPI init (FUNNELED) ---------- */
    int provided = 0;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
    if (provided < MPI_THREAD_FUNNELED)
    {
        fprintf(stderr, "Error: MPI thread support < FUNNELED\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int rank = -1, nprocs = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    if (rank == 0 && nprocs != expected_procs)
    {
        fprintf(stderr,
                "Warning: expected %d MPI processes, but launcher started %d.\n",
                expected_procs, nprocs);
    }

    /* ---------- OpenMP setup ---------- */
    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);

    /* ---------- Parallel region ---------- */
    double t0 = 0.0, t1 = 0.0;
    gtompi_init(nprocs, num_threads);

#pragma omp parallel shared(num_threads)
    {
#pragma omp master
        t0 = MPI_Wtime();

        for (int i = 0; i < rounds; ++i)
        {
            gtompi_barrier();
        }

#pragma omp master
        t1 = MPI_Wtime();
    }

    gtompi_finalize();

    /* ---------- Report ---------- */
    if (rank == 0)
    {
        double total = t1 - t0;
        printf("Ranks: %d (expected %d), Threads/rank: %d, Rounds: %d, "
               "Total: %.6f s, Avg/round: %.3f us\n",
               nprocs, expected_procs, num_threads, rounds,
               total, 1e6 * total / rounds);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}