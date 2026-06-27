/**
 * @file monotonic_stack.cpp
 * @brief Monotonic Stack Patterns - Next Greater, Temperatures, Largest Rectangle
 * 
 * Key insight: A monotonic stack efficiently finds the "next greater/smaller"
 * element for every position in O(n) total time.
 * 
 * Patterns:
 *   1. Decreasing stack: find next greater element (pop when current > stack top)
 *   2. Increasing stack: find next smaller element
 *   3. Histogram area: largest rectangle using heights as constraints
 * 
 * Stack invariant: elements in stack are always in sorted order (mono-inc or mono-dec).
 * Each element is pushed once and popped once → O(n) amortized.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread monotonic_stack.cpp -o monotonic_stack
 */

#include <iostream>
#include <vector>
#include <stack>

// === Problem 1: Next Greater Element (LC 496/503) ===
// For each element, find the first element to the right that is larger.
std::vector<int> nextGreaterElement(const std::vector<int>& nums) {
    int n = nums.size();
    std::vector<int> result(n, -1);
    std::stack<int> stk;  // Monotonic decreasing: stores indices

    for (int i = 0; i < n; ++i) {
        // Pop elements smaller than current → current is their "next greater"
        while (!stk.empty() && nums[stk.top()] < nums[i]) {
            result[stk.top()] = nums[i];
            stk.pop();
        }
        stk.push(i);
    }
    return result;  // Elements remaining in stack have no next greater (-1)
}

// === Problem 2: Daily Temperatures (LC 739) ===
// How many days until a warmer temperature?
std::vector<int> dailyTemperatures(const std::vector<int>& temps) {
    int n = temps.size();
    std::vector<int> result(n, 0);
    std::stack<int> stk;  // Decreasing stack of indices

    for (int i = 0; i < n; ++i) {
        while (!stk.empty() && temps[stk.top()] < temps[i]) {
            int prev = stk.top();
            stk.pop();
            result[prev] = i - prev;  // Days to wait
        }
        stk.push(i);
    }
    return result;
}

// === Problem 3: Largest Rectangle in Histogram (LC 84) ===
// Key: for each bar, find how far left and right it can extend
int largestRectangleArea(const std::vector<int>& heights) {
    int n = heights.size();
    std::stack<int> stk;  // Increasing stack of indices
    int max_area = 0;

    for (int i = 0; i <= n; ++i) {
        int current_height = (i == n) ? 0 : heights[i];
        // When we see a shorter bar, pop and calculate areas
        while (!stk.empty() && heights[stk.top()] > current_height) {
            int height = heights[stk.top()];
            stk.pop();
            int width = stk.empty() ? i : (i - stk.top() - 1);
            max_area = std::max(max_area, height * width);
        }
        stk.push(i);
    }
    return max_area;
}

int main() {
    std::cout << "=== Monotonic Stack Patterns ===\n\n";

    // Problem 1: Next Greater Element
    std::cout << "--- Next Greater Element ---\n";
    std::vector<int> nums = {4, 5, 2, 25, 7, 8};
    std::cout << "Input: ";
    for (int x : nums) std::cout << x << " ";
    auto nge = nextGreaterElement(nums);
    std::cout << "\nNGE:   ";
    for (int x : nge) std::cout << x << " ";
    std::cout << "\n(expected: 5 25 25 -1 8 -1)\n\n";

    // Problem 2: Daily Temperatures
    std::cout << "--- Daily Temperatures ---\n";
    std::vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    std::cout << "Temps: ";
    for (int x : temps) std::cout << x << " ";
    auto days = dailyTemperatures(temps);
    std::cout << "\nDays:  ";
    for (int x : days) std::cout << x << " ";
    std::cout << "\n(expected: 1 1 4 2 1 1 0 0)\n\n";

    // Problem 3: Largest Rectangle in Histogram
    std::cout << "--- Largest Rectangle in Histogram ---\n";
    std::vector<int> hist = {2, 1, 5, 6, 2, 3};
    std::cout << "Heights: ";
    for (int x : hist) std::cout << x << " ";
    std::cout << "\nLargest area: " << largestRectangleArea(hist) << " (expected: 10)\n";

    return 0;
}
