#include <stdlib.h>
#include <mpi.h>
#include <stdio.h>
#include "gtmpi.h"

int my_id, partner, num_procs, tag, my_msg[2];
MPI_Status mpi_result;
int iteration = 0;
int mask = 1;
void gtmpi_init(int num_processes)
{
    num_procs = num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

    my_msg[0] = my_id;
    my_msg[1] = num_procs;
}

void gtmpi_barrier()
{
    mask = 1;
    int dummy = 0;
    // phase 1 -> arrival
    // reduction algorithm
    while (mask < num_procs)
    {
        partner = my_id ^ mask;
        if (partner < num_procs) // checking for num_procs that is power of 2
        {
            if (my_id & mask) // send from the higher num
            {
                MPI_Send(&dummy, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
                break; // no need to send again in the next round just move to the broadcast
            }
            else
                MPI_Recv(&dummy, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        mask <<= 1; // next round
    }

    // Phase 2 broadcast
    if (!(my_id & mask))
        mask >>= 1;

    while (mask > 0)
    {
        partner = mask ^ my_id;
        if (partner < num_procs)
        {
            if (mask & my_id)
            {
                MPI_Recv(&dummy, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            else
                MPI_Send(&dummy, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
        }

        mask >>= 1;
    }
}

void gtmpi_finalize()
{
}
