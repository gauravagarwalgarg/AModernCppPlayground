# SOLID Principles C++ Quick Reference

→ `src/solid/solid_principles.cpp`

## Summary

| Principle | Meaning | C++ Gotcha |
|-----------|---------|------------|
| **S** Single Responsibility | One class, one reason to change | God-class anti-pattern; split into composition |
| **O** Open/Closed | Open for extension, closed for modification | Use virtual interfaces or templates, not `if/else` chains |
| **L** Liskov Substitution | Subtypes must be substitutable for base types | Don't tighten preconditions or weaken postconditions in derived |
| **I** Interface Segregation | Don't force clients to depend on unused interfaces | Prefer many small abstract classes over one fat interface |
| **D** Dependency Inversion | Depend on abstractions, not concretions | Inject `unique_ptr<Interface>` instead of concrete class |

## Real System Mappings

- **SRP:** Separate `OrderParser`, `OrderValidator`, `OrderExecutor` instead of one `OrderManager`
- **OCP:** Strategy pattern for new pricing models without touching existing engine
- **LSP:** If `Square` inherits `Rectangle`, setting width shouldn't break height assumptions
- **ISP:** Split `IDatabase` into `IReader` + `IWriter` read-only clients shouldn't know about writes
- **DIP:** Trading system depends on `IMarketFeed` interface, not `SpecificExchangeAPI`

## Common Interview Violations to Spot

| Violation | What's Wrong | Fix |
|-----------|-------------|-----|
| `switch` on type enum | OCP must modify switch for new types | Polymorphism or `std::variant` + `std::visit` |
| Derived class throws on base method | LSP breaks substitutability | Redesign hierarchy or use composition |
| Constructor creates its own dependencies | DIP hard to test | Inject via constructor parameter |
| Class with 20+ methods | SRP + ISP doing too much | Split into focused components |
| `#include` of concrete implementation | DIP tight coupling | Forward-declare, depend on interface header |

## C++-Specific Tips
- Use pure virtual classes (`= 0`) as interfaces no data members
- `final` keyword prevents unwanted inheritance (enforces closed classes)
- Concepts (C++20) can replace interface classes for compile-time DIP
- Prefer composition (`unique_ptr<Component>`) over inheritance for SRP
