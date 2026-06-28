# HPC & GPU Programming Interview Guide

## SIMD (Single Instruction, Multiple Data)

| Instruction Set | Width | Floats/op | Use Case |
|----------------|-------|-----------|----------|
| SSE | 128-bit | 4 float / 2 double | Baseline SIMD |
| AVX/AVX2 | 256-bit | 8 float / 4 double | General HPC |
| AVX-512 | 512-bit | 16 float / 8 double | Intel Xeon, some AMD |

```cpp
#include <immintrin.h>
__m256 a = _mm256_load_ps(data);
__m256 b = _mm256_load_ps(data + 8);
__m256 c = _mm256_add_ps(a, b);  // 8 additions in one instruction
```

→ `src/systems/hpc_gpu/simd_vectorization.cpp`

## Cache Hierarchy

| Level | Size | Latency | Shared |
|-------|------|---------|--------|
| L1 | 32-64 KB | ~1 ns (4 cycles) | Per core |
| L2 | 256 KB-1 MB | ~4 ns (12 cycles) | Per core |
| L3 | 8-64 MB | ~12 ns (40 cycles) | Per socket |
| DRAM | GBs | ~100 ns | All cores |

**Cache line:** 64 bytes. Access patterns matter more than algorithm complexity at scale.

## NUMA Awareness

- Non-Uniform Memory Access: memory latency depends on which socket owns it
- `numactl --membind=0` pin memory to local node
- `numa_alloc_local()` in code
- Thread pinning: `pthread_setaffinity_np` or `taskset`
- Crossing NUMA boundary: ~1.5-2x latency penalty

## Vectorization Tips
- Align data to cache lines (`alignas(64)`)
- Avoid branches in loops (use masks instead)
- `#pragma omp simd` or `__attribute__((vector))` hints
- Check with `-fopt-info-vec-missed` (GCC) or `-Rpass=loop-vectorize` (Clang)

## OpenMP Basics
```cpp
#pragma omp parallel for reduction(+:sum)
for (int i = 0; i < n; ++i) sum += data[i];
```
Key directives: `parallel`, `for`, `critical`, `atomic`, `reduction`, `schedule(dynamic)`

## MPI Basics
- Distributed memory: each process has its own address space
- `MPI_Send/Recv` (point-to-point), `MPI_Bcast`, `MPI_Reduce` (collective)
- Hybrid: MPI between nodes + OpenMP within node

## GPU / CUDA

### Memory Model
| Memory | Scope | Speed | Size |
|--------|-------|-------|------|
| Registers | Per thread | Fastest | Limited |
| Shared | Per block | ~5ns | 48-96 KB |
| Global (DRAM) | All threads | ~400 cycles | GBs |
| Constant | All threads (read-only) | Cached | 64 KB |

### Thread Hierarchy
- **Thread** → **Warp (32 threads)** → **Block** → **Grid**
- All threads in a warp execute same instruction (SIMT)

### Warp Divergence
- `if/else` within a warp → both branches executed, results masked
- Fix: structure code so warps take same path, or use predication

### Memory Coalescing
- Adjacent threads access adjacent memory → single transaction
- Strided or random access → multiple transactions (huge slowdown)
- AoS → SoA transformation for GPU

## Common Interview Questions

| Question | Key Answer |
|----------|------------|
| Why is GPU fast for ML? | Thousands of cores, optimized for data-parallel ops |
| Warp divergence | Threads in warp take different branches → serialization |
| Shared memory bank conflicts | 32 banks; same bank access by multiple threads → sequential |
| CPU cache vs GPU shared memory | CPU cache is automatic; GPU shared memory is explicitly managed |
| How to detect NUMA issues? | `numastat`, `perf stat` with NUMA events, latency profiling |
