/**************************************************************************************************
 * Topic: Modern Error Handling Beyond Exceptions
 * 
 * Interview Relevance: ★★★★★ (Every C++ role)
 * What interviewers look for:
 *   - std::expected (C++23) vs exceptions vs error codes
 *   - When NOT to use exceptions (embedded, HFT, real-time)
 *   - Monadic operations: and_then, transform, or_else
 *   - Error propagation without try/catch boilerplate
 *
 * Day-to-day application:
 *   - File I/O, network calls, parsing anywhere operations can fail
 *   - API boundaries: return rich error info without throwing
 *   - Chaining fallible operations (like Rust's Result)
 *
 * Compile: g++ -std=c++23 -Wall -Wextra -o error_handling error_handling.cpp
 *          (For C++20, use the std::optional fallback shown in the file)
 **************************************************************************************************/

#include <cassert>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>
#include <variant>

// ============================================================================
// Pre-C++23: Result type using std::variant (works everywhere)
// ============================================================================

template <typename T, typename E = std::string>
class Result {
public:
    // Success constructor
    static Result success(T value) { return Result{std::move(value)}; }
    // Error constructor
    static Result error(E err) { return Result{std::move(err), Error{}}; }

    bool has_value() const { return std::holds_alternative<T>(data_); }
    bool has_error() const { return !has_value(); }

    T const& value() const& { return std::get<T>(data_); }
    T& value() & { return std::get<T>(data_); }
    T&& value() && { return std::get<T>(std::move(data_)); }

    E const& error() const& { return std::get<E>(data_); }

    // Monadic and_then: chain operations that can fail
    template <typename F>
    auto and_then(F&& f) const -> decltype(f(value())) {
        if (has_value()) return f(value());
        return decltype(f(value()))::error(error());
    }

    // Transform: apply a function to the success value
    template <typename F>
    auto transform(F&& f) const -> Result<decltype(f(value())), E> {
        if (has_value()) return Result<decltype(f(value())), E>::success(f(value()));
        return Result<decltype(f(value())), E>::error(error());
    }

    // or_else: handle error case
    template <typename F>
    Result or_else(F&& f) const {
        if (has_value()) return *this;
        return f(error());
    }

private:
    struct Error {};  // Tag for error constructor
    explicit Result(T value) : data_(std::move(value)) {}
    Result(E err, Error) : data_(std::move(err)) {}
    std::variant<T, E> data_;
};

// ============================================================================
// Real-world example: Parsing pipeline
// ============================================================================

Result<int> parse_int(std::string_view sv) {
    int result{};
    auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), result);
    if (ec == std::errc{} && ptr == sv.data() + sv.size()) {
        return Result<int>::success(result);
    }
    return Result<int>::error("Failed to parse '" + std::string(sv) + "' as integer");
}

Result<int> validate_positive(int value) {
    if (value > 0) return Result<int>::success(value);
    return Result<int>::error("Value must be positive, got: " + std::to_string(value));
}

Result<int> validate_range(int value, int min, int max) {
    if (value >= min && value <= max) return Result<int>::success(value);
    return Result<int>::error("Value " + std::to_string(value) +
                             " out of range [" + std::to_string(min) +
                             ", " + std::to_string(max) + "]");
}

// ============================================================================
// std::optional for simple "has value or doesn't" cases
// ============================================================================

std::optional<double> safe_divide(double a, double b) {
    if (b == 0.0) return std::nullopt;
    return a / b;
}

std::optional<std::string> find_user(int id) {
    if (id == 1) return "Alice";
    if (id == 2) return "Bob";
    return std::nullopt;
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Modern Error Handling ===\n\n";

    // 1. Monadic chaining
    std::cout << "1. Monadic chaining:\n";
    auto result = parse_int("42")
                      .and_then(validate_positive)
                      .and_then([](int v) { return validate_range(v, 1, 100); });

    assert(result.has_value());
    assert(result.value() == 42);
    std::cout << "   parse_int(\"42\") → validate → range: " << result.value() << '\n';

    // Error propagation
    auto bad = parse_int("abc");
    assert(bad.has_error());
    std::cout << "   parse_int(\"abc\"): " << bad.error() << '\n';

    // Chain stops at first error
    auto chained = parse_int("-5")
                       .and_then(validate_positive);
    assert(chained.has_error());
    std::cout << "   parse_int(\"-5\") → validate_positive: " << chained.error() << '\n';

    // 2. Transform (map success values)
    std::cout << "\n2. Transform:\n";
    auto doubled = parse_int("21").transform([](int v) { return v * 2; });
    assert(doubled.has_value() && doubled.value() == 42);
    std::cout << "   parse_int(\"21\").transform(*2): " << doubled.value() << '\n';

    // 3. std::optional for simple cases
    std::cout << "\n3. std::optional:\n";
    auto div_result = safe_divide(10.0, 3.0);
    assert(div_result.has_value());
    std::cout << "   10/3 = " << *div_result << '\n';

    auto div_zero = safe_divide(10.0, 0.0);
    assert(!div_zero.has_value());
    std::cout << "   10/0 = " << div_zero.value_or(-1.0) << " (default)\n";

    // 4. Optional with monadic operations (C++23 style emulated)
    auto user = find_user(1);
    std::string greeting = user.has_value()
                               ? "Hello, " + *user + "!"
                               : "User not found";
    std::cout << "\n4. " << greeting << '\n';

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
