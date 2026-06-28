/**************************************************************************************************
 * Topic: Compile-Time Programming constexpr, consteval, and if constexpr
 *
 * Interview Relevance: ★★★★★ (HFT, Embedded, FAANG)
 * What interviewers look for:
 *   - Moving computation from runtime to compile-time
 *   - constexpr vs consteval: when each is appropriate
 *   - if constexpr for compile-time branching (no dead code generation)
 *   - Compile-time containers and algorithms (C++20)
 *   - static_assert for compile-time validation
 *
 * Day-to-day application:
 *   - Lookup tables computed at compile time (CRC, trig tables)
 *   - Config validation at compile time (catch errors early)
 *   - Template metaprogramming made readable
 *   - Zero-cost abstractions in embedded systems
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o compile_time_programming compile_time_programming.cpp
 **************************************************************************************************/

#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string_view>
#include <type_traits>

// ============================================================================
// 1. constexpr: CAN be evaluated at compile time, also works at runtime
// ============================================================================

constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; ++i) result *= i;
    return result;
}

// Compile-time check: this value is computed by the compiler
static_assert(factorial(5) == 120);
static_assert(factorial(0) == 1);

// ============================================================================
// 2. consteval: MUST be evaluated at compile time (C++20)
// ============================================================================

consteval int compile_time_hash(std::string_view sv) {
    // FNV-1a hash
    unsigned int hash = 2166136261u;
    for (char c : sv) {
        hash ^= static_cast<unsigned int>(c);
        hash *= 16777619u;
    }
    return static_cast<int>(hash);
}

// These are guaranteed compile-time constants
constexpr int HASH_GET = compile_time_hash("GET");
constexpr int HASH_POST = compile_time_hash("POST");
static_assert(HASH_GET != HASH_POST);

// ============================================================================
// 3. Compile-time lookup table generation
// ============================================================================

constexpr auto generate_squares() {
    std::array<int, 16> table{};
    for (int i = 0; i < 16; ++i) {
        table[i] = i * i;
    }
    return table;
}

// Table computed at compile time zero runtime cost
constexpr auto SQUARES = generate_squares();
static_assert(SQUARES[4] == 16);
static_assert(SQUARES[10] == 100);

// ============================================================================
// 4. if constexpr Compile-time branching (eliminates dead branches)
// ============================================================================

template <typename T>
std::string type_name() {
    if constexpr (std::is_integral_v<T>) {
        if constexpr (std::is_signed_v<T>) return "signed integer";
        else return "unsigned integer";
    } else if constexpr (std::is_floating_point_v<T>) {
        return "floating point";
    } else if constexpr (std::is_pointer_v<T>) {
        return "pointer";
    } else {
        return "other";
    }
}

// Generic serializer using if constexpr
template <typename T>
std::string serialize(T const& value) {
    if constexpr (std::is_arithmetic_v<T>) {
        return std::to_string(value);
    } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>) {
        return "\"" + std::string(value) + "\"";
    } else if constexpr (std::is_same_v<T, bool>) {
        return value ? "true" : "false";
    } else {
        static_assert(std::is_arithmetic_v<T>, "Unsupported type for serialization");
    }
}

// ============================================================================
// 5. Constexpr validation catch errors at compile time
// ============================================================================

struct Config {
    int port;
    int max_connections;
    int timeout_seconds;

    constexpr bool is_valid() const {
        return port > 0 && port < 65536 &&
               max_connections > 0 && max_connections <= 10000 &&
               timeout_seconds > 0;
    }
};

constexpr Config DEFAULT_CONFIG{8080, 100, 30};
static_assert(DEFAULT_CONFIG.is_valid(), "Default config is invalid!");

// This would fail at compile time:
// constexpr Config BAD_CONFIG{-1, 100, 30};
// static_assert(BAD_CONFIG.is_valid());  // ERROR: "Default config is invalid!"

// ============================================================================
// 6. Compile-time fibonacci (showing constexpr with loops, not recursion)
// ============================================================================

constexpr auto fibonacci(int n) -> long long {
    if (n <= 1) return n;
    long long a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        long long temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

static_assert(fibonacci(10) == 55);
static_assert(fibonacci(20) == 6765);

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Compile-Time Programming ===\n\n";

    // 1. constexpr at runtime too
    std::cout << "1. factorial(10) = " << factorial(10) << '\n';

    // 2. Compile-time hash for switch-case on strings
    std::cout << "2. Hash(\"GET\") = " << HASH_GET << '\n';
    std::cout << "   Hash(\"POST\") = " << HASH_POST << '\n';

    // 3. Lookup table
    std::cout << "3. Squares table: ";
    for (int i = 0; i < 8; ++i) std::cout << SQUARES[i] << " ";
    std::cout << "...\n";

    // 4. if constexpr
    std::cout << "4. Type names:\n";
    std::cout << "   int: " << type_name<int>() << '\n';
    std::cout << "   unsigned: " << type_name<unsigned>() << '\n';
    std::cout << "   double: " << type_name<double>() << '\n';
    std::cout << "   int*: " << type_name<int*>() << '\n';

    // 5. Serialization
    std::cout << "5. Serialize: " << serialize(42) << ", "
              << serialize(3.14) << ", " << serialize(std::string("hello")) << '\n';

    // 6. Fibonacci
    std::cout << "6. fibonacci(30) = " << fibonacci(30) << '\n';

    // Assertions
    assert(factorial(5) == 120);
    assert(SQUARES[7] == 49);
    assert(fibonacci(10) == 55);

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
