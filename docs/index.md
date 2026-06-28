# Modern C++ Playground 🏗️

Production-quality Modern C++ interview preparation for HFT, HPC, GPU, Systems, Networking, OS, Embedded, and Low-Latency engineering roles.

## Source Modules

| Module | Files | Topics |
|--------|-------|--------|
| Modern C++ | 16 | Move semantics, smart pointers, templates, concepts, CRTP |
| Concurrency | 7 | Threads, atomics, lock-free, memory model, thread pool |
| Design Patterns | 14 | Singleton, Factory, Observer, Strategy, Visitor |
| SOLID | 1 | All 5 principles with code |
| Systems | 11 | HFT order book, networking, Linux, OS, embedded, SIMD |
| DSA | 9 | Two pointers, sliding window, monotonic stack, trie |
| Custom STL | 5 | Linked list, hash map, LRU cache, memory pool |

## Build & Run

```bash
mkdir build && cd build && cmake .. && make -j$(nproc)
./bin/modern_cpp_move_semantics
```
