/*
 * std::optional, std::variant, std::any (C++17)
 *
 * WHAT: Type-safe vocabulary types replacing error-prone patterns:
 * - optional: nullable value without pointers (replaces sentinel values)
 * - variant: type-safe tagged union (replaces raw unions/inheritance)
 * - any: type-erased container (replaces void* with safety)
 *
 * WHY IT MATTERS IN INTERVIEWS: These types show modern C++ design thinking.
 * optional for function returns, variant for state machines, visit for
 * dispatch all common in production code.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - optional: when to use vs exceptions vs error codes
 * - variant: visitor pattern with std::visit, overloaded lambda trick
 * - Understanding of valueless_by_exception state
 * - Avoiding std::any in performance-critical code (heap allocation)
 *
 * COMMON PITFALLS:
 * - Accessing optional without checking (UB with *)
 * - variant becoming valueless_by_exception
 * - std::any type mismatch throws bad_any_cast (not compile-time safe)
 * - Overusing any when variant is more appropriate
 */

#include <iostream>
#include <optional>
#include <variant>
#include <any>
#include <string>
#include <vector>
#include <cassert>
#include <map>

// === 1. std::optional nullable without pointers ===
std::optional<int> findIndex(const std::vector<int>& vec, int target) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) return static_cast<int>(i);
    }
    return std::nullopt; // Not found no sentinel value needed
}

struct Config {
    std::optional<int> port;
    std::optional<std::string> host;
    std::optional<bool> verbose;
};

// === 2. std::variant type-safe union / state machine ===
struct Disconnected { std::string reason; };
struct Connecting { std::string address; int attempt; };
struct Connected { int socket_fd; double latency_ms; };

using ConnectionState = std::variant<Disconnected, Connecting, Connected>;

// Overloaded pattern (C++17 idiom for std::visit)
template <typename... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template <typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::string describeState(const ConnectionState& state) {
    return std::visit(overloaded{
        [](const Disconnected& d) { return "Disconnected: " + d.reason; },
        [](const Connecting& c) {
            return "Connecting to " + c.address + " (attempt " + std::to_string(c.attempt) + ")";
        },
        [](const Connected& c) {
            return "Connected (fd=" + std::to_string(c.socket_fd) +
                   ", latency=" + std::to_string(c.latency_ms) + "ms)";
        }
    }, state);
}

// === 3. std::any type-erased container ===
class PropertyBag {
    std::map<std::string, std::any> props_;
public:
    template <typename T>
    void set(const std::string& key, T value) {
        props_[key] = std::move(value);
    }

    template <typename T>
    std::optional<T> get(const std::string& key) const {
        auto it = props_.find(key);
        if (it == props_.end()) return std::nullopt;
        try {
            return std::any_cast<T>(it->second);
        } catch (const std::bad_any_cast&) {
            return std::nullopt;
        }
    }
};

int main() {
    std::cout << "=== 1. std::optional ===\n";
    std::vector<int> data{10, 20, 30, 40, 50};

    auto result = findIndex(data, 30);
    assert(result.has_value());
    assert(result.value() == 2);
    assert(*result == 2);

    auto missing = findIndex(data, 99);
    assert(!missing.has_value());
    assert(missing.value_or(-1) == -1);

    std::cout << "  Found 30 at index: " << *result << "\n";
    std::cout << "  Found 99 at index: " << missing.value_or(-1) << " (not found)\n";

    // Optional for config with defaults
    Config cfg{.port = 8080, .host = std::nullopt, .verbose = true};
    std::cout << "  Port: " << cfg.port.value_or(3000) << "\n";
    std::cout << "  Host: " << cfg.host.value_or("localhost") << "\n";

    std::cout << "\n=== 2. std::variant state machine ===\n";
    ConnectionState state = Disconnected{"timeout"};
    std::cout << "  " << describeState(state) << "\n";

    state = Connecting{"192.168.1.1", 1};
    std::cout << "  " << describeState(state) << "\n";

    state = Connected{42, 0.5};
    std::cout << "  " << describeState(state) << "\n";

    assert(std::holds_alternative<Connected>(state));
    auto& conn = std::get<Connected>(state);
    assert(conn.socket_fd == 42);

    std::cout << "  variant size: " << sizeof(ConnectionState) << " bytes\n";

    std::cout << "\n=== 3. std::any type-erased property bag ===\n";
    PropertyBag bag;
    bag.set("name", std::string("TradingEngine"));
    bag.set("version", 3);
    bag.set("latency", 0.42);

    assert(bag.get<std::string>("name") == "TradingEngine");
    assert(bag.get<int>("version") == 3);
    assert(!bag.get<int>("name").has_value()); // Wrong type safe failure

    std::cout << "  name: " << bag.get<std::string>("name").value() << "\n";
    std::cout << "  version: " << bag.get<int>("version").value() << "\n";
    std::cout << "  wrong type: " << bag.get<int>("name").value_or(-1) << "\n";

    std::cout << "\n=== When to use each ===\n";
    std::cout << "  optional: function may not return a value\n";
    std::cout << "  variant: value is one of N known types (state machines)\n";
    std::cout << "  any: truly unknown type (plugin systems, scripting)\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
