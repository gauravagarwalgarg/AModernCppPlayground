# AModernCppPlayground 🏗️

[![CI](https://github.com/gauravagarwalgarg/modern-cpp-playground/actions/workflows/ci.yml/badge.svg)](https://github.com/gauravagarwalgarg/modern-cpp-playground/actions/workflows/ci.yml) [![Docs](https://img.shields.io/badge/docs-live-blue?logo=github)](https://gauravagarwalgarg.github.io/modern-cpp-playground/) ![C++](https://img.shields.io/badge/C++-17/20/23-blue?logo=cplusplus&logoColor=white) [![License](https://img.shields.io/github/license/gauravagarwalgarg/modern-cpp-playground)](https://github.com/gauravagarwalgarg/modern-cpp-playground/blob/main/LICENSE)

> 📖 **Documentation**: [https://gauravagarwalgarg.github.io/modern-cpp-playground/](https://gauravagarwalgarg.github.io/modern-cpp-playground/)
>
> 📦 **Repository**: [GitHub](https://github.com/gauravagarwalgarg/modern-cpp-playground)


> Production-quality Modern C++ learning & interview preparation repository for HFT, HPC, GPU, Systems, Networking, OS, Embedded, and Low-Latency engineering roles.

---

## Who This Is For

**Starting fresh with C++?** See the [Learning Path](#learning-path) below and the [C++ Standards Evolution](docs/cpp-standards-evolution.md) doc.

**Preparing for interviews?** Every file is self-contained, compilable, and annotated with interview relevance and day-to-day usage context.

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
├── CMakeLists.txt                    # Root CMake build (auto-discovers all src/*.cpp)
│
├── src/
│   ├── modern_cpp/                   # C++11/14/17/20 language features
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
│   │   ├── noexcept.cpp
│   │   └── practical/               # Day-to-day modern C++ patterns
│   │       ├── value_semantics.cpp       # Regular types, spaceship operator
│   │       ├── compile_time_programming.cpp # constexpr, consteval, if constexpr
│   │       ├── raii_patterns.cpp         # ScopeGuard, TimerGuard, C API wrappers
│   │       ├── error_handling.cpp        # Result<T,E>, monadic chaining, optional
│   │       └── type_traits_concepts.cpp  # Custom concepts, constrained templates
│   │
│   ├── patterns/                     # Design Patterns
│   │   ├── singleton.cpp             # GoF: Meyer's singleton
│   │   ├── factory.cpp               # GoF: Factory method
│   │   ├── abstract_factory.cpp      # GoF: Abstract factory
│   │   ├── builder.cpp               # GoF: Builder
│   │   ├── observer.cpp              # GoF: Classic observer
│   │   ├── strategy.cpp              # GoF: Strategy
│   │   ├── command.cpp               # GoF: Command
│   │   ├── decorator.cpp             # GoF: Decorator
│   │   ├── adapter.cpp               # GoF: Adapter
│   │   ├── facade.cpp                # GoF: Facade
│   │   ├── template_method.cpp       # GoF: Template Method
│   │   ├── state.cpp                 # GoF: State
│   │   ├── visitor.cpp               # GoF: Visitor
│   │   ├── chain_of_responsibility.cpp
│   │   ├── proxy.cpp                 # GoF: Caching/Logging proxy
│   │   ├── composite.cpp             # GoF: File system tree
│   │   ├── flyweight.cpp             # GoF: Shared state optimization
│   │   ├── memento.cpp               # GoF: Undo/Redo history
│   │   ├── iterator.cpp              # GoF: Custom ring buffer iterator
│   │   ├── mediator.cpp              # GoF: Event bus
│   │   └── software_design/          # ★ Modern C++ Design Patterns
│   │       ├── type_erasure.cpp          # The core pattern of modern C++
│   │       ├── external_polymorphism.cpp # Polymorphism without modifying types
│   │       ├── value_based_strategy.cpp  # std::function strategy injection
│   │       ├── pimpl_idiom.cpp           # Bridge/Pimpl for ABI stability
│   │       ├── strong_types.cpp          # CRTP mixin for type safety
│   │       ├── small_buffer_optimization.cpp # SBO type erasure (no heap)
│   │       ├── prototype_pattern.cpp     # Virtual clone() for deep copy
│   │       ├── modern_observer.cpp       # Signal/slot with RAII connections
│   │       ├── compile_time_decorator.cpp# Zero-overhead template decoration
│   │       └── runtime_decorator.cpp     # Type-erased composable decorators
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
│   │   ├── bit_manipulation.cpp
│   │   ├── dijkstra.cpp              # Graph: shortest path
│   │   ├── kmp_string_matching.cpp   # Strings: pattern matching
│   │   ├── backtracking_nqueens.cpp  # Backtracking with pruning
│   │   ├── knapsack_01.cpp           # DP: 0/1 knapsack
│   │   ├── longest_common_subsequence.cpp  # DP: classic 2D
│   │   └── merge_sort.cpp            # Divide & conquer sort
│   │
│   └── stl_like/                     # Custom STL implementations
│       ├── doubly_linked_list.cpp
│       ├── hash_map.cpp
│       ├── lru_cache.cpp
│       ├── memory_pool.cpp
│       └── thread_safe_queue.cpp
│
├── docs/                             # Documentation
│   ├── cpp-standards-evolution.md    # ★ C++98 → C++03 → C++11 → C++14 → C++17 → C++20 → C++23 → C++26
│   ├── software-design-patterns.md   # ★ Modern C++ design patterns (Iglberger)
│   ├── modern-cpp.md                 # Quick reference: features & interview frequency
│   ├── design-patterns.md            # GoF patterns in C++
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

# Run any example (executable name = path with / replaced by _)
./bin/modern_cpp_move_semantics
./bin/modern_cpp_practical_value_semantics
./bin/patterns_software_design_type_erasure
./bin/systems_hft_order_book
./bin/dsa_monotonic_stack
./bin/stl_like_lru_cache
```

---

## Topic Coverage

| Area | Files | Interview Priority |
|------|-------|-------------------|
| Modern C++ Features | 16 + 5 practical | ★★★★★ |
| Software Design Patterns (NEW) | 10 | ★★★★★ |
| Concurrency & Lock-Free | 7 | ★★★★★ |
| GoF Design Patterns | 20 | ★★★★☆ |
| SOLID Principles | 1 | ★★★★☆ |
| HFT / Low-Latency | 3 | ★★★★★ |
| Networking | 2 | ★★★★☆ |
| Linux Programming | 2 | ★★★☆☆ |
| OS Internals | 2 | ★★★☆☆ |
| Embedded | 1 | ★★★☆☆ |
| HPC / GPU | 1 | ★★★☆☆ |
| DSA Patterns | 15 | ★★★★★ |
| Custom STL | 5 | ★★★★★ |

---

## Learning Path

### For Absolute Beginners

Start here if you're new to C++ or coming from another language:

```
📖 Read: docs/cpp-standards-evolution.md (understand the timeline)
     ↓
1️⃣  src/modern_cpp/raii.cpp                    ← Foundation of C++ design
2️⃣  src/modern_cpp/smart_pointers.cpp          ← Memory management
3️⃣  src/modern_cpp/move_semantics.cpp          ← Efficiency
4️⃣  src/modern_cpp/lambdas.cpp                 ← Modern style
5️⃣  src/modern_cpp/practical/value_semantics.cpp ← How to think in C++
     ↓
📖 Read: docs/modern-cpp.md (feature reference)
     ↓
6️⃣  src/modern_cpp/templates_sfinae.cpp        ← Generic programming
7️⃣  src/modern_cpp/concepts.cpp                ← Modern constraints
8️⃣  src/modern_cpp/practical/compile_time_programming.cpp
9️⃣  src/modern_cpp/practical/error_handling.cpp
🔟  src/modern_cpp/practical/type_traits_concepts.cpp
     ↓
📖 Read: docs/software-design-patterns.md
     ↓
1️⃣1️⃣ src/patterns/software_design/type_erasure.cpp       ← THE pattern
1️⃣2️⃣ src/patterns/software_design/external_polymorphism.cpp
1️⃣3️⃣ src/patterns/software_design/pimpl_idiom.cpp
1️⃣4️⃣ src/patterns/software_design/strong_types.cpp
```

### For Experienced Developers (Interview Prep)

Focus on what interviewers actually ask:

| Week 1 | Week 2 | Week 3 |
|--------|--------|--------|
| Move semantics | Type erasure | Lock-free queue |
| Smart pointers | SBO optimization | Thread pool |
| Perfect forwarding | Pimpl idiom | Memory model |
| Rule of Five | Strong types | Atomics |
| RAII patterns | External polymorphism | HFT systems |

### For HFT/Low-Latency Roles

```
Critical Path:
  move_semantics → custom_allocator → memory_pool →
  small_buffer_optimization → lock_free_queue →
  order_book → matching_engine → simd_vectorization
```

---

## Code Style

- **C++20** (builds with GCC 12+, Clang 14+, MSVC 19.30+)
- Every file is self-contained with `main()` + assertions
- Comments explain **why**, not just what
- Each file header states: topic, interview relevance, day-to-day application
- Complexity annotations on all functions
- No raw `new`/`delete` unless demonstrating RAII contrast
- Adapted code from external sources is attributed and re-styled for consistency

---

## Reference Repos (Source Material)

The following repos were used as source material and adapted to fit this repository's structure and style:

| Repo | What we took | Where it lives now |
|------|-------------|-------------------|
| `cpp_software_design` (Iglberger) | Type erasure, external polymorphism, SBO, strong types | `src/patterns/software_design/` |
| `design-patterns-cpp` | GoF pattern structure verification | `src/patterns/` |
| `C-Plus-Plus` (algorithms) | DSA reference implementations | `src/dsa/`, `src/stl_like/` |
| `cp-algorithms-aux` | Competitive programming algorithms | Reference only |

---

## License

MIT
