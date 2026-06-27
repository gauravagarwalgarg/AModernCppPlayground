/*
 * Perfect Forwarding and Universal References
 *
 * WHAT: Perfect forwarding preserves the value category (lvalue/rvalue) of
 * arguments through template functions using forwarding references (T&&) and
 * std::forward<T>. Enables zero-overhead wrapper functions.
 *
 * WHY IT MATTERS IN INTERVIEWS: Core to understanding emplace_back, make_unique,
 * and any factory/wrapper pattern. HFT firms need this for zero-copy message
 * construction and efficient forwarding layers.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Difference between rvalue reference (Widget&&) and forwarding reference (T&&)
 * - Reference collapsing rules (T& && → T&, T&& && → T&&)
 * - std::forward vs std::move (forward preserves, move always casts to rvalue)
 * - Writing factory functions that construct in-place
 * - Understanding why auto&& is a forwarding reference
 *
 * COMMON PITFALLS:
 * - T&& is only a forwarding reference when T is deduced template parameter
 * - Using std::move instead of std::forward in forwarding context
 * - Forwarding the same argument twice (moved-from state on second use)
 * - Widget&& is ALWAYS rvalue reference, never forwarding reference
 */

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <utility>
#include <memory>
#include <type_traits>

// Track copies and moves for demonstration
struct Tracked {
    std::string data;
    static int copies;
    static int moves;

    explicit Tracked(std::string s) : data(std::move(s)) {}
    Tracked(const Tracked& o) : data(o.data) { ++copies; }
    Tracked(Tracked&& o) noexcept : data(std::move(o.data)) { ++moves; }

    static void reset() { copies = 0; moves = 0; }
    static void report(const char* label) {
        std::cout << "  [" << label << "] copies=" << copies << " moves=" << moves << "\n";
        reset();
    }
};
int Tracked::copies = 0;
int Tracked::moves = 0;

// === 1. The problem: without perfect forwarding ===
class Container {
    std::vector<Tracked> items_;
public:
    // Bad: always copies (even rvalue arguments)
    void addByCopy(const Tracked& item) {
        items_.push_back(item);
    }

    // Best: perfect forwarding one function handles both
    template <typename T>
    void addForwarded(T&& item) {
        items_.push_back(std::forward<T>(item));
    }

    // Emplace-style: forward constructor arguments directly
    template <typename... Args>
    void emplaceItem(Args&&... args) {
        items_.emplace_back(std::forward<Args>(args)...);
    }

    size_t size() const { return items_.size(); }
    const Tracked& back() const { return items_.back(); }
};

// === 2. Factory function with perfect forwarding ===
template <typename T, typename... Args>
std::unique_ptr<T> makeObject(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// === 3. Reference collapsing demonstration ===
template <typename T>
void showRefType(T&&) {
    if constexpr (std::is_lvalue_reference_v<T>) {
        std::cout << "  T is lvalue reference (T& && -> T&)\n";
    } else {
        std::cout << "  T is non-reference (T&& && -> T&&)\n";
    }
}

int main() {
    std::cout << "=== 1. The copy problem ===\n";
    Container c;
    Tracked::reset();

    Tracked item("hello");
    c.addByCopy(item);
    Tracked::report("addByCopy lvalue");

    c.addByCopy(Tracked("world"));
    Tracked::report("addByCopy rvalue");

    std::cout << "\n=== 2. Perfect forwarding solution ===\n";
    Tracked lval("lvalue_data");

    c.addForwarded(lval);                  // T=Tracked& -> forward as lvalue -> copy
    Tracked::report("forward lvalue");

    c.addForwarded(Tracked("rvalue_data")); // T=Tracked -> forward as rvalue -> move
    Tracked::report("forward rvalue");

    c.addForwarded(std::move(lval));       // T=Tracked -> forward as rvalue -> move
    Tracked::report("forward moved");

    std::cout << "\n=== 3. Emplace: forward args to constructor ===\n";
    c.emplaceItem("constructed_in_place"); // Zero copies, zero moves of Tracked!
    Tracked::report("emplace");
    assert(c.back().data == "constructed_in_place");

    std::cout << "\n=== 4. Factory function ===\n";
    auto obj = makeObject<Tracked>("factory_made");
    assert(obj->data == "factory_made");
    Tracked::report("factory");
    std::cout << "  Created via perfect-forwarding factory\n";

    std::cout << "\n=== 5. Reference collapsing ===\n";
    int x = 42;
    showRefType(x);           // T deduced as int& -> int& && -> int&
    showRefType(42);          // T deduced as int  -> int&& (rvalue)
    showRefType(std::move(x)); // T deduced as int -> int&& (rvalue)

    std::cout << "\n=== 6. auto&& is also a forwarding reference ===\n";
    auto&& ref1 = x;    // int& (lvalue)
    auto&& ref2 = 42;   // int&& (rvalue, extends lifetime)
    static_assert(std::is_lvalue_reference_v<decltype(ref1)>);
    static_assert(std::is_rvalue_reference_v<decltype(ref2)>);
    std::cout << "  auto&& with lvalue: lvalue ref\n";
    std::cout << "  auto&& with rvalue: rvalue ref (lifetime extended)\n";

    std::cout << "\n=== Key takeaways ===\n";
    std::cout << "  T&&  with deduced T = forwarding reference\n";
    std::cout << "  Widget&& = always rvalue reference\n";
    std::cout << "  std::forward = conditional cast (preserves category)\n";
    std::cout << "  std::move = unconditional cast to rvalue\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
