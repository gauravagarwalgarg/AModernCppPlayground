# DSA Pattern Recognition Guide

## Given Problem Keywords → Pattern to Try

| # | Pattern | Signals / Keywords | Time Complexity | File |
|---|---------|-------------------|-----------------|------|
| 1 | Sliding Window | "subarray", "substring", "window of size k", "contiguous" | O(n) | `src/dsa/sliding_window.cpp` |
| 2 | Two Pointers | "sorted array", "pair sum", "remove duplicates", "palindrome" | O(n) | `src/dsa/two_pointers.cpp` |
| 3 | Binary Search on Answer | "minimum maximum", "capacity", "kth smallest", "feasibility check" | O(n log range) | `src/dsa/binary_search_on_answer.cpp` |
| 4 | Monotonic Stack | "next greater", "previous smaller", "histogram", "temperatures" | O(n) | `src/dsa/monotonic_stack.cpp` |
| 5 | Union-Find | "connected components", "grouping", "redundant connection", "accounts" | O(α(n)) | `src/dsa/union_find.cpp` |
| 6 | Trie | "prefix search", "autocomplete", "word dictionary", "longest prefix" | O(L) per op | `src/dsa/trie.cpp` |
| 7 | Topological Sort | "prerequisites", "course schedule", "build order", DAG dependencies | O(V+E) | `src/dsa/topological_sort.cpp` |
| 8 | Segment Tree | "range query + update", "interval sum/min/max", "mutable array" | O(log n) per op | `src/dsa/segment_tree.cpp` |
| 9 | Bit Manipulation | "XOR", "single number", "power of 2", "subsets", "toggle" | O(n) or O(2^n) | `src/dsa/bit_manipulation.cpp` |
| 10 | BFS/DFS | "shortest path (unweighted)", "number of islands", "tree traversal" | O(V+E) | |
| 11 | Dynamic Programming | "optimal", "count ways", "min/max cost", "can you reach" | Varies | |
| 12 | Greedy | "earliest deadline", "intervals", "huffman", "local optimal → global" | O(n log n) | |
| 13 | Backtracking | "all combinations", "permutations", "N-Queens", "Sudoku" | O(k^n) | |
| 14 | Divide & Conquer | "merge sort", "closest pair", "count inversions" | O(n log n) | |
| 15 | Heap / Priority Queue | "kth largest", "top K", "merge K lists", "median stream" | O(n log k) | |
| 16 | Graph (Dijkstra) | "shortest path (weighted)", "network delay", "minimum cost" | O(E log V) | |
| 17 | LRU/LFU Cache | "design cache", "eviction policy", "O(1) get/put" | O(1) | `src/stl_like/lru_cache.cpp` |
| 18 | Hash Map Design | "O(1) lookup", "two sum", "group anagrams", "frequency count" | O(1) avg | `src/stl_like/hash_map.cpp` |
| 19 | Interval Merge | "overlapping intervals", "meeting rooms", "insert interval" | O(n log n) | |
| 20 | Fast/Slow Pointer | "cycle detection", "middle of list", "happy number" | O(n) | |

## Decision Flowchart

```
Is it about a subarray/substring?
├─ Fixed size window → Sliding Window
├─ Variable size with condition → Sliding Window (shrink)
└─ Sorted input → Two Pointers

Is it about optimization?
├─ "Minimum of maximum" or vice versa → Binary Search on Answer
├─ Overlapping subproblems → DP
└─ Each local choice leads to global optimum → Greedy

Is it about graph/tree?
├─ Connectivity/grouping → Union-Find
├─ Dependency order → Topological Sort
├─ Shortest path → BFS (unweighted) / Dijkstra (weighted)
└─ All paths / combinations → DFS + Backtracking

Is it about range queries?
├─ Static → Prefix sum
├─ Mutable + point update → Segment Tree / BIT
└─ Intervals → Sort by start, sweep line
```
