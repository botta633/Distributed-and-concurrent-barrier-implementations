# Barrier Synchronization Algorithms: OpenMP, MPI, and Hybrid

[cite_start]**Author:** Ahmed Ehab Hamouda [cite: 3]  
[cite_start]**Institution:** Georgia Institute of Technology, College of Computing [cite: 2]

## Overview
[cite_start]This repository contains a comprehensive narrative explanation and implementation of barrier synchronization algorithms, covering OpenMP (Shared Memory), MPI (Distributed Memory), and Hybrid approaches[cite: 7]. [cite_start]The project explores how these algorithms operate, how they minimize contention, and compares their performance through experimental observations[cite: 8, 64].

## Implementations

### 1. OpenMP Algorithms (Shared Memory)
[cite_start]These implementations run locally on a shared-memory node and are designed to manage thread synchronization efficiently[cite: 69].

#### MP1: Sense-Reversing Barrier
* [cite_start]**Description:** An implementation designed to minimize contention and false sharing[cite: 11].
* **Mechanism:**
    * [cite_start]Each thread maintains a private sense variable stored in a cache-line-aligned structure to ensure no two threads share the same cache line, reducing invalidations[cite: 12, 13].
    * [cite_start]Threads flip their local sense and atomically increment a shared counter using relaxed atomic operations[cite: 15].
    * [cite_start]The last thread resets the counter and toggles a global sense flag[cite: 18].
* [cite_start]**Complexity:** Scales linearly $O(N)$, but the critical section is constant time, resulting in near-constant latency on moderate core counts[cite: 20].

#### MP2: Combining Tree Barrier
* [cite_start]**Description:** A hierarchical synchronization mechanism based on a binary combining tree structure[cite: 21, 22].
* **Mechanism:**
    * [cite_start]Threads are arranged as leaves in a binary tree; internal nodes combine arrivals before propagating them upward[cite: 23, 24].
    * [cite_start]Uses weak atomic operations with **release** semantics for sense updates and **acquire** reads for spinning, ensuring memory visibility with minimal overhead[cite: 33, 34, 35].
* [cite_start]**Complexity:** Logarithmic scaling $O(\log N)$ with respect to the number of threads[cite: 37].

---

### 2. MPI Algorithms (Distributed Memory)
[cite_start]These algorithms manage synchronization across multiple processes/nodes using message passing[cite: 7].

#### MPI1: Linear Barrier (Ring-Based)
* [cite_start]**Description:** Follows a ring-based synchronization pattern[cite: 44].
* **Mechanism:**
    * Each process sends an arrival message to its successor.
    * [cite_start]Once the last rank receives the token, it broadcasts the end of the phase to all ranks[cite: 45, 46].
* [cite_start]**Complexity:** Latency grows linearly $O(P)$, as each hop introduces one message round-trip[cite: 47].

#### MP12: Binomial Tree Barrier
* [cite_start]**Description:** A tree algorithm using pairwise exchanges between MPI ranks[cite: 50].
* **Mechanism:**
    * **Arrival Phase:** Processes compute a communication partner by XORing their rank ID with a bitmask. [cite_start]Arrivals are reduced toward rank 0 in $O(\log P)$ steps[cite: 51, 52].
    * [cite_start]**Release Phase:** The direction is reversed; rank 0 fans out the "release" token back through the partners[cite: 53].
* [cite_start]**Complexity:** Total work per process is proportional to $O(\log P)$[cite: 54].

---

### 3. Hybrid Algorithm (MPI + OpenMP)
* [cite_start]**Selection:** Combines the fastest observed algorithms: **OpenMP Sense-Reversing** (MP1) for local node synchronization and **MPI Binomial Tree** (MP12) for distributed synchronization[cite: 60].
* **Workflow:**
    1.  [cite_start]Master thread initializes MPI processes[cite: 61].
    2.  Threads synchronize locally using the OpenMP barrier.
    3.  [cite_start]The master thread calls the MPI barrier to communicate completion across nodes[cite: 62].

---

## Experimental Methodology
[cite_start]Experiments evaluated the Average CPU Time (actual compute cycles) and Wall Clock Time (total elapsed time including communication)[cite: 64, 72].

* [cite_start]**Iterations:** Each test was run three times per configuration and averaged to ensure accuracy[cite: 67].
* [cite_start]**Environment:** Experiments were repeated across multiple days to minimize transient system effects (scheduling noise, network jitter)[cite: 68].
* **Configurations:**
    * [cite_start]**OpenMP:** 2 to 8 threads (increments of 2)[cite: 69].
    * [cite_start]**MPI:** 2 to 12 processes (increments of 2) on separate nodes (PACE cluster)[cite: 70].
    * [cite_start]**Hybrid:** 2 to 8 MPI processes, each managing 2 to 12 OpenMP threads[cite: 71].

---

## Results & Findings

### OpenMP Results
* [cite_start]**Speed:** The **MP1 (Sense-Reversing)** barrier was found to be much faster than the **MP2 (Combining Tree)**, which was unexpected given the latter's logarithmic complexity[cite: 126].
* [cite_start]**Consistency:** Sense-reversing behavior was highly consistent, whereas Combining Tree results fluctuated day-to-day[cite: 128, 129].

### MPI Results
* [cite_start]**Performance:** The **Binomial Tree (MP12)** algorithm was consistently faster than the linear algorithm, confirming expected logarithmic scaling benefits[cite: 167, 168].

### Hybrid Results
* [cite_start]**Scalability:** The hybrid barrier scaled better than pure linear algorithms, with performance not drastically decreasing as nodes increased[cite: 170].
* [cite_start]**Bottleneck:** For node counts of 6 and 8, increasing threads per process beyond 8 or 10 caused a performance decrease, likely due to cluster resource allocation limits[cite: 226].
