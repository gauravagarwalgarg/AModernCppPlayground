/**************************************************************************************************
 * Topic: 0/1 Knapsack Classic Dynamic Programming
 *
 * Interview Relevance: ★★★★★
 * Time: O(n * W)
 * Space: O(W) with space optimization
 *
 * What interviewers look for:
 *   - State definition: dp[i][w] = max value using items 0..i-1 with capacity w
 *   - Transition: include item or skip it
 *   - Space optimization: 1D array, iterate capacity backwards
 *   - Item reconstruction (which items were selected)
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o knapsack_01 knapsack_01.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

struct Item {
    int weight;
    int value;
    std::string name;
};

// Space-optimized 1D DP
int knapsack_max_value(std::vector<Item> const& items, int capacity) {
    std::vector<int> dp(capacity + 1, 0);

    for (auto const& item : items) {
        // Iterate backwards to avoid using same item twice
        for (int w = capacity; w >= item.weight; --w) {
            dp[w] = std::max(dp[w], dp[w - item.weight] + item.value);
        }
    }

    return dp[capacity];
}

// Full 2D DP with item reconstruction
std::pair<int, std::vector<int>> knapsack_with_items(std::vector<Item> const& items, int capacity) {
    int n = static_cast<int>(items.size());
    // dp[i][w] = max value using first i items with capacity w
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(capacity + 1, 0));

    for (int i = 1; i <= n; ++i) {
        for (int w = 0; w <= capacity; ++w) {
            dp[i][w] = dp[i - 1][w];  // Skip item i-1
            if (items[i - 1].weight <= w) {
                dp[i][w] = std::max(dp[i][w],
                                    dp[i - 1][w - items[i - 1].weight] + items[i - 1].value);
            }
        }
    }

    // Backtrack to find selected items
    std::vector<int> selected;
    int w = capacity;
    for (int i = n; i > 0; --i) {
        if (dp[i][w] != dp[i - 1][w]) {
            selected.push_back(i - 1);  // Item i-1 was selected
            w -= items[i - 1].weight;
        }
    }
    std::reverse(selected.begin(), selected.end());

    return {dp[n][capacity], selected};
}

int main() {
    std::cout << "=== 0/1 Knapsack (Dynamic Programming) ===\n\n";

    std::vector<Item> items = {
        {2, 6, "Gold Ring"},
        {3, 8, "Silver Necklace"},
        {5, 12, "Diamond"},
        {1, 4, "Ruby"},
        {4, 10, "Emerald"}
    };
    int capacity = 8;

    // Space-optimized version
    int max_value = knapsack_max_value(items, capacity);
    std::cout << "Max value (capacity=" << capacity << "): " << max_value << '\n';

    // Version with item reconstruction
    auto [value, selected] = knapsack_with_items(items, capacity);
    std::cout << "Selected items:\n";
    int total_weight = 0;
    for (int idx : selected) {
        std::cout << "  " << items[idx].name
                  << " (w=" << items[idx].weight << ", v=" << items[idx].value << ")\n";
        total_weight += items[idx].weight;
    }
    std::cout << "Total weight: " << total_weight << "/" << capacity << '\n';
    std::cout << "Total value: " << value << '\n';

    // Assertions
    assert(max_value == value);
    assert(total_weight <= capacity);
    assert(value == 22);  // Gold Ring(w=2,v=6) + Diamond(w=5,v=12) + Ruby(w=1,v=4) = 22, weight=8

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
