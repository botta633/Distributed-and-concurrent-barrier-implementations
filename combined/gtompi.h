#ifndef GTOMPI_H
#define GTOMPI_H

void gtompi_init(int num_processes, int num_threads);
void gtompi_barrier();
void gtompi_finalize();

#endif
