# AModernCppPlayground 🏗️

> Production-quality Modern C++ interview preparation repository for HFT, HPC, GPU, Systems, Networking, OS, Embedded, and Low-Latency engineering roles.

---

## Who This Is For

Candidates preparing for:
- **C++ Software Engineer** roles at FAANG, trading firms, GPU companies
- **Low-Latency / HFT** roles (Citadel, Jump, Two Sigma, Jane Street, Optiver)
- **High-Performance Computing** roles (HPC clusters, scientific computing)
- **GPU / Infrastructure** roles (NVIDIA, AMD, graphics engines)
- **Systems / Networking** roles (kernel, drivers, protocols)
- **Embedded C++** roles (automotive, IoT, RTOS)
- **OS and Concurrency** roles (scheduler, lock-free, memory model)

---

## Repository Structure

```
AModernCppPlayground/
├── CMakeLists.txt                    # Root CMake build
├── cmake/                            # CMake utilities
│
├── src/
│   ├── modern_cpp/                   # C++17/20/23 language features
│   │   ├── move_semantics.cpp
│   │   ├── smart_pointers.cpp
│   │   ├── templates_sfinae.cpp
│   │   ├── concepts.cpp
│   │   ├── constexpr_consteval.cpp
│   │   ├── lambdas.cpp
│   │   ├── raii.cpp
│   │   ├── rule_of_five.cpp
│   │   ├── optional_variant_any.cpp
│   │   ├── structured_bindings.cpp
│   │   ├── ranges.cpp
│   │   ├── crtp.cpp
│   │   ├── perfect_forwarding.cpp
│   │   ├── custom_allocator.cpp
│   │   ├── copy_elision.cpp
│   │   └── noexcept.cpp
│   │
│   ├── concurrency/                  # Threading, atomics, lock-free
│   │   ├── threads_basics.cpp
│   │   ├── mutex_condvar.cpp
│   │   ├── atomics.cpp
│   │   ├── memory_model.cpp
│   │   ├── lock_free_queue.cpp
│   │   ├── thread_pool.cpp
│   │   └── producer_consumer.cpp
│   │
│   ├── patterns/                     # GoF Design Patterns
│   │   ├── singleton.cpp
│   │   ├── factory.cpp
│   │   ├── abstract_factory.cpp
│   │   ├── builder.cpp
│   │   ├── observer.cpp
│   │   ├── strategy.cpp
│   │   ├── command.cpp
│   │   ├── decorator.cpp
│   │   ├── adapter.cpp
│   │   ├── facade.cpp
│   │   ├── template_method.cpp
│   │   ├── state.cpp
│   │   ├── visitor.cpp
│   │   └── chain_of_responsibility.cpp
│   │
│   ├── solid/                        # SOLID principles with code
│   │   └── solid_principles.cpp
│   │
│   ├── systems/                      # Industry domain code
│   │   ├── hft/                      # HFT / Low-latency
│   │   │   ├── order_book.cpp
│   │   │   ├── market_data_handler.cpp
│   │   │   └── matching_engine.cpp
│   │   ├── networking/               # Sockets, protocols
│   │   │   ├── tcp_server.cpp
│   │   │   └── epoll_server.cpp
│   │   ├── linux/                    # Linux programming
│   │   │   ├── signal_handling.cpp
│   │   │   └── mmap_shared_memory.cpp
│   │   ├── os/                       # OS concepts
│   │   │   ├── virtual_memory.cpp
│   │   │   └── scheduler_sim.cpp
│   │   ├── embedded/                 # Embedded C++
│   │   │   └── state_machine.cpp
│   │   └── hpc_gpu/                  # HPC / GPU ecosystem
│   │       └── simd_vectorization.cpp
│   │
│   ├── dsa/                          # DSA patterns in C++
│   │   ├── two_pointers.cpp
│   │   ├── sliding_window.cpp
│   │   ├── binary_search_on_answer.cpp
│   │   ├── monotonic_stack.cpp
│   │   ├── union_find.cpp
│   │   ├── trie.cpp
│   │   ├── segment_tree.cpp
│   │   ├── topological_sort.cpp
│   │   └── bit_manipulation.cpp
│   │
│   └── stl_like/                     # Custom STL implementations
│       ├── doubly_linked_list.cpp
│       ├── hash_map.cpp
│       ├── lru_cache.cpp
│       ├── memory_pool.cpp
│       └── thread_safe_queue.cpp
│
├── docs/                             # Documentation
│   ├── modern-cpp.md
│   ├── design-patterns.md
│   ├── solid-principles.md
│   ├── hft-low-latency.md
│   ├── networking.md
│   ├── linux-programming.md
│   ├── hpc-gpu.md
│   ├── os-internals.md
│   ├── embedded-cpp.md
│   ├── dsa-patterns.md
│   ├── concurrency.md
│   └── interview-roadmap.md
│
├── tests/                            # Test programs
├── benchmarks/                       # Performance benchmarks
└── scripts/                          # Build helpers
```

---

## Build & Run

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run any example
./bin/modern_cpp_move_semantics
./bin/systems_hft_order_book
./bin/dsa_monotonic_stack
./bin/stl_like_lru_cache
```

---

## Topic Coverage

| Area | Files | Interview Priority |
|------|-------|-------------------|
| Modern C++ Features | 16 | ★★★★★ |
| Concurrency & Lock-Free | 7 | ★★★★★ |
| Design Patterns | 14 | ★★★★☆ |
| SOLID Principles | 1 | ★★★★☆ |
| HFT / Low-Latency | 3 | ★★★★★ |
| Networking | 2 | ★★★★☆ |
| Linux Programming | 2 | ★★★☆☆ |
| OS Internals | 2 | ★★★☆☆ |
| Embedded | 1 | ★★★☆☆ |
| HPC / GPU | 1 | ★★★☆☆ |
| DSA Patterns | 9 | ★★★★★ |
| Custom STL | 5 | ★★★★★ |

---

## Code Style

- **C++17 minimum**, C++20 where beneficial
- Every file is self-contained with `main()` + assertions
- Comments explain **why**, not just what
- Each file header states: topic, interview relevance, what interviewers look for
- Complexity annotations on all functions
- No raw `new`/`delete` unless demonstrating RAII contrast

---

## License

MIT
