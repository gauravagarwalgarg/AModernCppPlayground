# C++ Standards Evolution: C++98 → C++26

A complete reference of major features introduced in each C++ standard, with practical relevance ratings.

---

## C++98 / C++03 The Classic Foundation

C++98 was the first ISO standard. C++03 was a bug-fix release (no new features, only corrections).

| Feature | Description | Still Relevant? |
|---------|-------------|-----------------|
| Classes & Inheritance | OOP foundation, virtual dispatch, access control | ★★★★★ |
| Templates | Generic programming, STL foundation | ★★★★★ |
| Exceptions | try/catch/throw, exception safety guarantees | ★★★★ |
| RTTI | `dynamic_cast`, `typeid` | ★★★ |
| Namespaces | Code organization, avoid name collisions | ★★★★★ |
| STL Containers | vector, list, map, set, deque, stack, queue | ★★★★★ |
| STL Algorithms | sort, find, transform, accumulate, etc. | ★★★★★ |
| IOStreams | cin/cout, file streams, string streams | ★★★★ |
| Function Objects | Functors (operator()), binders (bind1st/bind2nd) | ★★ (replaced by lambdas) |
| auto_ptr | Ownership transfer (BROKEN, removed in C++17) | ☆ (removed) |
| const correctness | const methods, const references, mutable | ★★★★★ |
| Operator Overloading | Custom operators for user-defined types | ★★★★★ |

**Key Idioms from this era:**
- RAII (Resource Acquisition Is Initialization)
- Rule of Three (destructor + copy ctor + copy assignment)
- Pimpl Idiom (compilation firewall)
- NVI (Non-Virtual Interface)

---

## C++11 The Modern C++ Revolution

The biggest update in C++ history. Changed how C++ is written fundamentally.

| Feature | Description | Interview Freq |
|---------|-------------|----------------|
| **Move Semantics** | Rvalue references (`&&`), `std::move`, transfer ownership | ★★★★★ |
| **Smart Pointers** | `unique_ptr`, `shared_ptr`, `weak_ptr` (replace raw new/delete) | ★★★★★ |
| **Lambda Expressions** | Inline closures with capture: `[&](int x){ return x*2; }` | ★★★★★ |
| **auto** | Type deduction for variables | ★★★★★ |
| **Range-for** | `for (auto& x : container)` | ★★★★★ |
| **nullptr** | Type-safe null pointer (replaces NULL/0) | ★★★★★ |
| **constexpr** | Compile-time evaluation of functions | ★★★★★ |
| **Variadic Templates** | Template parameter packs (`typename... Args`) | ★★★★ |
| **Static Assert** | `static_assert(condition, "message")` | ★★★★ |
| **Uniform Initialization** | Brace initialization `{}` for everything | ★★★★ |
| **Delegating Constructors** | Constructor calls another constructor | ★★★★ |
| **override / final** | Explicit virtual function overriding | ★★★★★ |
| **enum class** | Scoped enumerations (no implicit int conversion) | ★★★★★ |
| **Type Aliases (using)** | `using Vec = std::vector<int>;` (replaces typedef) | ★★★★ |
| **Thread Support** | `std::thread`, `std::mutex`, `std::condition_variable` | ★★★★★ |
| **Atomics** | `std::atomic<T>`, memory ordering | ★★★★★ |
| **Regular Expressions** | `<regex>` (note: slow in most implementations) | ★★ |
| **Random Numbers** | `<random>` (proper distributions, engines) | ★★★ |
| **Chrono** | `<chrono>` type-safe time durations and clocks | ★★★★ |
| **Initializer Lists** | `std::initializer_list<T>` | ★★★ |
| **noexcept** | Mark functions that don't throw (enables optimizations) | ★★★★ |
| **decltype** | Query the type of an expression | ★★★★ |
| **Rvalue References** | `T&&` for move semantics and perfect forwarding | ★★★★★ |
| **Perfect Forwarding** | `std::forward<T>(arg)` preserves value category | ★★★★★ |

**Key Idioms Introduced:**
- Rule of Five (add move ctor + move assignment)
- Copy-and-Swap Idiom (exception-safe assignment)
- SFINAE (`enable_if`)
- Tag Dispatch

---

## C++14 Polish and Convenience

A minor release that polished C++11 features.

| Feature | Description | Interview Freq |
|---------|-------------|----------------|
| **Generic Lambdas** | `[](auto x) { return x; }` | ★★★★ |
| **Return Type Deduction** | `auto` return type for functions | ★★★★ |
| **Variable Templates** | `template<typename T> constexpr T pi = T(3.14...)` | ★★★ |
| **Relaxed constexpr** | Loops and local variables in constexpr functions | ★★★★ |
| **Binary Literals** | `0b1010'1100` | ★★★ |
| **Digit Separators** | `1'000'000` for readability | ★★★ |
| **std::make_unique** | Factory for unique_ptr (was missing in C++11!) | ★★★★★ |
| **[[deprecated]]** | Attribute to mark deprecated APIs | ★★★ |
| **Generalized Lambda Capture** | `[x = std::move(obj)](){}` | ★★★★ |

---

## C++17 Practical Modernization

Made everyday C++ significantly more pleasant.

| Feature | Description | Interview Freq |
|---------|-------------|----------------|
| **Structured Bindings** | `auto [key, value] = map_entry;` | ★★★★ |
| **if constexpr** | Compile-time branch elimination in templates | ★★★★★ |
| **std::optional** | Type-safe nullable: `optional<T>` (no sentinel values) | ★★★★★ |
| **std::variant** | Type-safe union: `variant<int, string, double>` | ★★★★ |
| **std::any** | Type-erased container for any single value | ★★★ |
| **std::string_view** | Non-owning string reference (zero-copy) | ★★★★★ |
| **Fold Expressions** | `(args + ...)` for variadic templates | ★★★★ |
| **Class Template Argument Deduction (CTAD)** | `std::pair p{1, 2.0};` (no template args needed) | ★★★★ |
| **Inline Variables** | `inline` for variables in headers (ODR-safe) | ★★★ |
| **std::filesystem** | Portable file system operations | ★★★ |
| **Parallel Algorithms** | Execution policies: `std::execution::par` | ★★★★ |
| **std::pmr** | Polymorphic memory resources | ★★★ (HFT: ★★★★★) |
| **if/switch with initializer** | `if (auto it = m.find(k); it != m.end())` | ★★★★ |
| **Nested Namespaces** | `namespace A::B::C {}` | ★★★ |
| **[[nodiscard]]** | Warn if return value is ignored | ★★★★ |
| **[[maybe_unused]]** | Suppress unused warnings | ★★★ |
| **[[fallthrough]]** | Explicit switch fallthrough | ★★★ |
| **std::byte** | Distinct type for raw bytes (not char) | ★★★ |

**Key Idioms Enhanced:**
- Visitor pattern with `std::visit` + `std::variant`
- Overload set: `template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };`
- Deducing This (preparation)

---

## C++20 A New Language Era

As significant as C++11. Introduced concepts, ranges, coroutines, and modules.

| Feature | Description | Interview Freq |
|---------|-------------|----------------|
| **Concepts** | Named constraints: `template<Sortable T>` | ★★★★★ |
| **Ranges** | Composable, lazy view pipelines (`views::filter \| views::transform`) | ★★★★ |
| **Coroutines** | `co_await`, `co_yield`, `co_return` for async | ★★★★ |
| **Modules** | `import std;` replace headers (adoption slow) | ★★ |
| **Three-Way Comparison** | `<=>` spaceship operator (auto-generates all comparisons) | ★★★★★ |
| **Designated Initializers** | `Point{.x=1, .y=2}` | ★★★★ |
| **consteval** | Guaranteed compile-time evaluation | ★★★★ |
| **constinit** | Ensure constant initialization (no static init order fiasco) | ★★★ |
| **std::span** | Non-owning view over contiguous data | ★★★★★ |
| **std::format** | Python-style formatting: `std::format("{} {}", a, b)` | ★★★★ |
| **Calendar/Timezone** | `<chrono>` extensions for dates | ★★★ |
| **std::jthread** | Cooperative cancellation, RAII thread | ★★★★ |
| **std::stop_token** | Cooperative thread cancellation | ★★★ |
| **Abbreviated Function Templates** | `void f(auto x)` == `template<typename T> void f(T x)` | ★★★★ |
| **Template Lambdas** | `[]<typename T>(T x){ ... }` | ★★★★ |
| **constexpr Dynamic Alloc** | `new`/`delete` in constexpr (with restrictions) | ★★★ |
| **std::source_location** | Replace `__FILE__`/`__LINE__` macros | ★★★ |
| **Feature Test Macros** | `__cpp_concepts`, `__cpp_ranges`, etc. | ★★ |
| **std::bit_cast** | Type-safe reinterpretation of bits | ★★★★ |
| **std::atomic_ref** | Atomic operations on non-atomic objects | ★★★ |
| **NTTP (expanded)** | Floating-point and class types as template parameters | ★★★ |
| **Aggregate Init with ()** | `Aggregate(1, 2, 3)` syntax | ★★★ |
| **Lambda in unevaluated contexts** | Use lambdas in decltype, sizeof | ★★★ |
| **[[likely]]/[[unlikely]]** | Branch prediction hints | ★★★ (HFT: ★★★★★) |
| **using enum** | Import enum values into scope | ★★★ |

**Key Idioms:**
- Concept-constrained interfaces replace SFINAE
- Ranges pipelines replace raw iterator code
- Type Erasure with concepts

---

## C++23 Completing the Ranges Story

Fills gaps in C++20 features and adds quality-of-life improvements.

| Feature | Description | Interview Freq |
|---------|-------------|----------------|
| **std::expected** | Result type: `expected<T, E>` for error handling without exceptions | ★★★★★ |
| **Deducing this** | Explicit `this` parameter: `void foo(this Self&& self)` | ★★★★ |
| **std::print / println** | `std::print("Hello {}", name);` (replaces cout) | ★★★★ |
| **std::generator** | Coroutine-based lazy ranges | ★★★★ |
| **std::flat_map / flat_set** | Cache-friendly sorted containers | ★★★★ (HFT) |
| **std::mdspan** | Multi-dimensional span (matrices, tensors) | ★★★ |
| **Ranges Improvements** | `views::zip`, `views::chunk`, `views::stride`, `views::enumerate` | ★★★★ |
| **Monadic optional** | `.and_then()`, `.transform()`, `.or_else()` on optional | ★★★★ |
| **std::stacktrace** | Portable stack trace capture | ★★★ |
| **Multidimensional operator[]** | `matrix[i, j]` syntax | ★★★ |
| **if consteval** | Check if currently in compile-time evaluation | ★★★ |
| **std::move_only_function** | Non-copyable function (better for callbacks) | ★★★★ |
| **static operator()** | Stateless callables without object overhead | ★★★ |
| **import std;** | Standard library module (faster compilation) | ★★ |
| **size_t literals** | `42uz` for std::size_t | ★★★ |
| **[[assume]]** | Optimizer hints (UB if assumption violated) | ★★ (HFT: ★★★★) |
| **std::unreachable()** | Mark code paths as unreachable | ★★★ |

**Key Idioms:**
- `std::expected` pipelines with monadic chaining
- Deducing this for CRTP replacement
- `std::generator` for lazy data pipelines

---

## C++26 In Development (Expected ~2026)

Features are being voted into the working draft. Not all are finalized.

| Feature (Proposed/Accepted) | Description | Status |
|----------------------------|-------------|--------|
| **Reflection** | Compile-time introspection of types, members, attributes | Accepted |
| **Contracts** | Preconditions, postconditions, assertions (`pre`, `post`, `assert`) | Accepted |
| **std::execution (Senders/Receivers)** | Structured concurrency, async composition | Accepted |
| **Pattern Matching** | `inspect(x) { 42 => "answer"; _ => "other"; }` | Proposed |
| **std::inplace_vector** | Fixed-capacity vector (no heap, stack-allocated) | Accepted |
| **Trivial Relocation** | Efficient move for trivially-relocatable types | Proposed |
| **std::hive** | Colony/bucket container for stable pointer iteration | Proposed |
| **Pack Indexing** | `Args...[I]` index into parameter packs | Accepted |
| **constexpr improvements** | More stdlib usable at compile time | Ongoing |
| **Hazard Pointers** | Lock-free memory reclamation | Accepted |
| **RCU (Read-Copy-Update)** | Concurrent data structure primitive | Accepted |
| **#embed** | Include binary data as arrays at compile time | Accepted |
| **std::simd** | Portable SIMD abstraction | Proposed |

---

## Timeline Summary

```
C++98 ──── C++03 ───────────────── C++11 ── C++14 ── C++17 ── C++20 ── C++23 ── C++26
1998       2003                     2011     2014     2017     2020     2023     2026
 │          │                        │        │        │        │        │        │
 └ Foundation└ Bugfix              └ Revolution       └ Polish  └ Era    └ Fill gaps└ Reflection
                                    Move, Smart Ptr    optional  Concepts expected  Contracts
                                    Threads, Lambda    variant   Ranges   Deducing  Senders
                                    auto, constexpr    if constexpr      this
```

---

## Learning Path for Beginners

### Phase 1: Core Language (2-3 weeks)
1. Classes, inheritance, virtual functions (C++98)
2. RAII, smart pointers, move semantics (C++11)
3. Lambdas, auto, range-for (C++11)
4. Templates basics (C++98/11)

### Phase 2: Modern Idioms (2-3 weeks)
5. Rule of Five, copy elision (C++11/17)
6. std::optional, variant, string_view (C++17)
7. Structured bindings, if constexpr (C++17)
8. Perfect forwarding, SFINAE (C++11/14)

### Phase 3: Advanced Modern (2-3 weeks)
9. Concepts and Ranges (C++20)
10. Coroutines basics (C++20)
11. constexpr everything (C++20)
12. Type erasure pattern

### Phase 4: Systems & Performance
13. Threading, atomics, memory model (C++11/20)
14. Lock-free programming
15. Custom allocators, PMR (C++17)
16. SIMD, cache optimization

### Phase 5: Design & Architecture
17. SOLID principles in C++
18. Design patterns (GoF + Modern variants)
19. Software design patterns (Iglberger)
20. Error handling strategies

---

## Compiler Support Matrix

| Feature | GCC | Clang | MSVC |
|---------|-----|-------|------|
| C++20 Concepts | 10+ | 10+ | 19.30+ |
| C++20 Ranges | 10+ | 13+ | 19.30+ |
| C++20 Coroutines | 10+ | 14+ | 19.28+ |
| C++20 Modules | 11+ (partial) | 16+ (partial) | 19.28+ |
| C++23 expected | 12+ | 16+ | 19.33+ |
| C++23 Deducing this | 14+ | 18+ | 19.36+ |
| C++23 std::print | 14+ | 18+ | 19.37+ |
| C++26 Reflection | experimental | experimental | experimental |

---

## Recommended Reading

| Book | Standard Focus | Best For |
|------|---------------|----------|
| *Effective Modern C++* (Meyers) | C++11/14 | Must-read fundamentals |
| *C++ Software Design* (Iglberger) | C++17/20 | Design patterns modernized |
| *C++ Concurrency in Action* (Williams) | C++11/17 | Threading deep-dive |
| *C++ Templates: The Complete Guide* (Vandevoorde) | All | Template metaprogramming |
| *A Tour of C++* (Stroustrup) | C++20 | Quick overview for experienced devs |
