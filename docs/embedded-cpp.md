# Embedded C++ Interview Guide

## Constraints & Rules

| Constraint | Reason | Alternative |
|-----------|--------|-------------|
| No exceptions | Stack unwinding is non-deterministic, code size bloat | Return codes, `std::expected` (C++23), error states |
| No RTTI | Memory overhead for type_info | `static_cast`, `std::variant`, enums |
| No/controlled heap | Fragmentation, non-deterministic allocation time | Stack allocation, static pools, placement new |
| No STL containers (sometimes) | Hidden allocations, exception paths | Custom fixed-size containers |

→ `src/systems/embedded/state_machine.cpp`

## Key Techniques

### Static Polymorphism (CRTP)
```cpp
template<typename Derived>
struct Sensor {
    int read() { return static_cast<Derived*>(this)->read_impl(); }
};
struct TempSensor : Sensor<TempSensor> {
    int read_impl() { return /* hw register */; }
};
// Zero vtable overhead, fully inlineable
```

### constexpr Everything
- Compute lookup tables at compile time
- Validate configurations statically
- `static_assert` for hardware register checks

### volatile
```cpp
volatile uint32_t* const GPIO_PORT = reinterpret_cast<volatile uint32_t*>(0x40020000);
*GPIO_PORT = 0x01;  // Compiler MUST NOT optimize away or reorder
```
- Required for memory-mapped I/O and hardware registers
- Does NOT provide atomicity or thread synchronization

### Memory-Mapped I/O
```cpp
struct GPIO_Registers {
    volatile uint32_t MODER;
    volatile uint32_t OTYPER;
    volatile uint32_t ODR;
};
auto* gpio = reinterpret_cast<GPIO_Registers*>(GPIOA_BASE);
```

### Interrupt Handlers (ISR)
- Keep short: set flag, return. Process in main loop
- No heap allocation, no blocking calls, no `printf`
- Use `volatile` flag or atomic for communication with main loop
- Disable/enable interrupts for critical sections (`__disable_irq()`)

## Standards

| Standard | Focus | Industry |
|----------|-------|----------|
| MISRA C++ | Safety-critical rules, subset of C++ | Automotive, aerospace |
| AUTOSAR C++14 | Modern C++ guidelines for MISRA-like safety | Automotive |
| CERT C++ | Security-focused coding rules | General embedded |
| DO-178C | Avionics software certification | Aerospace |

## Common Interview Questions

| Question | Key Answer |
|----------|------------|
| Why no exceptions in embedded? | Non-deterministic timing, code size increase (~15-20%), stack usage unknown |
| volatile vs atomic? | volatile = no optimization. atomic = thread-safe + memory ordering |
| How to debug without printf? | SWD/JTAG debugger, LED toggling, logic analyzer, ITM trace |
| Static vs dynamic memory | Static: deterministic, no fragmentation. Dynamic: flexible but risky |
| Watchdog timer purpose | Reset MCU if software hangs (must be periodically "kicked") |
| How to handle ISR communication? | volatile flag, ring buffer, or atomic operations |
