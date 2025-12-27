#!/bin/bash

#SBATCH --mem-per-cpu=1G
#SBATCH -t 5
#SBATCH -q coc-ice

echo "Started on `/bin/hostname`"

: "${EXE:?set EXE=...}"
: "${OMP_T:=8}"
: "${NODES:=8}"



module load gcc/12.3.0 mvapich2/2.3.7-1

export OMP_NUM_THREADS="$OMP_T"
export OMP_PROC_BIND=close
export OMP_PLACES=cores

srun -N $NODES --ntasks-per-node=1 --cpus-per-task=$OMP_T ./mpi_hybrid $NODES $OMP_T