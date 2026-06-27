/**************************************************************************************************
 * Topic: Type Traits & Concepts Constraining Templates for Readable Code
 *
 * Interview Relevance: ★★★★★ (C++20 and beyond)
 * What interviewers look for:
 *   - Writing custom concepts
 *   - Constraining function templates (requires clause vs concept auto)
 *   - SFINAE vs Concepts (evolution story)
 *   - Combining concepts with conjunction/disjunction
 *   - Practical use: enable_if (legacy) → concepts (modern)
 *
 * Day-to-day application:
 *   - Generic library code with clear error messages
 *   - Overload resolution based on type properties
 *   - Constraining APIs to prevent misuse at compile time
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o type_traits_concepts type_traits_concepts.cpp
 **************************************************************************************************/

#include <cassert>
#include <concepts>
#include <cstdlib>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// ============================================================================
// 1. Custom concepts Readable constraints on templates
// ============================================================================

// Basic concept: type must support arithmetic operations
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Concept: type must be hashable (has std::hash specialization)
template <typename T>
concept Hashable = requires(T t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};

// Concept: type must be a container with size() and begin/end
template <typename T>
concept Container = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.begin() } -> std::input_or_output_iterator;
    { t.end() } -> std::input_or_output_iterator;
    typename T::value_type;
};

// Concept: type must be printable to ostream
template <typename T>
concept Printable = requires(std::ostream& os, T const& t) {
    { os << t } -> std::same_as<std::ostream&>;
};

// Compound concept
template <typename T>
concept PrintableNumeric = Numeric<T> && Printable<T>;

// ============================================================================
// 2. Constrained functions Much clearer than SFINAE
// ============================================================================

// Old way (SFINAE):
// template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
// T old_add(T a, T b) { return a + b; }

// New way (Concepts):
template <Numeric T>
T add(T a, T b) {
    return a + b;
}

// Alternative syntax with requires clause:
template <typename T>
    requires Container<T> && Printable<typename T::value_type>
void print_container(T const& container) {
    std::cout << "[ ";
    for (auto const& elem : container) {
        std::cout << elem << " ";
    }
    std::cout << "](size=" << container.size() << ")\n";
}

// Abbreviated function template (auto with concept):
void process(Numeric auto value) {
    std::cout << "Processing numeric: " << value * 2 << '\n';
}

// ============================================================================
// 3. Concept-based overloading (replaces tag dispatch / SFINAE overloads)
// ============================================================================

template <typename T>
std::string to_debug_string(T const& value) {
    if constexpr (std::is_integral_v<T>) {
        return "int:" + std::to_string(value);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float:" + std::to_string(value);
    } else if constexpr (Container<T>) {
        return "container[size=" + std::to_string(value.size()) + "]";
    } else {
        return "unknown";
    }
}

// ============================================================================
// 4. requires expressions for complex constraints
// ============================================================================

template <typename T>
concept Serializable = requires(T const& t) {
    { t.serialize() } -> std::convertible_to<std::string>;
} && requires(std::string const& data) {
    { T::deserialize(data) } -> std::same_as<T>;
};

struct UserData {
    std::string name;
    int age;

    std::string serialize() const {
        return name + ":" + std::to_string(age);
    }

    static UserData deserialize(std::string const& data) {
        auto pos = data.find(':');
        return {data.substr(0, pos), std::stoi(data.substr(pos + 1))};
    }
};

// Only accepts Serializable types
template <Serializable T>
std::string round_trip(T const& obj) {
    auto serialized = obj.serialize();
    auto deserialized = T::deserialize(serialized);
    return deserialized.serialize();
}

// ============================================================================
// 5. subsumption Concept specificity for overload resolution
// ============================================================================

template <typename T>
concept Integral = std::integral<T>;

template <typename T>
concept SignedIntegral = Integral<T> && std::signed_integral<T>;

// Compiler picks the more specific overload
void classify(Integral auto v) { std::cout << "  Integral: " << v << '\n'; }
void classify(SignedIntegral auto v) { std::cout << "  SignedIntegral: " << v << '\n'; }

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Type Traits & Concepts ===\n\n";

    // 1. Constrained add
    std::cout << "1. Constrained add:\n";
    auto i = add(3, 4);
    auto d = add(1.5, 2.5);
    // add("hello", "world");  // Won't compile: string not Numeric
    std::cout << "   add(3,4)=" << i << " add(1.5,2.5)=" << d << '\n';
    assert(i == 7);

    // 2. Container printing
    std::cout << "\n2. Container printing:\n   ";
    std::vector<int> vec{1, 2, 3, 4, 5};
    print_container(vec);

    // 3. Abbreviated template
    std::cout << "\n3. Abbreviated template:\n   ";
    process(42);

    // 4. Debug string via concepts
    std::cout << "\n4. to_debug_string:\n";
    std::cout << "   " << to_debug_string(42) << '\n';
    std::cout << "   " << to_debug_string(3.14) << '\n';
    std::cout << "   " << to_debug_string(vec) << '\n';

    // 5. Serializable concept
    std::cout << "\n5. Serializable concept:\n";
    UserData user{"Alice", 30};
    auto result = round_trip(user);
    std::cout << "   Round-trip: " << result << '\n';
    assert(result == "Alice:30");

    // 6. Subsumption
    std::cout << "\n6. Subsumption (most-specific overload):\n";
    classify(42);         // Picks SignedIntegral (more specific)
    classify(42u);        // Picks Integral (unsigned not signed)

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
