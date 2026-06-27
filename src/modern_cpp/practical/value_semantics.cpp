/**************************************************************************************************
 * Topic: Value Semantics The Foundation of Modern C++ Design
 *
 * Interview Relevance: ★★★★★
 * What interviewers look for:
 *   - Regular types: copyable, comparable, substitutable
 *   - Value types vs entity types (when to use which)
 *   - std::span pitfalls: non-owning view invalidation
 *   - Aggregate initialization, structured bindings
 *   - Comparison operators: <=> (spaceship, C++20)
 *
 * Day-to-day application:
 *   - Domain modeling: Money, Date, Coordinate, Color
 *   - Config objects, DTOs, request/response types
 *   - Making code easier to reason about (no aliasing surprises)
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o value_semantics value_semantics.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <compare>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// 1. A proper value type Regular (copyable, equality-comparable, orderable)
// ============================================================================

struct Point {
    double x{};
    double y{};

    // C++20: defaulted spaceship operator gives all 6 comparison operators
    auto operator<=>(Point const&) const = default;
};

std::ostream& operator<<(std::ostream& os, Point const& p) {
    return os << "(" << p.x << ", " << p.y << ")";
}

// ============================================================================
// 2. Value type with invariants Encapsulated but still regular
// ============================================================================

class Temperature {
public:
    // Factory methods for different units
    static Temperature from_celsius(double c) { return Temperature{c}; }
    static Temperature from_fahrenheit(double f) { return Temperature{(f - 32.0) * 5.0 / 9.0}; }

    double celsius() const { return celsius_; }
    double fahrenheit() const { return celsius_ * 9.0 / 5.0 + 32.0; }

    // Value semantics: comparable
    auto operator<=>(Temperature const&) const = default;

    // Arithmetic
    friend Temperature operator+(Temperature a, Temperature b) {
        return Temperature{a.celsius_ + b.celsius_};
    }

private:
    explicit Temperature(double c) : celsius_(c) {}
    double celsius_;
};

std::ostream& operator<<(std::ostream& os, Temperature const& t) {
    return os << t.celsius() << "°C";
}

// ============================================================================
// 3. Aggregate value types C++20 designated initializers
// ============================================================================

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    int timeout_ms{5000};  // Default value
};

struct HttpResponse {
    int status_code;
    std::string body;
    bool ok() const { return status_code >= 200 && status_code < 300; }
};

HttpResponse handle_request(HttpRequest const& req) {
    if (req.method == "GET" && req.path == "/health") {
        return {200, "OK"};
    }
    return {404, "Not Found"};
}

// ============================================================================
// 4. Demonstrating value semantics: copies are independent
// ============================================================================

int main() {
    std::cout << "=== Value Semantics in Modern C++ ===\n\n";

    // 1. Points
    std::cout << "1. Regular types (Point):\n";
    Point p1{3.0, 4.0};
    Point p2 = p1;  // Deep copy
    Point p3{3.0, 4.0};

    assert(p1 == p3);  // Value equality
    assert(p1 == p2);  // Copy is equal
    p2.x = 99.0;
    assert(p1 != p2);  // Independent after copy
    std::cout << "   p1=" << p1 << " p2=" << p2 << " (independent copies)\n";

    // Sorting works thanks to <=>
    std::vector<Point> points{{3, 1}, {1, 2}, {2, 3}, {1, 1}};
    std::sort(points.begin(), points.end());
    std::cout << "   Sorted: ";
    for (auto const& p : points) std::cout << p << " ";
    std::cout << '\n';

    // 2. Temperature
    std::cout << "\n2. Temperature (invariant-preserving value type):\n";
    auto boiling = Temperature::from_celsius(100.0);
    auto body_temp = Temperature::from_fahrenheit(98.6);
    std::cout << "   Boiling: " << boiling << " | Body: " << body_temp << '\n';
    assert(boiling > body_temp);

    // 3. Aggregates with designated initializers (C++20)
    std::cout << "\n3. Aggregate value types:\n";
    HttpRequest req{
        .method = "GET",
        .path = "/health",
        .body = "",
        .timeout_ms = 3000};

    auto resp = handle_request(req);
    std::cout << "   " << req.method << " " << req.path
              << " → " << resp.status_code << " " << resp.body << '\n';
    assert(resp.ok());

    HttpRequest bad_req{.method = "GET", .path = "/nonexistent"};
    auto bad_resp = handle_request(bad_req);
    assert(!bad_resp.ok());

    // 4. Vectors of values copy entire collections
    std::cout << "\n4. Collections of values:\n";
    std::vector<Temperature> temps{
        Temperature::from_celsius(20.0),
        Temperature::from_celsius(25.0),
        Temperature::from_celsius(15.0)};

    auto temps_copy = temps;  // Deep copy of entire vector
    temps_copy.push_back(Temperature::from_celsius(30.0));
    assert(temps.size() == 3);      // Original unchanged
    assert(temps_copy.size() == 4); // Copy modified independently
    std::cout << "   Original size: " << temps.size()
              << " | Copy size: " << temps_copy.size() << '\n';

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
