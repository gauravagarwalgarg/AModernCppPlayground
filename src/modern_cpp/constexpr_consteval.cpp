/*
 * constexpr and consteval in Modern C++
 *
 * WHAT: constexpr = can be evaluated at compile time (but also at runtime).
 * consteval = MUST be evaluated at compile time (C++20). if constexpr = 
 * compile-time branch elimination (C++17).
 *
 * WHY IT MATTERS IN INTERVIEWS: Compile-time computation eliminates runtime
 * overhead entirely. HFT firms love this zero-cost abstractions, compile-time
 * hashing, lookup tables computed at build time.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Difference between const, constexpr, consteval
 * - Writing constexpr functions (fibonacci, factorial, hashing)
 * - if constexpr for compile-time branching (no dead code generation)
 * - constexpr in class constructors and member functions
 * - Understanding when constexpr is actually evaluated at compile time
 *
 * COMMON PITFALLS:
 * - constexpr function CAN run at runtime if called with runtime values
 * - consteval function CANNOT be called at runtime (compile error)
 * - Recursive constexpr limited by compiler depth
 * - Not all standard library functions are constexpr (but growing)
 */

#include <iostream>
#include <array>
#include <cassert>
#include <string_view>
#include <type_traits>
#include <cstdint>

// === 1. constexpr function compile-time fibonacci ===
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; ++i) {
        int tmp = a + b;
        a = b;
        b = tmp;
    }
    return b;
}

// === 2. constexpr compile-time string hashing (FNV-1a) ===
constexpr uint64_t fnv1a_hash(std::string_view str) {
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

// === 3. consteval guaranteed compile-time only (C++20) ===
consteval int compileTimeSquare(int x) {
    return x * x;
}

// === 4. if constexpr compile-time branching ===
template <typename T>
auto stringify(T value) {
    if constexpr (std::is_integral_v<T>) {
        return std::to_string(value) + " (integral)";
    } else if constexpr (std::is_floating_point_v<T>) {
        return std::to_string(value) + " (floating)";
    } else {
        return std::string(value); // Only compiled if T is neither
    }
}

// === 5. constexpr class ===
class Point {
    double x_, y_;
public:
    constexpr Point(double x, double y) : x_(x), y_(y) {}
    constexpr double x() const { return x_; }
    constexpr double y() const { return y_; }
    constexpr double distSquared() const { return x_ * x_ + y_ * y_; }
    constexpr Point operator+(const Point& other) const {
        return Point(x_ + other.x_, y_ + other.y_);
    }
};

// === 6. constexpr array generation at compile time ===
constexpr auto generateFibTable() {
    std::array<int, 20> table{};
    for (int i = 0; i < 20; ++i) {
        table[i] = fibonacci(i);
    }
    return table;
}

int main() {
    std::cout << "=== 1. constexpr fibonacci ===\n";
    static_assert(fibonacci(10) == 55);
    static_assert(fibonacci(0) == 0);
    static_assert(fibonacci(1) == 1);
    constexpr int fib20 = fibonacci(20);
    static_assert(fib20 == 6765);
    std::cout << "  fib(10) = " << fibonacci(10) << "\n";
    std::cout << "  fib(20) = " << fib20 << "\n";

    std::cout << "\n=== 2. Compile-time string hashing ===\n";
    constexpr auto hash1 = fnv1a_hash("hello");
    constexpr auto hash2 = fnv1a_hash("world");
    static_assert(hash1 != hash2);
    static_assert(fnv1a_hash("hello") == fnv1a_hash("hello"));
    std::cout << "  hash(\"hello\") = " << hash1 << "\n";
    std::cout << "  hash(\"world\") = " << hash2 << "\n";

    // Use in switch statement compile-time string matching!
    std::string_view cmd = "hello";
    switch (fnv1a_hash(cmd)) {
        case fnv1a_hash("hello"): std::cout << "  Matched 'hello'!\n"; break;
        case fnv1a_hash("world"): std::cout << "  Matched 'world'!\n"; break;
        default: std::cout << "  Unknown\n"; break;
    }

    std::cout << "\n=== 3. consteval must be compile-time ===\n";
    constexpr int sq = compileTimeSquare(7);
    static_assert(sq == 49);
    std::cout << "  compileTimeSquare(7) = " << sq << "\n";
    // int runtime_val = 5;
    // compileTimeSquare(runtime_val); // ERROR: not a constant expression

    std::cout << "\n=== 4. if constexpr type-dependent branching ===\n";
    std::cout << "  " << stringify(42) << "\n";
    std::cout << "  " << stringify(3.14) << "\n";
    std::cout << "  " << stringify("text") << "\n";

    std::cout << "\n=== 5. constexpr class ===\n";
    constexpr Point p1(3.0, 4.0);
    constexpr Point p2(1.0, 2.0);
    constexpr Point p3 = p1 + p2;
    static_assert(p3.x() == 4.0 && p3.y() == 6.0);
    static_assert(p1.distSquared() == 25.0);
    std::cout << "  p1 + p2 = (" << p3.x() << ", " << p3.y() << ")\n";

    std::cout << "\n=== 6. Compile-time lookup table ===\n";
    constexpr auto fib_table = generateFibTable();
    static_assert(fib_table[10] == 55);
    static_assert(fib_table[15] == 610);
    std::cout << "  Fib table[10] = " << fib_table[10] << "\n";
    std::cout << "  Fib table[15] = " << fib_table[15] << "\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
