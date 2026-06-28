# HFT & Low-Latency C++ Interview Guide

## What HFT Interviewers Look For

### Cache Optimization
- Data-oriented design: struct of arrays vs array of structs
- Hot/cold data separation keep critical path data in L1
- Prefetching (`__builtin_prefetch`), cache line alignment (`alignas(64)`)
- Avoid pointer chasing; prefer contiguous memory (vector > list)

### Lock-Free Structures
- CAS loops (`compare_exchange_weak/strong`)
- Lock-free queue → `src/concurrency/lock_free_queue.cpp`
- ABA problem: use tagged pointers or hazard pointers
- Memory ordering: acquire/release for producer-consumer

### Custom Allocators
- Arena/pool allocators for deterministic latency → `src/modern_cpp/custom_allocator.cpp`
- Memory pool → `src/stl_like/memory_pool.cpp`
- Avoid `malloc` on hot path (pre-allocate everything)
- Huge pages (`mmap` with `MAP_HUGETLB`) to reduce TLB misses

### SIMD & Vectorization
- SSE/AVX intrinsics for batch processing → `src/systems/hpc_gpu/simd_vectorization.cpp`
- Auto-vectorization hints: `restrict`, loop alignment, no branches in loop body
- Use SIMD for checksum, parsing, search in market data

### Kernel Bypass
- DPDK: user-space networking, poll-mode drivers
- Solarflare OpenOnload: kernel bypass via ef_vi
- Bypass eliminates syscall overhead (~1μs → ~100ns for packet)
- Busy-polling vs interrupt-driven

### Hot/Cold Path Separation
- `[[likely]]` / `[[unlikely]]` attributes
- Place error handling in cold functions (won't pollute icache)
- Inline hot path; `__attribute__((noinline))` cold path
- Profile with `perf stat` look at icache misses

### Branch Prediction
- `__builtin_expect` (or C++20 `[[likely]]`)
- Branchless programming: `x = (cond) * a + (!cond) * b`
- Sort-by-likelihood in switch statements
- Measure with `perf stat -e branch-misses`

### Memory Layout
- `alignas(64)` to avoid false sharing
- Pack structs on hot path; pad between thread-local data
- Use `std::hardware_destructive_interference_size`

## Repo HFT Examples
- Order book (sorted price levels, O(1) best bid/ask) → `src/systems/hft/order_book.cpp`
- Market data handler (fast parsing) → `src/systems/hft/market_data_handler.cpp`
- Matching engine → `src/systems/hft/matching_engine.cpp`

## Common HFT Interview Questions

| Question | Key Points |
|----------|------------|
| Design an order book | Sorted map for price levels, deque per level, O(1) top-of-book |
| Lock-free SPSC queue | Ring buffer + atomic head/tail, acquire-release ordering |
| Reduce tick-to-trade latency | Kernel bypass, pre-allocated buffers, branch-free parsing |
| False sharing explain & fix | Two threads writing adjacent cache lines; pad with `alignas(64)` |
| When would you use `relaxed` ordering? | Counters, statistics no inter-thread data dependency |
| Custom allocator for trading | Pool allocator: fixed-size blocks, free-list, O(1) alloc/dealloc |
| How to measure latency? | `rdtsc`, `clock_gettime(CLOCK_MONOTONIC)`, histogram percentiles |
| What is cache warming? | Touch data structures before market open to load into cache |

## Target Companies & Focus Areas

| Company | Special Focus |
|---------|--------------|
| Citadel Securities | System design, low-latency networking, atomics |
| Jump Trading | FPGA awareness, kernel bypass, lock-free |
| Optiver | Mental math + cache optimization + branchless code |
| Two Sigma | Concurrency, system design, Python+C++ interop |
| Tower Research | Template metaprogramming, SIMD, memory model |
