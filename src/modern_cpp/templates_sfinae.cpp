/*
 * Templates and SFINAE (Substitution Failure Is Not An Error)
 *
 * WHAT: Templates enable generic programming. SFINAE is the mechanism where
 * template substitution failure removes a candidate from overload set instead
 * of causing a hard compilation error.
 *
 * WHY IT MATTERS IN INTERVIEWS: Templates + SFINAE are the backbone of C++
 * metaprogramming. Interview question: "Write a function that only accepts
 * integral types" tests SFINAE understanding directly.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Function/class template syntax and instantiation
 * - Template specialization (full and partial)
 * - enable_if for conditional overloads
 * - Type traits usage (is_integral, is_floating_point, is_same)
 * - Understanding of overload resolution with SFINAE
 *
 * COMMON PITFALLS:
 * - SFINAE only works in the "immediate context" (substitution, not body)
 * - Ambiguous overloads when SFINAE conditions overlap
 * - Forgetting typename for dependent types
 * - Template code bloat from excessive instantiations
 */

#include <iostream>
#include <type_traits>
#include <cassert>
#include <string>
#include <vector>

// === 1. Basic function template ===
template <typename T>
T maximum(T a, T b) {
    return (a > b) ? a : b;
}

// === 2. Class template with specialization ===
template <typename T>
struct TypeName {
    static constexpr const char* value = "unknown";
};

template <>
struct TypeName<int> {
    static constexpr const char* value = "int";
};

template <>
struct TypeName<double> {
    static constexpr const char* value = "double";
};

template <>
struct TypeName<std::string> {
    static constexpr const char* value = "std::string";
};

// === 3. SFINAE with enable_if classic interview question ===
// "Write a function that only accepts integral types"
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
safeAdd(T a, T b) {
    return a + b;
}

// Overload for floating point
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
safeAdd(T a, T b) {
    return a + b;
}

// === 4. SFINAE with trailing return type (C++11 style) ===
template <typename Container>
auto getSize(const Container& c) -> decltype(c.size(), size_t{}) {
    return c.size();
}

// === 5. Type traits for compile-time decisions ===
template <typename T>
void printTypeInfo() {
    std::cout << "  Type: " << TypeName<T>::value;
    std::cout << " | integral: " << std::is_integral_v<T>;
    std::cout << " | floating: " << std::is_floating_point_v<T>;
    std::cout << " | size: " << sizeof(T) << " bytes\n";
}

// === 6. Variadic templates ===
template <typename T>
T sum(T val) { return val; }

template <typename T, typename... Args>
T sum(T first, Args... rest) {
    return first + sum(rest...);
}

// === 7. SFINAE to detect if type has .begin() ===
template <typename T, typename = void>
struct is_iterable : std::false_type {};

template <typename T>
struct is_iterable<T, std::void_t<decltype(std::declval<T>().begin())>>
    : std::true_type {};

int main() {
    std::cout << "=== 1. Function templates ===\n";
    assert(maximum(3, 7) == 7);
    assert(maximum(3.14, 2.71) == 3.14);
    assert(maximum(std::string("abc"), std::string("xyz")) == "xyz");
    std::cout << "  max(3,7) = " << maximum(3, 7) << "\n";

    std::cout << "\n=== 2. Template specialization ===\n";
    printTypeInfo<int>();
    printTypeInfo<double>();
    printTypeInfo<std::string>();

    std::cout << "\n=== 3. SFINAE: only integral/floating types ===\n";
    assert(safeAdd(3, 4) == 7);
    assert(safeAdd(1.5, 2.5) == 4.0);
    // safeAdd(std::string("a"), std::string("b")); // Won't compile SFINAE
    std::cout << "  safeAdd(3, 4) = " << safeAdd(3, 4) << "\n";
    std::cout << "  safeAdd(1.5, 2.5) = " << safeAdd(1.5, 2.5) << "\n";

    std::cout << "\n=== 4. SFINAE detecting .size() ===\n";
    std::vector<int> v{1, 2, 3};
    assert(getSize(v) == 3);
    std::cout << "  vector size via SFINAE: " << getSize(v) << "\n";

    std::cout << "\n=== 5. Variadic templates ===\n";
    assert(sum(1, 2, 3, 4, 5) == 15);
    std::cout << "  sum(1,2,3,4,5) = " << sum(1, 2, 3, 4, 5) << "\n";

    std::cout << "\n=== 6. Type trait: is_iterable ===\n";
    static_assert(is_iterable<std::vector<int>>::value);
    static_assert(!is_iterable<int>::value);
    std::cout << "  vector<int> is iterable: " << is_iterable<std::vector<int>>::value << "\n";
    std::cout << "  int is iterable: " << is_iterable<int>::value << "\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
