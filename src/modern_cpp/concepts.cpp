/*
 * C++20 Concepts
 *
 * WHAT: Concepts are named constraints on template parameters. They replace
 * SFINAE with readable, declarative syntax and produce clear error messages.
 *
 * WHY IT MATTERS IN INTERVIEWS: Concepts show you're current with C++20.
 * Interviewers want to see you replace ugly enable_if with clean constraints.
 * Shows deep understanding of templates and type requirements.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Defining custom concepts with requires expressions
 * - Using concepts to constrain templates (requires clause, shorthand)
 * - Replacing SFINAE with concepts
 * - Subsumption (more constrained overload wins)
 * - Understanding of concept satisfaction vs modeling
 *
 * COMMON PITFALLS:
 * - Concepts check syntax, not semantics (satisfies != models)
 * - Overcomplicating concepts (keep them focused)
 * - Forgetting that concept satisfaction is checked at instantiation
 * - Not leveraging standard library concepts (<concepts> header)
 */

#include <iostream>
#include <concepts>
#include <type_traits>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <functional>

// === 1. Basic concept definition ===
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

template <typename T>
concept Integral = std::integral<T>; // Using standard concept

// === 2. Custom concept with requires expression ===
template <typename T>
concept Sortable = requires(T container) {
    { container.begin() } -> std::input_or_output_iterator;
    { container.end() } -> std::input_or_output_iterator;
    { container.size() } -> std::convertible_to<std::size_t>;
    requires std::totally_ordered<typename T::value_type>;
};

template <typename T>
concept Hashable = requires(T val) {
    { std::hash<T>{}(val) } -> std::convertible_to<std::size_t>;
};

// === 3. Using concepts to constrain three syntax forms ===
// Form 1: requires clause
template <typename T> requires Numeric<T>
T add(T a, T b) { return a + b; }

// Form 2: trailing requires
template <typename T>
T multiply(T a, T b) requires Numeric<T> { return a * b; }

// Form 3: shorthand (constrained template parameter)
template <Integral T>
T bitwiseAnd(T a, T b) { return a & b; }

// === 4. Replacing SFINAE with concepts ===
// Old way (SFINAE): enable_if<is_integral_v<T>, T>::type process(T)
// New way (concepts):
template <std::integral T>
std::string describe(T val) {
    return "integer: " + std::to_string(val);
}

template <std::floating_point T>
std::string describe(T val) {
    return "floating: " + std::to_string(val);
}

// === 5. Concept for sortable containers ===
template <Sortable Container>
void sortContainer(Container& c) {
    std::sort(c.begin(), c.end());
}

// === 6. Compound concepts ===
template <typename T>
concept Printable = requires(std::ostream& os, T val) {
    { os << val } -> std::same_as<std::ostream&>;
};

template <typename T>
concept PrintableNumeric = Numeric<T> && Printable<T>;

template <PrintableNumeric T>
void printValue(T val) {
    std::cout << "  Value: " << val << "\n";
}

// === 7. Requires expression as ad-hoc constraint ===
template <typename T>
    requires requires(T x) { x + x; x * x; }
auto squareAndAdd(T x) { return x * x + x; }

int main() {
    std::cout << "=== 1. Basic constrained functions ===\n";
    assert(add(3, 4) == 7);
    assert(add(1.5, 2.5) == 4.0);
    assert(multiply(3, 4) == 12);
    assert(bitwiseAnd(0b1100, 0b1010) == 0b1000);
    // add(std::string("a"), std::string("b")); // Compile error: clear message!
    std::cout << "  add(3,4) = " << add(3, 4) << "\n";

    std::cout << "\n=== 2. Overloading with concepts (replaces SFINAE) ===\n";
    assert(describe(42) == "integer: 42");
    assert(describe(3.14).substr(0, 9) == "floating:");
    std::cout << "  " << describe(42) << "\n";
    std::cout << "  " << describe(3.14) << "\n";

    std::cout << "\n=== 3. Sortable concept ===\n";
    std::vector<int> nums{5, 2, 8, 1, 9};
    sortContainer(nums);
    assert(std::is_sorted(nums.begin(), nums.end()));
    std::cout << "  Sorted: ";
    for (int n : nums) std::cout << n << " ";
    std::cout << "\n";

    std::cout << "\n=== 4. Hashable concept check ===\n";
    static_assert(Hashable<int>);
    static_assert(Hashable<std::string>);
    static_assert(!Hashable<std::vector<int>>); // vector has no std::hash
    std::cout << "  int is Hashable: true\n";
    std::cout << "  vector<int> is Hashable: false\n";

    std::cout << "\n=== 5. Compound concepts ===\n";
    printValue(42);
    printValue(3.14);

    std::cout << "\n=== 6. Ad-hoc requires ===\n";
    assert(squareAndAdd(3) == 12); // 9 + 3
    std::cout << "  squareAndAdd(3) = " << squareAndAdd(3) << "\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
