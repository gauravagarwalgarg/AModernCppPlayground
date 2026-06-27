/**
 * @file state_machine.cpp
 * @brief Embedded-Style State Machine - LED Controller
 * 
 * Design principles for embedded state machines:
 *   - enum class for states and events: type-safe, no implicit conversions
 *   - Compile-time transition table: no dynamic allocation, predictable timing
 *   - No virtual functions: avoid vtable indirection in hard real-time
 *   - All transitions explicit: every (state, event) pair accounted for
 * 
 * Pattern: Table-driven state machine (common in automotive, aerospace firmware)
 * Alternative patterns: Switch/case (simple), State pattern (OOP, uses heap)
 * 
 * Compile: g++ -std=c++20 -Wall -pthread state_machine.cpp -o state_machine
 */

#include <iostream>
#include <array>
#include <cstdint>

// States and Events as enum class (type-safe, scoped)
enum class State : uint8_t { Off, Blinking, Solid, Error, COUNT };
enum class Event : uint8_t { PowerOn, PowerOff, Toggle, Fault, Reset, COUNT };

// String conversion (constexpr for compile-time)
constexpr const char* stateToString(State s) {
    switch (s) {
        case State::Off:      return "Off";
        case State::Blinking: return "Blinking";
        case State::Solid:    return "Solid";
        case State::Error:    return "Error";
        default:              return "Unknown";
    }
}

constexpr const char* eventToString(Event e) {
    switch (e) {
        case Event::PowerOn:  return "PowerOn";
        case Event::PowerOff: return "PowerOff";
        case Event::Toggle:   return "Toggle";
        case Event::Fault:    return "Fault";
        case Event::Reset:    return "Reset";
        default:              return "Unknown";
    }
}

// Action function type (no heap, no std::function)
using ActionFn = void(*)();

// Transition table entry
struct Transition {
    State next_state;
    ActionFn action;
};

// Actions (simulating hardware control)
void actionLedOn()    { std::cout << "    [HW] LED ON\n"; }
void actionLedOff()   { std::cout << "    [HW] LED OFF\n"; }
void actionBlink()    { std::cout << "    [HW] LED BLINK mode\n"; }
void actionAlarm()    { std::cout << "    [HW] ERROR: LED rapid flash!\n"; }
void actionNone()     { /* no-op */ }

// Compile-time transition table: [State][Event] → {NextState, Action}
// Every combination is explicitly defined - no undefined behavior
constexpr size_t NUM_STATES = static_cast<size_t>(State::COUNT);
constexpr size_t NUM_EVENTS = static_cast<size_t>(Event::COUNT);

// State machine with static table
class LEDController {
    State current_state_ = State::Off;

    // Transition table (could be constexpr in C++23 with constexpr function pointers)
    static inline const Transition table_[NUM_STATES][NUM_EVENTS] = {
        // State::Off
        {{State::Solid, actionLedOn}, {State::Off, actionNone}, {State::Blinking, actionBlink},
         {State::Error, actionAlarm}, {State::Off, actionNone}},
        // State::Blinking
        {{State::Blinking, actionNone}, {State::Off, actionLedOff}, {State::Solid, actionLedOn},
         {State::Error, actionAlarm}, {State::Off, actionLedOff}},
        // State::Solid
        {{State::Solid, actionNone}, {State::Off, actionLedOff}, {State::Blinking, actionBlink},
         {State::Error, actionAlarm}, {State::Off, actionLedOff}},
        // State::Error
        {{State::Error, actionNone}, {State::Off, actionLedOff}, {State::Error, actionNone},
         {State::Error, actionAlarm}, {State::Off, actionLedOff}},
    };

public:
    State getState() const noexcept { return current_state_; }

    void processEvent(Event event) {
        size_t s = static_cast<size_t>(current_state_);
        size_t e = static_cast<size_t>(event);

        const auto& transition = table_[s][e];

        std::cout << "  " << stateToString(current_state_) << " + "
                  << eventToString(event) << " → " << stateToString(transition.next_state) << "\n";

        transition.action();
        current_state_ = transition.next_state;
    }
};

int main() {
    std::cout << "=== Embedded State Machine: LED Controller ===\n\n";
    std::cout << "States: Off, Blinking, Solid, Error\n";
    std::cout << "Events: PowerOn, PowerOff, Toggle, Fault, Reset\n\n";

    LEDController led;

    // Demonstrate various transitions
    Event scenario[] = {
        Event::PowerOn,   // Off → Solid
        Event::Toggle,    // Solid → Blinking
        Event::Toggle,    // Blinking → Solid
        Event::Fault,     // Solid → Error
        Event::PowerOn,   // Error → Error (stuck in error!)
        Event::Reset,     // Error → Off (only reset works)
        Event::PowerOn,   // Off → Solid (back to normal)
        Event::PowerOff,  // Solid → Off
    };

    for (Event e : scenario) {
        led.processEvent(e);
    }

    std::cout << "\nFinal state: " << stateToString(led.getState()) << "\n";
    std::cout << "\nNote: Zero heap allocations, deterministic timing, all transitions compile-time defined.\n";

    return 0;
}
