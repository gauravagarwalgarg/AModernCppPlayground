/*
 * noexcept Specifier and Operator
 *
 * WHAT: noexcept declares that a function will not throw exceptions. This
 * enables critical compiler optimizations and is required for efficient
 * move operations with STL containers.
 *
 * WHY IT MATTERS IN INTERVIEWS: std::vector won't use move semantics during
 * reallocation unless the move constructor is noexcept. This is THE reason
 * vector reallocation can be slow it falls back to copying. HFT firms
 * care deeply about this.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Why noexcept enables optimizations (no unwind tables, moves in vector)
 * - move_if_noexcept behavior (vector reallocation strategy)
 * - Conditional noexcept: noexcept(expr)
 * - noexcept operator vs specifier
 * - What happens if noexcept function throws (std::terminate)
 *
 * COMMON PITFALLS:
 * - Marking function noexcept when it can throw -> std::terminate
 * - Forgetting noexcept on move operations (vector falls back to copy)
 * - Not using conditional noexcept for generic code
 * - Destructors are implicitly noexcept (throwing in dtor = terminate)
 */

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <type_traits>
#include <chrono>
#include <utility>

// === 1. Class with noexcept move (vector will use move) ===
class FastWidget {
    std::vector<int> data_;
    std::string name_;
    static int moves_;
    static int copies_;

public:
    explicit FastWidget(std::string name, size_t sz = 100)
        : data_(sz, 42), name_(std::move(name)) {}

    // noexcept move -> vector WILL use this during reallocation
    FastWidget(FastWidget&& other) noexcept
        : data_(std::move(other.data_)), name_(std::move(other.name_)) {
        ++moves_;
    }

    FastWidget(const FastWidget& other) : data_(other.data_), name_(other.name_) {
        ++copies_;
    }

    FastWidget& operator=(FastWidget&&) noexcept = default;
    FastWidget& operator=(const FastWidget&) = default;

    static void reset() { moves_ = copies_ = 0; }
    static int moveCount() { return moves_; }
    static int copyCount() { return copies_; }
};
int FastWidget::moves_ = 0;
int FastWidget::copies_ = 0;

// === 2. Class WITHOUT noexcept move (vector will COPY!) ===
class SlowWidget {
    std::vector<int> data_;
    std::string name_;
    static int moves_;
    static int copies_;

public:
    explicit SlowWidget(std::string name, size_t sz = 100)
        : data_(sz, 42), name_(std::move(name)) {}

    // No noexcept! -> vector will COPY during reallocation
    SlowWidget(SlowWidget&& other)
        : data_(std::move(other.data_)), name_(std::move(other.name_)) {
        ++moves_;
    }

    SlowWidget(const SlowWidget& other) : data_(other.data_), name_(other.name_) {
        ++copies_;
    }

    SlowWidget& operator=(SlowWidget&&) = default;
    SlowWidget& operator=(const SlowWidget&) = default;

    static void reset() { moves_ = copies_ = 0; }
    static int moveCount() { return moves_; }
    static int copyCount() { return copies_; }
};
int SlowWidget::moves_ = 0;
int SlowWidget::copies_ = 0;

// === 3. Conditional noexcept ===
template <typename T>
class Wrapper {
    T value_;
public:
    explicit Wrapper(T val) : value_(std::move(val)) {}
    Wrapper(Wrapper&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
        : value_(std::move(other.value_)) {}
    Wrapper& operator=(Wrapper&&) noexcept(std::is_nothrow_move_assignable_v<T>) = default;
    const T& get() const { return value_; }
};

// === 4. noexcept operator (compile-time query) ===
void mayThrow() { throw 42; }
void wontThrow() noexcept {}

int main() {
    std::cout << "=== 1. noexcept move -> vector uses move ===\n";
    FastWidget::reset();
    {
        std::vector<FastWidget> vec;
        for (int i = 0; i < 100; ++i) {
            vec.emplace_back("widget_" + std::to_string(i));
        }
    }
    std::cout << "  FastWidget (noexcept move): "
              << FastWidget::moveCount() << " moves, "
              << FastWidget::copyCount() << " copies\n";
    assert(FastWidget::copyCount() == 0); // NO copies!

    std::cout << "\n=== 2. No noexcept -> vector falls back to COPY ===\n";
    SlowWidget::reset();
    {
        std::vector<SlowWidget> vec;
        for (int i = 0; i < 100; ++i) {
            vec.emplace_back("widget_" + std::to_string(i));
        }
    }
    std::cout << "  SlowWidget (throwing move): "
              << SlowWidget::moveCount() << " moves, "
              << SlowWidget::copyCount() << " copies\n";
    assert(SlowWidget::copyCount() > 0); // Copies during reallocation!

    std::cout << "\n=== 3. Conditional noexcept ===\n";
    static_assert(std::is_nothrow_move_constructible_v<Wrapper<int>>);
    static_assert(std::is_nothrow_move_constructible_v<Wrapper<std::string>>);
    std::cout << "  Wrapper<int> noexcept move: true\n";
    std::cout << "  Wrapper<string> noexcept move: true\n";

    std::cout << "\n=== 4. noexcept operator (compile-time check) ===\n";
    static_assert(!noexcept(mayThrow()));
    static_assert(noexcept(wontThrow()));
    std::cout << "  noexcept(mayThrow()) = " << noexcept(mayThrow()) << "\n";
    std::cout << "  noexcept(wontThrow()) = " << noexcept(wontThrow()) << "\n";

    // Check move_if_noexcept behavior
    FastWidget fw("test");
    auto&& ref = std::move_if_noexcept(fw);
    static_assert(std::is_rvalue_reference_v<decltype(ref)>);
    std::cout << "  move_if_noexcept(FastWidget): rvalue ref (will move)\n";

    std::cout << "\n=== 5. Performance impact ===\n";
    constexpr int N = 50000;
    FastWidget::reset(); SlowWidget::reset();

    auto t1 = std::chrono::high_resolution_clock::now();
    { std::vector<FastWidget> v; for (int i = 0; i < N; ++i) v.emplace_back("f"); }
    auto t2 = std::chrono::high_resolution_clock::now();
    { std::vector<SlowWidget> v; for (int i = 0; i < N; ++i) v.emplace_back("s"); }
    auto t3 = std::chrono::high_resolution_clock::now();

    auto fast_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto slow_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "  noexcept move (Fast): " << fast_us << " us\n";
    std::cout << "  throwing move (Slow): " << slow_us << " us\n";

    std::cout << "\n=== Key rule ===\n";
    std::cout << "  ALWAYS mark move ctor/assign noexcept if they can't throw!\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
