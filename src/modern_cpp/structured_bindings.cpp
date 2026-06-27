/*
 * Structured Bindings (C++17)
 *
 * WHAT: Decompose objects (pairs, tuples, arrays, structs) into named variables
 * with a single declaration. Syntax: auto [x, y, z] = expression;
 *
 * WHY IT MATTERS IN INTERVIEWS: Shows comfort with modern C++ syntax. Used
 * heavily in range-based loops over maps, multiple return values, and
 * pattern-like decomposition. Clean, readable code.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding what types support structured bindings
 * - auto& vs auto vs const auto& semantics
 * - Using with maps, tuples, pairs, custom structs
 * - Multiple return values pattern
 * - Performance awareness (avoiding copies)
 *
 * COMMON PITFALLS:
 * - Binding by value when you want reference (unexpected copies)
 * - Cannot bind to bitfields
 * - Names are aliases (auto&), not copies modifying affects original
 * - Doesn't work with private members (need public or get<> interface)
 */

#include <iostream>
#include <map>
#include <unordered_map>
#include <tuple>
#include <array>
#include <vector>
#include <cassert>
#include <string>
#include <utility>
#include <algorithm>

// === Struct for decomposition ===
struct Point3D {
    double x, y, z;
};

struct TradeResult {
    bool success;
    double price;
    int quantity;
    std::string message;
};

// Multiple return values via struct
TradeResult executeTrade(const std::string& symbol, int qty) {
    if (symbol == "AAPL") {
        return {true, 150.25, qty, "filled"};
    }
    return {false, 0.0, 0, "rejected: unknown symbol"};
}

// Multiple return values via tuple
std::tuple<int, int, int> parseDate(const std::string& /*date*/) {
    return {2024, 6, 15}; // year, month, day
}

int main() {
    std::cout << "=== 1. Arrays ===\n";
    int arr[] = {10, 20, 30};
    auto [a, b, c] = arr;
    assert(a == 10 && b == 20 && c == 30);
    std::cout << "  [" << a << ", " << b << ", " << c << "]\n";

    std::cout << "\n=== 2. std::pair ===\n";
    std::pair<std::string, int> p{"hello", 42};
    auto& [word, num] = p;  // Reference binding no copy
    assert(word == "hello");
    word = "modified";       // Modifies p.first!
    assert(p.first == "modified");
    std::cout << "  pair: {" << p.first << ", " << p.second << "}\n";

    std::cout << "\n=== 3. std::tuple ===\n";
    auto [year, month, day] = parseDate("2024-06-15");
    assert(year == 2024 && month == 6 && day == 15);
    std::cout << "  Date: " << year << "-" << month << "-" << day << "\n";

    std::cout << "\n=== 4. Structs ===\n";
    Point3D pt{1.0, 2.0, 3.0};
    auto [x, y, z] = pt;
    assert(x == 1.0 && y == 2.0 && z == 3.0);
    std::cout << "  Point: (" << x << ", " << y << ", " << z << ")\n";

    // Struct return value
    auto [success, price, quantity, msg] = executeTrade("AAPL", 100);
    assert(success && price == 150.25 && quantity == 100);
    std::cout << "  Trade: " << msg << " @ " << price << "\n";

    std::cout << "\n=== 5. Map iteration (the killer use case) ===\n";
    std::map<std::string, int> portfolio{
        {"AAPL", 100}, {"GOOGL", 50}, {"MSFT", 200}, {"TSLA", 75}
    };

    std::cout << "  Portfolio:\n";
    for (const auto& [symbol, shares] : portfolio) {
        std::cout << "    " << symbol << ": " << shares << " shares\n";
    }

    // Modifying values through reference binding
    for (auto& [symbol, shares] : portfolio) {
        shares *= 2;
    }
    assert(portfolio["AAPL"] == 200);

    std::cout << "\n=== 6. With insert/emplace return values ===\n";
    auto [iter, inserted] = portfolio.insert({"NVDA", 150});
    assert(inserted);
    assert(iter->second == 150);
    std::cout << "  Inserted " << iter->first << ": " << inserted << "\n";

    auto [iter2, inserted2] = portfolio.insert({"AAPL", 999});
    assert(!inserted2);
    assert(iter2->second == 200);
    std::cout << "  Insert AAPL again: " << inserted2 << " (already exists)\n";

    std::cout << "\n=== 7. std::array ===\n";
    std::array<double, 3> rgb{0.5, 0.8, 0.2};
    const auto& [r, g, bl] = rgb;
    std::cout << "  RGB: (" << r << ", " << g << ", " << bl << ")\n";
    assert(r == 0.5);

    std::cout << "\n=== 8. Nested with algorithms ===\n";
    std::vector<std::pair<std::string, double>> prices{
        {"AAPL", 150.0}, {"GOOGL", 140.0}, {"MSFT", 380.0}
    };
    auto maxIt = std::max_element(prices.begin(), prices.end(),
        [](const auto& a2, const auto& b2) { return a2.second < b2.second; });
    auto [maxSymbol, maxPrice] = *maxIt;
    assert(maxSymbol == "MSFT");
    std::cout << "  Most expensive: " << maxSymbol << " @ " << maxPrice << "\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
