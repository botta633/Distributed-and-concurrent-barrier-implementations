#!/bin/bash
#SBATCH -q coc-ice
#SBATCH -t 5
#SBATCH --mem-per-cpu=1G

set -euo pipefail

: "${EXE:?set EXE=...}"      # executable name (no path or with ./)

module load gcc/12.3.0 mvapich2/2.3.7-1

echo "Started on $(/bin/hostname)"
echo "SLURM_NTASKS=${SLURM_NTASKS}"

# Launch exactly the number of tasks Slurm gave us
srun -n "${SLURM_NTASKS}" "./${EXE}" "${SLURM_NTASKS}"