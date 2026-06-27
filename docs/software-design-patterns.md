# Software Design Patterns Beyond GoF

Advanced design patterns from "C++ Software Design" by Klaus Iglberger, adapted for production use.

These patterns represent the **modern C++ approach** to polymorphism and extensibility: preferring value semantics, composition, and type erasure over deep inheritance hierarchies.

---

## Pattern Overview

| Pattern | Core Idea | Use When... | File |
|---------|-----------|-------------|------|
| **Type Erasure** | Polymorphism without inheritance via Concept/Model | You want value-semantic polymorphic containers | `src/patterns/software_design/type_erasure.cpp` |
| **External Polymorphism** | Add polymorphic behavior to unrelated types | You can't modify existing classes | `src/patterns/software_design/external_polymorphism.cpp` |
| **Value-Based Strategy** | `std::function` instead of virtual strategy interface | Runtime-swappable behavior | `src/patterns/software_design/value_based_strategy.cpp` |
| **Pimpl Idiom** | Bridge pattern for ABI stability | Library interfaces, compilation speed | `src/patterns/software_design/pimpl_idiom.cpp` |
| **Strong Types** | CRTP mixin for type-safe wrappers | Prevent mixing semantically different types | `src/patterns/software_design/strong_types.cpp` |
| **SBO Type Erasure** | Type erasure without heap allocation | Hot-path code, HFT, embedded | `src/patterns/software_design/small_buffer_optimization.cpp` |
| **Prototype** | Virtual clone() for polymorphic deep copy | Object registries, undo systems | `src/patterns/software_design/prototype_pattern.cpp` |
| **Modern Observer** | Signal/slot with `std::function` + RAII disconnect | Event systems, reactive programming | `src/patterns/software_design/modern_observer.cpp` |
| **Compile-Time Decorator** | Template-based decoration, zero overhead | Pricing pipelines, middleware | `src/patterns/software_design/compile_time_decorator.cpp` |
| **Runtime Decorator** | Type-erased composable decorators | Dynamic composition of behaviors | `src/patterns/software_design/runtime_decorator.cpp` |

---

## The Type Erasure Meta-Pattern

Type Erasure is the most important pattern in modern C++ design. It combines three classic patterns:

```
┌──────────────────────────────────────────────────┐
│                 TYPE ERASURE                       │
│                                                   │
│  ┌─────────────────────┐  ┌───────────────────┐  │
│  │ External Polymorphism│  │   Bridge Pattern  │  │
│  │ (Concept + Model)   │  │ (unique_ptr<Concept>)│ │
│  └─────────────────────┘  └───────────────────┘  │
│              ┌───────────────────┐                │
│              │ Prototype Pattern │                │
│              │   (clone())       │                │
│              └───────────────────┘                │
└──────────────────────────────────────────────────┘
```

**Real-world examples of Type Erasure in the Standard Library:**
- `std::function` erases any callable
- `std::any` erases any copyable type
- `std::move_only_function` (C++23) erases any movable callable
- `std::pmr::memory_resource` erases allocator implementation

---

## When to Use Which Pattern

### Use Type Erasure when:
- You want to store heterogeneous objects **by value** in a container
- You need copyable polymorphism without `clone()` visible to users
- You're building a library and want a clean, owning API

### Use External Polymorphism when:
- You can't modify the classes (third-party code)
- You want to add operations without touching existing types
- You need to combine type + behavior as a unit

### Use Pimpl when:
- You're shipping a library (ABI stability)
- Build times are unacceptable (compilation firewall)
- You want to hide platform-specific implementations

### Use Strong Types when:
- You have multiple parameters of the same underlying type
- Type safety is critical (financial calculations, units)
- You want compile-time error detection for misuse

### Use SBO Type Erasure when:
- Heap allocation is not acceptable (real-time, HFT)
- Objects are small and the buffer size is predictable
- You need value semantics without indirection overhead

---

## Key Insight: Value Semantics vs Reference Semantics

| | Reference Semantics | Value Semantics |
|--|---------------------|-----------------|
| Ownership | Shared, unclear | Clear (copy = independent) |
| Thread Safety | Requires synchronization | Naturally safe (no aliasing) |
| Lifetime | Complex (dangling refs) | Simple (scope-based) |
| Polymorphism | Virtual + pointers | Type Erasure |
| Performance | Cache-unfriendly (indirection) | Cache-friendly (contiguous) |
| Testability | Hard (shared state) | Easy (isolated copies) |

**Modern C++ prefers value semantics.** Type Erasure bridges the gap between value semantics and runtime polymorphism.

---

## Interview Questions for These Patterns

### Type Erasure
1. "How does `std::function` work internally?"
2. "Implement a simplified `std::any`."
3. "What's the performance cost of type erasure vs virtual dispatch?"
4. "How would you avoid heap allocation in type erasure?"

### External Polymorphism
1. "Add serialization to classes you can't modify."
2. "What's the difference between Visitor and External Polymorphism?"

### Pimpl
1. "Why must the destructor be defined in the .cpp file?"
2. "What's the performance trade-off of Pimpl?"
3. "How does Pimpl affect copy semantics?"

### Strong Types
1. "How do you prevent mixing up OrderId and CustomerId?"
2. "Implement a zero-overhead strong typedef."
3. "What's the CRTP mixin pattern?"

---

## Comparison: Classical vs Modern Patterns

| Classical (GoF) | Modern Equivalent | Why Modern is Better |
|----------------|-------------------|---------------------|
| Virtual Strategy | `std::function` strategy | No inheritance, copyable, any callable |
| Visitor (double dispatch) | `std::variant` + `std::visit` | Closed set, value semantics |
| Abstract Factory | Type Erasure + concepts | Open extension, no base class |
| Observer (raw ptrs) | Signal/slot + RAII connection | No dangling, automatic cleanup |
| Decorator (inheritance) | Compile-time or type-erased | Zero overhead or flexible composition |
| Singleton | Dependency injection | Testable, no global state |
