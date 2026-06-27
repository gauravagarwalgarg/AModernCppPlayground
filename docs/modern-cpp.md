# Modern C++ Quick Reference

Features covered in this repo with interview frequency ratings.

## C++11/14 Features

| Feature | One-liner | File | Interview Freq |
|---------|-----------|------|----------------|
| Move Semantics | Transfer ownership without copying; rvalue refs (`&&`) | `src/modern_cpp/move_semantics.cpp` | ★★★★★ |
| Smart Pointers | Automatic lifetime management (unique, shared, weak) | `src/modern_cpp/smart_pointers.cpp` | ★★★★★ |
| RAII | Resource acquisition is initialization; deterministic cleanup | `src/modern_cpp/raii.cpp` | ★★★★★ |
| Rule of Five | Define dtor + copy/move ctor + copy/move assign together | `src/modern_cpp/rule_of_five.cpp` | ★★★★★ |
| Perfect Forwarding | Preserve value category through template layers via `std::forward` | `src/modern_cpp/perfect_forwarding.cpp` | ★★★★ |
| Lambda Expressions | Inline closures with capture semantics | `src/modern_cpp/lambdas.cpp` | ★★★★ |
| Templates & SFINAE | Compile-time polymorphism; substitution failure is not an error | `src/modern_cpp/templates_sfinae.cpp` | ★★★★ |
| Copy Elision / RVO | Compiler omits copy/move in return statements | `src/modern_cpp/copy_elision.cpp` | ★★★★ |
| CRTP | Curiously Recurring Template Pattern for static polymorphism | `src/modern_cpp/crtp.cpp` | ★★★★ |
| noexcept | Marks functions that won't throw; enables move optimizations | `src/modern_cpp/noexcept.cpp` | ★★★ |
| Custom Allocators | Control memory allocation strategy (pool, arena) | `src/modern_cpp/custom_allocator.cpp` | ★★★ |

## C++17 Features

| Feature | One-liner | File | Interview Freq |
|---------|-----------|------|----------------|
| Structured Bindings | `auto [a, b] = pair;` decompose aggregates | `src/modern_cpp/structured_bindings.cpp` | ★★★ |
| std::optional/variant/any | Type-safe nullable, union, and type-erased containers | `src/modern_cpp/optional_variant_any.cpp` | ★★★ |
| if constexpr | Compile-time branch elimination in templates | `src/modern_cpp/templates_sfinae.cpp` | ★★★ |
| Fold expressions | Variadic template parameter pack expansion | `src/modern_cpp/templates_sfinae.cpp` | ★★ |

## C++20 Features

| Feature | One-liner | File | Interview Freq |
|---------|-----------|------|----------------|
| Concepts | Named constraints on template parameters | `src/modern_cpp/concepts.cpp` | ★★★ |
| Ranges | Composable, lazy view pipelines over sequences | `src/modern_cpp/ranges.cpp` | ★★ |
| constexpr (extended) | Almost everything computable at compile time | `src/modern_cpp/constexpr_consteval.cpp` | ★★★ |
| consteval | Guaranteed compile-time-only evaluation | `src/modern_cpp/constexpr_consteval.cpp` | ★★ |
| Coroutines | Cooperative multitasking with co_await/co_yield | (not yet in repo) | ★★ |
| Modules | Replace headers with import declarations | (not yet in repo) | ★ |

## Top 5 Interview Topics (by frequency)

1. **Move semantics** "Explain std::move. When is it useful? What happens after a move?"
2. **Smart pointers** "unique_ptr vs shared_ptr ownership model. Circular reference fix?"
3. **RAII** "How does RAII prevent resource leaks? Show a real example."
4. **Templates** "SFINAE vs Concepts. How does template deduction work?"
5. **Perfect forwarding** "What problem does std::forward solve? Universal references?"
