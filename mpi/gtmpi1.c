#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "gtmpi.h"

int my_id, my_dst, my_src, num_procs, tag, my_msg[2];
MPI_Status mpi_result;
int iteration = 0;

void gtmpi_init(int num_processes)
{
    num_procs = num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    my_dst = (my_id + 1) % num_procs;
    my_src = (my_id - 1) % num_procs;
    while (my_src < 0)
        my_src += num_procs;

    my_msg[0] = my_id;
    my_msg[1] = num_procs;
}

void gtmpi_barrier()
{
    int dummy = 0;
    if (my_id != 0)
    {
        MPI_Recv(&my_msg, 2, MPI_INT, my_src, tag, MPI_COMM_WORLD, &mpi_result);
    }
    if (my_id != num_procs - 1)
    {
        MPI_Send(&my_msg, 2, MPI_INT, my_dst, tag, MPI_COMM_WORLD);
    }
    MPI_Bcast(&dummy, 1, MPI_INT, num_procs - 1, MPI_COMM_WORLD);
}

void gtmpi_finalize()
{
}
