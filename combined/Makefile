MPICC    = mpicc.mpich
CFLAGS   = -g -Wall -std=gnu99 -I.
OMPFLAGS = -fopenmp
LDLIBS   = -lm

COMBINED_SRC   = combined.c
OMP_SRC        = ../omp/gtmp1.c
MPI_SRC        = ../mpi/gtmpi2.c
HARNESS_SRC    = harness.c

OBJS = $(COMBINED_SRC:.c=.o) $(OMP_SRC:.c=.o) $(MPI_SRC:.c=.o) $(HARNESS_SRC:.c=.o)

all: combined

combined: $(OBJS)
	$(MPICC) -o $@ $(OMPFLAGS) $(CFLAGS) $^ $(LDLIBS)

%.o: %.c
	$(MPICC) $(OMPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o */*.o *.dSYM combined
