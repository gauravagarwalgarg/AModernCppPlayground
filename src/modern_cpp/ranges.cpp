/*
 * C++20 Ranges: Views, Pipelines, and Lazy Evaluation
 *
 * WHAT: Ranges generalize iterators with composable, lazy view pipelines.
 * Instead of raw loops or chained algorithms with temporary containers,
 * you compose filter|transform|take operations that execute lazily.
 *
 * WHY IT MATTERS IN INTERVIEWS: Ranges represent the future of C++ data
 * processing. They show modern C++20 fluency. Pipelines are more readable
 * than nested algorithm calls and avoid intermediate allocations.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding lazy evaluation (views don't allocate)
 * - Pipe syntax (|) for composing range adaptors
 * - Common adaptors: filter, transform, take, drop, reverse
 * - Difference between views (lazy) and actions (eager)
 * - Range concepts (range, input_range, sized_range)
 *
 * COMMON PITFALLS:
 * - Views don't own data dangling if source is destroyed
 * - Infinite ranges need take/take_while to terminate
 * - Some views are single-pass (input ranges)
 * - views::filter can change element count can't index efficiently
 */

#include <iostream>
#include <ranges>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace views = std::views;
namespace ranges = std::ranges;

// Helper to print a range
template <ranges::input_range R>
void printRange(const char* label, R&& r) {
    std::cout << "  " << label << ": [";
    bool first = true;
    for (const auto& elem : r) {
        if (!first) std::cout << ", ";
        std::cout << elem;
        first = false;
    }
    std::cout << "]\n";
}

int main() {
    std::vector<int> numbers{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    std::cout << "=== 1. Basic views: filter, transform, take ===\n";
    auto pipeline = numbers
        | views::filter([](int n) { return n % 2 == 0; })
        | views::transform([](int n) { return n * n; })
        | views::take(4);

    printRange("even squared|take(4)", pipeline);
    std::vector<int> result;
    for (int v : pipeline) result.push_back(v);
    assert(result == (std::vector<int>{4, 16, 36, 64}));

    std::cout << "\n=== 2. Lazy evaluation (no intermediate containers) ===\n";
    int evaluations = 0;
    auto lazy = numbers
        | views::filter([&](int n) { ++evaluations; return n > 10; })
        | views::transform([&](int n) { ++evaluations; return n * 2; })
        | views::take(3);

    std::cout << "  Before iteration: evaluations = " << evaluations << "\n";
    assert(evaluations == 0); // Nothing computed yet!

    std::vector<int> materialized;
    for (int v : lazy) materialized.push_back(v);
    std::cout << "  After iteration: evaluations = " << evaluations << "\n";
    printRange("lazy result", materialized);
    assert(materialized == (std::vector<int>{22, 24, 26}));

    std::cout << "\n=== 3. views::iota (infinite range) ===\n";
    auto squares = views::iota(1)
        | views::transform([](int n) { return n * n; })
        | views::take(10);
    printRange("first 10 squares", squares);

    std::cout << "\n=== 4. views::drop and views::reverse ===\n";
    auto mid = numbers | views::drop(5) | views::take(5);
    printRange("drop(5)|take(5)", mid);

    auto rev = numbers | views::take(5) | views::reverse;
    printRange("take(5)|reverse", rev);

    std::cout << "\n=== 5. String processing with ranges ===\n";
    std::vector<std::string> words_vec{"Hello", "World", "C++20", "Ranges"};
    auto long_words = words_vec
        | views::filter([](const std::string& s) { return s.size() > 4; });
    std::cout << "  Long words: ";
    for (const auto& w : long_words) std::cout << "[" << w << "] ";
    std::cout << "\n";

    std::cout << "\n=== 6. ranges::algorithms (direct range overloads) ===\n";
    std::vector<int> data{5, 3, 8, 1, 9, 2, 7};
    ranges::sort(data);
    assert(ranges::is_sorted(data));
    printRange("sorted", data);

    auto it = ranges::find(data, 7);
    assert(it != data.end() && *it == 7);

    auto [mn, mx] = ranges::minmax(data);
    assert(mn == 1 && mx == 9);
    std::cout << "  min=" << mn << " max=" << mx << "\n";

    std::cout << "\n=== 7. Composability (building reusable pipelines) ===\n";
    auto topNSquaredEvens = [](int n) {
        return views::filter([](int x) { return x % 2 == 0; })
             | views::transform([](int x) { return x * x; })
             | views::take(n);
    };

    auto top3 = numbers | topNSquaredEvens(3);
    printRange("top3 squared evens", top3);
    std::vector<int> top3_vec;
    for (int v : top3) top3_vec.push_back(v);
    assert(top3_vec == (std::vector<int>{4, 16, 36}));

    std::cout << "\n=== Key benefits over raw loops ===\n";
    std::cout << "  - Composable (pipe operations together)\n";
    std::cout << "  - Lazy (no intermediate allocations)\n";
    std::cout << "  - Readable (declarative intent)\n";
    std::cout << "  - Safe (range concepts prevent misuse)\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
