/**************************************************************************************************
 * Topic: Compile-Time Decorator Template-Based Extension Without Virtual Dispatch
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★ (HFT, Performance)
 * What interviewers look for:
 *   - Zero-overhead decoration via templates (no vtable, full inlining)
 *   - Concepts to constrain decorator composition
 *   - NTTP (Non-Type Template Parameters) for compile-time configuration
 *   - Comparison with runtime decorator (trade-offs)
 *
 * Day-to-day application:
 *   - Pricing engines: discount → tax → rounding pipeline
 *   - Middleware stacks resolved at compile time
 *   - Zero-cost logging wrappers in release builds
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o compile_time_decorator compile_time_decorator.cpp
 **************************************************************************************************/

#include <cassert>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

// ============================================================================
// Money type (value type for currency calculations)
// ============================================================================

struct Money {
    uint64_t cents{};  // Store in cents to avoid floating-point issues

    constexpr Money() = default;
    constexpr explicit Money(uint64_t c) : cents(c) {}
};

constexpr Money operator+(Money lhs, Money rhs) noexcept {
    return Money{lhs.cents + rhs.cents};
}

template <typename T>
    requires std::is_arithmetic_v<T>
constexpr Money operator*(Money m, T factor) {
    return Money{static_cast<uint64_t>(m.cents * factor)};
}

std::ostream& operator<<(std::ostream& os, Money m) {
    return os << "$" << m.cents / 100 << "." << (m.cents % 100 < 10 ? "0" : "")
              << m.cents % 100;
}

// ============================================================================
// Concept: what makes something a "priced item"?
// ============================================================================

template <typename T>
concept PricedItem = requires(T item) {
    { item.price() } -> std::same_as<Money>;
    { item.name() } -> std::convertible_to<std::string>;
};

// ============================================================================
// Concrete items
// ============================================================================

class ConferenceTicket {
public:
    ConferenceTicket(std::string name, Money price)
        : name_(std::move(name)), price_(price) {}

    std::string const& name() const { return name_; }
    Money price() const { return price_; }

private:
    std::string name_;
    Money price_;
};

class Book {
public:
    Book(std::string title, Money price)
        : title_(std::move(title)), price_(price) {}

    std::string const& name() const { return title_; }
    Money price() const { return price_; }

private:
    std::string title_;
    Money price_;
};

// ============================================================================
// Compile-time decorators template wrappers with zero overhead
// ============================================================================

// Discount decorator (compile-time percentage)
template <int DiscountPercent, PricedItem Item>
class Discounted {
public:
    static_assert(DiscountPercent > 0 && DiscountPercent < 100,
                  "Discount must be between 1-99%");

    template <typename... Args>
    explicit Discounted(Args&&... args) : item_{std::forward<Args>(args)...} {}

    Money price() const {
        return item_.price() * (1.0 - DiscountPercent / 100.0);
    }

    std::string name() const {
        return item_.name() + " (-" + std::to_string(DiscountPercent) + "%)";
    }

private:
    Item item_;
};

// Tax decorator (compile-time tax rate)
template <int TaxPercent, PricedItem Item>
class Taxed {
public:
    static_assert(TaxPercent > 0, "Tax rate must be positive");

    template <typename... Args>
    explicit Taxed(Args&&... args) : item_{std::forward<Args>(args)...} {}

    Money price() const {
        return item_.price() * (1.0 + TaxPercent / 100.0);
    }

    std::string name() const {
        return item_.name() + " (+" + std::to_string(TaxPercent) + "% tax)";
    }

private:
    Item item_;
};

// ============================================================================
// Usage all decoration resolved at compile time, fully inlined
// ============================================================================

int main() {
    // CppCon ticket: $499, 20% early bird discount, 15% tax
    // (499*0.80)*1.15 = $459.08 → in cents: 49900*0.80*1.15 = 45908
    Taxed<15, Discounted<20, ConferenceTicket>> ticket{
        "CppCon 2024", Money{49900}};

    std::cout << "Item: " << ticket.name() << '\n';
    std::cout << "Price: " << ticket.price() << '\n';

    // Book: $40, 10% member discount
    Discounted<10, Book> book{"Effective Modern C++", Money{4000}};
    std::cout << "Item: " << book.name() << '\n';
    std::cout << "Price: " << book.price() << '\n';

    // Verify pricing
    // 49900 * 0.80 = 39920 cents, * 1.15 = 45908 cents
    assert(ticket.price().cents == 45908);
    // 4000 * 0.90 = 3600 cents
    assert(book.price().cents == 3600);

    std::cout << "\nAll assertions passed. Zero virtual dispatch overhead!\n";
    return EXIT_SUCCESS;
}
