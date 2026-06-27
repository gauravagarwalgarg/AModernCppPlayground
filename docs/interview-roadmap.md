# C++ Interview Preparation Roadmap (4 Weeks)

## Week 1: Modern C++ Fundamentals

### Must-Know
- Move semantics & rvalue references → `src/modern_cpp/move_semantics.cpp`
- Smart pointers (unique, shared, weak) → `src/modern_cpp/smart_pointers.cpp`
- RAII & Rule of Five → `src/modern_cpp/raii.cpp`, `rule_of_five.cpp`
- Templates & SFINAE → `src/modern_cpp/templates_sfinae.cpp`
- Perfect forwarding → `src/modern_cpp/perfect_forwarding.cpp`
- Lambda expressions → `src/modern_cpp/lambdas.cpp`
- constexpr/consteval → `src/modern_cpp/constexpr_consteval.cpp`

### Nice-to-Know
- Ranges (C++20) → `src/modern_cpp/ranges.cpp`
- Concepts (C++20) → `src/modern_cpp/concepts.cpp`
- Structured bindings → `src/modern_cpp/structured_bindings.cpp`
- Copy elision / RVO / NRVO → `src/modern_cpp/copy_elision.cpp`

## Week 2: Concurrency + Design Patterns

### Must-Know
- std::thread, mutex, condition_variable → `src/concurrency/mutex_condvar.cpp`
- Atomics & memory orderings → `src/concurrency/atomics.cpp`, `memory_model.cpp`
- Lock-free data structures → `src/concurrency/lock_free_queue.cpp`
- Thread pool implementation → `src/concurrency/thread_pool.cpp`
- Producer-consumer pattern → `src/concurrency/producer_consumer.cpp`
- Singleton, Factory, Observer, Strategy → `src/patterns/`

### Nice-to-Know
- CRTP (static polymorphism) → `src/modern_cpp/crtp.cpp`
- Visitor, Command, State patterns → `src/patterns/`
- Memory model formal semantics

## Week 3: Systems Programming + HFT

### Must-Know
- TCP/UDP sockets, epoll → `src/systems/networking/`
- Virtual memory, page tables → `src/systems/os/virtual_memory.cpp`
- Process scheduling → `src/systems/os/scheduler_sim.cpp`
- mmap, shared memory → `src/systems/linux/mmap_shared_memory.cpp`
- Signal handling → `src/systems/linux/signal_handling.cpp`
- Order book design → `src/systems/hft/order_book.cpp`
- Market data handler → `src/systems/hft/market_data_handler.cpp`

### Nice-to-Know
- Kernel bypass (DPDK, Solarflare)
- SIMD/vectorization → `src/systems/hpc_gpu/simd_vectorization.cpp`
- Custom allocators → `src/modern_cpp/custom_allocator.cpp`
- Memory pool → `src/stl_like/memory_pool.cpp`

## Week 4: DSA + Custom Data Structures + Mock Interviews

### Must-Know
- Sliding window, two pointers → `src/dsa/`
- Binary search on answer → `src/dsa/binary_search_on_answer.cpp`
- Monotonic stack → `src/dsa/monotonic_stack.cpp`
- Union-Find, Trie → `src/dsa/union_find.cpp`, `trie.cpp`
- LRU Cache → `src/stl_like/lru_cache.cpp`
- Hash map from scratch → `src/stl_like/hash_map.cpp`
- Thread-safe queue → `src/stl_like/thread_safe_queue.cpp`

### Nice-to-Know
- Segment tree → `src/dsa/segment_tree.cpp`
- Topological sort → `src/dsa/topological_sort.cpp`
- Bit manipulation tricks → `src/dsa/bit_manipulation.cpp`

## Priority by Company Type

| Topic | FAANG | HFT | GPU/HPC | Embedded |
|-------|-------|-----|---------|----------|
| Move semantics / RAII | ★★★★★ | ★★★★★ | ★★★★ | ★★★★ |
| Smart pointers | ★★★★★ | ★★★★ | ★★★ | ★★ |
| Templates / SFINAE | ★★★★ | ★★★★★ | ★★★★ | ★★★ |
| Concurrency / Atomics | ★★★★ | ★★★★★ | ★★★★★ | ★★★ |
| Lock-free structures | ★★★ | ★★★★★ | ★★★ | ★★ |
| Cache optimization | ★★★ | ★★★★★ | ★★★★★ | ★★★★ |
| DSA / Algorithms | ★★★★★ | ★★★ | ★★★ | ★★★ |
| System design | ★★★★★ | ★★★★ | ★★★ | ★★★ |
| OS internals | ★★★ | ★★★★★ | ★★★★ | ★★★★★ |
| Networking | ★★★ | ★★★★★ | ★★ | ★★★ |
| SIMD / Vectorization | ★★ | ★★★★ | ★★★★★ | ★★★ |
| Embedded constraints | ★ | ★★ | ★★ | ★★★★★ |
| Design patterns | ★★★★ | ★★★ | ★★★ | ★★★★ |
