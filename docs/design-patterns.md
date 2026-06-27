# Design Patterns C++ Interview Guide

## Pattern Selection Guide

| Pattern | Use When... | C++ Specifics | File |
|---------|-------------|---------------|------|
| Singleton | Exactly one instance, global access | Meyer's singleton (static local), thread-safe since C++11 | `src/patterns/singleton.cpp` |
| Factory | Create objects without specifying exact class | Return `unique_ptr<Base>`, virtual ctor idiom | `src/patterns/factory.cpp` |
| Abstract Factory | Families of related objects | Template factories, concept-constrained | `src/patterns/abstract_factory.cpp` |
| Builder | Complex object construction step-by-step | Fluent interface with `*this` return, `std::optional` fields | `src/patterns/builder.cpp` |
| Observer | Event notification to multiple listeners | `std::function` callbacks, weak_ptr to avoid dangling | `src/patterns/observer.cpp` |
| Strategy | Swap algorithm at runtime | `std::function`, templates for compile-time strategy | `src/patterns/strategy.cpp` |
| Command | Encapsulate requests as objects | Undo/redo stacks, `std::function` + lambda | `src/patterns/command.cpp` |
| Decorator | Add behavior without inheritance explosion | Forwarding constructors, `std::reference_wrapper` | `src/patterns/decorator.cpp` |
| Adapter | Incompatible interface conversion | Private inheritance or composition | `src/patterns/adapter.cpp` |
| Facade | Simplify complex subsystem | Single header exposing clean API | `src/patterns/facade.cpp` |
| State | Object behavior changes with internal state | `std::variant` for states (no heap), or unique_ptr<State> | `src/patterns/state.cpp` |
| Template Method | Define skeleton, defer steps to subclasses | NVI (Non-Virtual Interface) idiom | `src/patterns/template_method.cpp` |
| Visitor | Add operations without modifying classes | `std::variant` + `std::visit` (modern), or double dispatch | `src/patterns/visitor.cpp` |
| Chain of Responsibility | Pass request along handler chain | `std::unique_ptr<Handler> next_` linked list | `src/patterns/chain_of_responsibility.cpp` |
| CRTP | Static polymorphism, mixin behavior | No vtable overhead, compile-time dispatch | `src/modern_cpp/crtp.cpp` |

## Top 5 Most Asked in C++ Interviews

### 1. Singleton
- Thread-safe via static local (C++11 guarantees)
- When NOT to use: testing, dependency injection preferred
- **Interview trap:** "Make it thread-safe" → it already is with Meyer's singleton

### 2. Factory / Abstract Factory
- Key: return `std::unique_ptr<Base>` caller owns the object
- Registration pattern: map of string → creator function
- **Interview Q:** "How would you add a new type without modifying the factory?"

### 3. Observer
- Modern C++: use `std::function` + connection management
- **Danger:** dangling references. Use `weak_ptr` or explicit unsubscribe
- **Interview Q:** "How do you handle observer lifetime?"

### 4. Strategy
- Compile-time: template parameter (zero overhead)
- Runtime: `std::function<Signature>` or virtual interface
- **Interview Q:** "Strategy vs Template Method?"

### 5. CRTP (Curiously Recurring Template Pattern)
```cpp
template<typename Derived>
struct Base {
    void interface() { static_cast<Derived*>(this)->impl(); }
};
struct Concrete : Base<Concrete> {
    void impl() { /* ... */ }
};
```
- **Interview Q:** "How does CRTP avoid virtual dispatch overhead?"
