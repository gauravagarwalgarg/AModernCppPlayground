/*
 * Lambdas in Modern C++
 *
 * WHAT: Anonymous function objects with closure semantics. Evolved significantly
 * from C++11 → C++14 (generic) → C++17 (constexpr) → C++20 (template, default).
 *
 * WHY IT MATTERS IN INTERVIEWS: Lambdas are everywhere in modern C++ STL
 * algorithms, callbacks, event handlers, policy-based design. Interviewers test
 * capture semantics and lifetime understanding.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Capture modes: [=], [&], [this], [x = expr] (init capture)
 * - Mutable lambdas (modifying captured-by-value)
 * - Generic lambdas (auto parameters)
 * - Using lambdas with STL (sort, transform, find_if)
 * - Immediately invoked lambda expressions (IIFE)
 * - Understanding lambda lifetime vs captured references
 *
 * COMMON PITFALLS:
 * - Dangling reference when lambda outlives captured variable
 * - Forgetting mutable for modifying by-value captures
 * - Capturing 'this' in async contexts (object may be destroyed)
 * - Large captures by value when reference would suffice
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include <numeric>
#include <string>
#include <memory>

int main() {
    std::cout << "=== 1. Basic lambda syntax & captures ===\n";
    int x = 10;
    int y = 20;

    auto byVal = [x]() { return x * 2; };         // Capture x by value
    auto byRef = [&x]() { return x * 2; };        // Capture x by reference
    auto byAll = [=]() { return x + y; };          // All by value
    auto byAllRef = [&]() { x += 1; return x; };  // All by reference

    assert(byVal() == 20);
    assert(byAll() == 30); // x(10) + y(20)
    byAllRef(); // modifies x
    assert(x == 11);
    assert(byRef() == 22); // Sees updated x
    std::cout << "  byVal() = " << byVal() << ", byRef() = " << byRef() << "\n";

    std::cout << "\n=== 2. Mutable lambda ===\n";
    auto counter = [count = 0]() mutable { return ++count; };
    assert(counter() == 1);
    assert(counter() == 2);
    assert(counter() == 3);
    std::cout << "  counter() called 3 times: " << counter() << "\n"; // 4

    std::cout << "\n=== 3. Generic lambdas (C++14 auto params) ===\n";
    auto multiply = [](auto a, auto b) { return a * b; };
    assert(multiply(3, 4) == 12);
    assert(multiply(2.5, 4.0) == 10.0);
    std::cout << "  multiply(3,4) = " << multiply(3, 4) << "\n";
    std::cout << "  multiply(2.5,4.0) = " << multiply(2.5, 4.0) << "\n";

    std::cout << "\n=== 4. Init capture (C++14) move into lambda ===\n";
    auto data = std::make_unique<std::vector<int>>(std::vector<int>{1, 2, 3, 4, 5});
    auto lambda = [d = std::move(data)]() {
        int sum = 0;
        for (int v : *d) sum += v;
        return sum;
    };
    assert(data == nullptr); // Moved into lambda
    assert(lambda() == 15);
    std::cout << "  Sum via moved unique_ptr: " << lambda() << "\n";

    std::cout << "\n=== 5. Lambdas with STL algorithms ===\n";
    std::vector<int> nums{5, 2, 8, 1, 9, 3, 7, 4, 6};

    // Sort descending
    std::sort(nums.begin(), nums.end(), [](int a, int b) { return a > b; });
    assert(nums[0] == 9);

    // Filter: find first even
    auto it = std::find_if(nums.begin(), nums.end(), [](int n) { return n % 2 == 0; });
    assert(*it == 8);

    // Transform: square each element
    std::vector<int> squared(nums.size());
    std::transform(nums.begin(), nums.end(), squared.begin(),
                   [](int n) { return n * n; });
    assert(squared[0] == 81); // 9*9
    std::cout << "  First sorted: " << nums[0] << ", first squared: " << squared[0] << "\n";

    std::cout << "\n=== 6. Immediately Invoked Lambda (IIFE) ===\n";
    const auto config = []() {
        std::vector<std::string> result;
        result.push_back("setting1");
        result.push_back("setting2");
        result.push_back("setting3");
        return result;
    }(); // Note the () invoked immediately
    assert(config.size() == 3);
    std::cout << "  Config initialized with " << config.size() << " items\n";

    std::cout << "\n=== 7. Lambda as callback / std::function ===\n";
    auto applyTwice = [](std::function<int(int)> f, int val) {
        return f(f(val));
    };
    auto addThree = [](int n) { return n + 3; };
    assert(applyTwice(addThree, 10) == 16); // 10+3=13, 13+3=16
    std::cout << "  applyTwice(addThree, 10) = " << applyTwice(addThree, 10) << "\n";

    std::cout << "\n=== 8. constexpr lambda (C++17) ===\n";
    constexpr auto square = [](int n) { return n * n; };
    static_assert(square(5) == 25);
    std::cout << "  constexpr square(5) = " << square(5) << "\n";

    std::cout << "\n=== 9. PITFALL: dangling reference capture ===\n";
    std::function<int()> danglingDemo;
    {
        int local = 42;
        danglingDemo = [&local]() { return local; }; // DANGER!
        assert(danglingDemo() == 42); // OK here, local still alive
    }
    // danglingDemo(); // UNDEFINED BEHAVIOR local is destroyed
    std::cout << "  Dangling reference: calling after scope = UB!\n";
    std::cout << "  Fix: capture by value [local] or extend lifetime\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
