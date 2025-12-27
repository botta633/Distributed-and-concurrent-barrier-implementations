#!/bin/bash
#SBATCH -q coc-ice
#SBATCH -t 5
#SBATCH --mem-per-cpu=1G

set -euo pipefail

: "${EXE:?set EXE=...}"
: "${OMP_T:=8}"

echo "Started on $(/bin/hostname)"
module load gcc/12.3.0 mvapich2/2.3.7-1

export OMP_NUM_THREADS="$OMP_T"
export OMP_PROC_BIND=close
export OMP_PLACES=cores

srun -n 1 "./$EXE" "$OMP_T"