/*
 * Move Semantics in Modern C++
 * 
 * WHAT: Move semantics allow transferring ownership of resources from one object
 * to another without copying. Introduced in C++11 via rvalue references (T&&).
 *
 * WHY IT MATTERS IN INTERVIEWS: Move semantics is THE fundamental C++11 feature.
 * Every HFT/systems firm expects you to explain lvalue vs rvalue, when moves happen
 * implicitly, and the performance implications.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding of value categories (lvalue, rvalue, xvalue)
 * - Ability to implement move constructor and move assignment
 * - Knowledge of when compiler moves implicitly (return, throw, temporaries)
 * - Understanding moved-from state (valid but unspecified)
 *
 * COMMON PITFALLS:
 * - Using an object after std::move (it's in a valid but unspecified state)
 * - Forgetting to leave moved-from object in destructible state
 * - std::move doesn't move it's just a cast to rvalue reference
 * - Not marking move operations noexcept (prevents vector optimization)
 */

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <chrono>
#include <utility>

class HeavyObject {
    std::vector<int> data_;
    std::string name_;
public:
    explicit HeavyObject(std::string name, size_t size = 1000)
        : data_(size, 42), name_(std::move(name)) {
        std::cout << "  Constructed: " << name_ << "\n";
    }

    // Copy constructor expensive
    HeavyObject(const HeavyObject& other)
        : data_(other.data_), name_(other.name_ + "_copy") {
        std::cout << "  COPIED: " << name_ << "\n";
    }

    // Move constructor cheap, just pointer swap
    HeavyObject(HeavyObject&& other) noexcept
        : data_(std::move(other.data_)), name_(std::move(other.name_)) {
        std::cout << "  MOVED: " << name_ << "\n";
    }

    // Copy assignment
    HeavyObject& operator=(const HeavyObject& other) {
        if (this != &other) {
            data_ = other.data_;
            name_ = other.name_ + "_assigned";
        }
        std::cout << "  COPY ASSIGNED: " << name_ << "\n";
        return *this;
    }

    // Move assignment
    HeavyObject& operator=(HeavyObject&& other) noexcept {
        if (this != &other) {
            data_ = std::move(other.data_);
            name_ = std::move(other.name_);
        }
        std::cout << "  MOVE ASSIGNED: " << name_ << "\n";
        return *this;
    }

    size_t size() const { return data_.size(); }
    const std::string& name() const { return name_; }
};

// Demonstrates implicit move on return (NRVO may elide, but move is fallback)
HeavyObject createObject(const std::string& name) {
    HeavyObject obj(name, 5000);
    return obj; // Implicit move (or copy elision)
}

int main() {
    std::cout << "=== 1. lvalue vs rvalue ===\n";
    HeavyObject a("alpha"); // a is an lvalue
    HeavyObject b(std::move(a)); // std::move casts a to rvalue, triggers move ctor
    assert(a.size() == 0); // moved-from: valid but unspecified (vector is empty)
    assert(b.size() == 1000);

    std::cout << "\n=== 2. Performance: copy vs move with vector ===\n";
    constexpr int N = 10000;
    std::vector<std::string> source(N, "hello_world_this_is_a_long_string_for_SSO");

    auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<std::string> copied = source; // copy
    auto t2 = std::chrono::high_resolution_clock::now();
    std::vector<std::string> moved = std::move(source); // move near instant
    auto t3 = std::chrono::high_resolution_clock::now();

    auto copy_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto move_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "  Copy took: " << copy_us << " us\n";
    std::cout << "  Move took: " << move_us << " us\n";
    assert(move_us <= copy_us); // Move should be faster (or equal for tiny workloads)
    assert(moved.size() == N);
    assert(source.empty()); // moved-from vector is empty

    std::cout << "\n=== 3. Implicit move on return ===\n";
    HeavyObject c = createObject("gamma"); // RVO or implicit move
    assert(c.size() == 5000);

    std::cout << "\n=== 4. PITFALL: using object after move ===\n";
    std::string s = "important_data";
    std::string t = std::move(s);
    // s is now in valid-but-unspecified state. DO NOT rely on its value.
    std::cout << "  After move, s = \"" << s << "\" (valid but unspecified)\n";
    std::cout << "  t = \"" << t << "\"\n";
    // Safe operations on moved-from: assign new value, destroy, check empty
    s = "reassigned"; // This is fine
    assert(s == "reassigned");

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
