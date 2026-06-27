/**************************************************************************************************
 * Topic: Strong Types via CRTP Mixin Classes
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★ (FAANG, HFT, Safety-Critical)
 * What interviewers look for:
 *   - Preventing implicit conversions between semantically different types
 *   - CRTP mixin pattern for composable operator overloading
 *   - Tag types for distinguishing otherwise identical underlying types
 *   - Zero-overhead abstraction (same assembly as raw types)
 *
 * Day-to-day application:
 *   - Prevent mixing up "meters" and "kilometers" in calculations
 *   - Order IDs vs Customer IDs in databases (both int64_t underneath)
 *   - Durations: seconds vs milliseconds
 *   - Financial: prevent mixing currencies
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o strong_types strong_types.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

// ============================================================================
// CRTP Mixin Skills provide operators automatically
// ============================================================================

template <typename Derived>
struct Addable {
    friend Derived operator+(Derived const& lhs, Derived const& rhs) {
        return Derived{lhs.get() + rhs.get()};
    }
    friend Derived& operator+=(Derived& lhs, Derived const& rhs) {
        lhs.get() += rhs.get();
        return lhs;
    }
};

template <typename Derived>
struct Subtractable {
    friend Derived operator-(Derived const& lhs, Derived const& rhs) {
        return Derived{lhs.get() - rhs.get()};
    }
};

template <typename Derived>
struct Comparable {
    friend bool operator==(Derived const& lhs, Derived const& rhs) {
        return lhs.get() == rhs.get();
    }
    friend bool operator!=(Derived const& lhs, Derived const& rhs) {
        return !(lhs == rhs);
    }
    friend bool operator<(Derived const& lhs, Derived const& rhs) {
        return lhs.get() < rhs.get();
    }
};

template <typename Derived>
struct Printable {
    friend std::ostream& operator<<(std::ostream& os, Derived const& d) {
        return os << d.get();
    }
};

// ============================================================================
// The StrongType template variadic CRTP skills
// ============================================================================

template <typename T, typename Tag, template <typename> class... Skills>
class StrongType : public Skills<StrongType<T, Tag, Skills...>>... {
public:
    using value_type = T;

    explicit constexpr StrongType(T value) : value_(std::move(value)) {}

    constexpr T& get() noexcept { return value_; }
    constexpr T const& get() const noexcept { return value_; }

private:
    T value_;
};

// ============================================================================
// Concrete strong types same underlying type, DIFFERENT C++ types
// ============================================================================

using Meters     = StrongType<double, struct MetersTag, Addable, Subtractable, Comparable, Printable>;
using Kilometers = StrongType<double, struct KilometersTag, Addable, Subtractable, Comparable, Printable>;
using OrderId    = StrongType<int64_t, struct OrderIdTag, Comparable, Printable>;
using CustomerId = StrongType<int64_t, struct CustomerIdTag, Comparable, Printable>;

// ============================================================================
// Functions are type-safe can't accidentally pass Kilometers where Meters expected
// ============================================================================

Meters add_distances(Meters a, Meters b) {
    return a + b;
}

// This would NOT compile:
// Meters add_wrong(Meters a, Kilometers b) { return a + b; }  // ERROR!

void process_order(OrderId id) {
    std::cout << "Processing order #" << id << '\n';
}

// This would NOT compile:
// void bad_call() { process_order(CustomerId{42}); }  // ERROR!

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    Meters m1{100.0};
    Meters m2{50.0};
    // Kilometers km{1.0};  // Can't mix with Meters!

    auto m3 = add_distances(m1, m2);
    assert(m3.get() == 150.0);
    std::cout << "Distance: " << m3 << "m\n";

    OrderId order{12345};
    CustomerId customer{67890};
    // process_order(customer);  // Won't compile! Type safety!
    process_order(order);

    assert(order != OrderId{99999});
    assert(OrderId{1} < OrderId{2});

    std::cout << "All assertions passed.\n";
    return EXIT_SUCCESS;
}
