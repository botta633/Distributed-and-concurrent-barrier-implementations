# Barrier Synchronization Algorithms: OpenMP, MPI, and Hybrid

**Author:** Ahmed Ehab Hamouda  
**Institution:** Georgia Institute of Technology, College of Computing

## Overview
This repository contains a comprehensive, narrative explanation and implementation of barrier synchronization algorithms, covering OpenMP (Shared Memory), MPI (Distributed Memory), and Hybrid approaches. The project explores how these algorithms operate, how they minimize contention, and compares their performance through experimental observations.

## Implementations

### 1. OpenMP Algorithms (Shared Memory)
These implementations run locally on a shared-memory node and are designed to manage thread synchronization efficiently.

#### MP1: Sense-Reversing Barrier
* **Description:** An implementation designed to minimize contention and false sharing in shared-memory environments.
* **Mechanism:**
    * Each thread maintains a private sense variable stored in a cache-line-aligned structure to ensure no two threads share the same cache line, reducing invalidations.
    * Threads flip their local sense and atomically increment a shared counter using relaxed atomic operations.
    * The last thread resets the counter and toggles a global sense flag.
* **Complexity:** Scales linearly $O(N)$, but the critical section is constant time, resulting in near-constant latency on moderate core counts.

#### MP2: Combining Tree Barrier
* **Description:** A hierarchical synchronization mechanism based on a binary combining tree structure.
* **Mechanism:**
    * Threads are arranged as leaves in a binary tree; internal nodes combine arrivals before propagating them upward.
    * Uses weak atomic operations with **release** semantics for sense updates and **acquire** reads for spinning, ensuring memory visibility with minimal overhead.
* **Complexity:** Logarithmic scaling $O(\log N)$ with respect to the number of threads.

---

### 2. MPI Algorithms (Distributed Memory)
These algorithms manage synchronization across multiple processes/nodes using message passing.

#### MPI1: Linear Barrier (Ring-Based)
* **Description:** Follows a ring-based synchronization pattern.
* **Mechanism:**
    * Each process sends an arrival message to its successor.
    * Once the last rank receives the token, it broadcasts the end of the phase to all ranks.
* **Complexity:** Latency grows linearly $O(P)$, as each hop introduces one message round-trip.

#### MP12: Binomial Tree Barrier
* **Description:** A tree algorithm using pairwise exchanges between MPI ranks.
* **Mechanism:**
    * **Arrival Phase:** Processes compute a communication partner by XORing their rank ID with a bitmask. Arrivals are reduced toward rank 0 in $O(\log P)$ steps.
    * **Release Phase:** The direction is reversed; rank 0 fans out the "release" token back through the partners.
* **Complexity:** Total work per process is proportional to $O(\log P)$.

---

### 3. Hybrid Algorithm (MPI + OpenMP)
* **Selection:** Combines the fastest observed algorithms: **OpenMP Sense-Reversing** (MP1) for local node synchronization and **MPI Binomial Tree** (MP12) for distributed synchronization.
* **Workflow:**
    1.  Master thread initializes MPI processes.
    2.  Threads synchronize locally using the OpenMP barrier.
    3.  The master thread calls the MPI barrier to communicate completion across nodes.

---

## Experimental Methodology
Experiments evaluated the Average CPU Time (actual compute cycles) and Wall Clock Time (total elapsed time including communication).

* **Iterations:** Each test was run three times per configuration and averaged to ensure accuracy.
* **Environment:** Experiments were repeated across multiple days to minimize transient system effects (scheduling noise, network jitter).
* **Configurations:**
    * **OpenMP:** 2 to 8 threads (increments of 2).
    * **MPI:** 2 to 12 processes (increments of 2) on separate nodes (PACE cluster).
    * **Hybrid:** 2 to 8 MPI processes, each managing 2 to 12 OpenMP threads.

---

## Results & Findings

### OpenMP Results
* **Speed:** The **MP1 (Sense-Reversing)** barrier was found to be much faster than the **MP2 (Combining Tree)**, which was unexpected given the latter's logarithmic complexity.
* **Consistency:** Sense-reversing behavior was highly consistent, whereas Combining Tree results fluctuated day-to-day.

### MPI Results
* **Performance:** The **Binomial Tree (MP12)** algorithm was consistently faster than the linear algorithm, confirming expected logarithmic scaling benefits.

### Hybrid Results
* **Scalability:** The hybrid barrier scaled better than pure linear algorithms, with performance not drastically decreasing as nodes increased.
* **Bottleneck:** For node counts of 6 and 8, increasing threads per process beyond 8 or 10 caused a performance decrease, likely due to cluster resource allocation limits.
