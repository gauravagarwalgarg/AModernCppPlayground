# OS Internals Interview Guide

## Virtual Memory

- Each process sees contiguous virtual address space (typically 48-bit on x86_64)
- Page size: 4 KB (standard), 2 MB / 1 GB (huge pages)
- Virtual → Physical translation via page tables
- **Demand paging:** pages loaded on first access (page fault)

→ `src/systems/os/virtual_memory.cpp`

## Page Tables

- Multi-level (x86_64: 4-level, PML4 → PDP → PD → PT)
- Each entry: physical frame number + flags (present, writable, user, dirty, accessed)
- Page walk: ~4 memory accesses without TLB

## TLB (Translation Lookaside Buffer)

- Cache for recent virtual→physical translations
- L1 TLB: ~64 entries per core, ~1 cycle
- L2 TLB: ~1024-2048 entries, ~7 cycles
- TLB miss: full page walk (~20-100 cycles)
- **Huge pages** reduce TLB pressure (1 entry covers 2MB instead of 4KB)

## Context Switching

| Step | What Happens |
|------|-------------|
| 1 | Save CPU registers (general, FP, SIMD) to kernel stack |
| 2 | Save TLB state / flush (if different address space) |
| 3 | Switch page tables (CR3 on x86) |
| 4 | Restore target process registers |
| 5 | Return to user-space |

Cost: ~1-10μs (depends on TLB flush, cache cold start)

## Scheduling Algorithms

| Algorithm | Type | Used In |
|-----------|------|---------|
| CFS (Completely Fair Scheduler) | Weighted fair share, red-black tree | Linux default |
| RT (SCHED_FIFO / SCHED_RR) | Priority-based, preemptive | Real-time tasks |
| SCHED_DEADLINE | Earliest deadline first | Deadline-critical workloads |
| O(1) scheduler | Priority arrays, O(1) pick | Older Linux (pre-2.6.23) |

→ `src/systems/os/scheduler_sim.cpp`

## System Calls

- User-space → kernel transition via `syscall` instruction (x86_64)
- Cost: ~100-200ns (register save + mode switch + validation)
- vDSO: kernel-mapped into user-space for fast calls (`clock_gettime`, `gettimeofday`)
- Minimize syscalls on hot path (batch I/O, `io_uring`)

## Interrupts

| Type | Source | Example |
|------|--------|---------|
| Hardware (IRQ) | Devices | NIC packet arrival, timer tick |
| Software (trap) | CPU instruction | `syscall`, `int 0x80` |
| Exception | CPU error | Page fault, divide by zero, GP fault |

Interrupt flow: save context → IDT lookup → handler → `iret`

## DMA (Direct Memory Access)

- Device reads/writes main memory without CPU involvement
- CPU sets up DMA descriptor (source, dest, length) → device does transfer
- Interrupt on completion
- Essential for: disk I/O, network cards, GPU data transfer

## Common OS Interview Questions

| Question | Key Points |
|----------|------------|
| What happens on a page fault? | Check valid mapping → allocate frame → load from disk → update PT → restart instruction |
| Process vs thread? | Process = separate address space. Thread = shared address space, own stack/registers |
| What's a zombie process? | Terminated but parent hasn't called `wait()`. Entry remains in process table |
| Explain thrashing | Working set > physical memory → constant page faults → system crawls |
| Priority inversion | Low-priority thread holds lock needed by high-priority thread. Fix: priority inheritance |
| How does `malloc` work? | `brk`/`mmap` from kernel; user-space allocator manages free lists/arenas |
