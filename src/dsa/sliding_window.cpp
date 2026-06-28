/**
 * @file sliding_window.cpp
 * @brief Sliding Window Patterns - Three Core Variants
 * 
 * Patterns:
 *   1. Fixed window: compute property over window of size k (max sum subarray)
 *   2. Variable window: expand/contract to meet condition (longest substring)
 *   3. Window with deque: maintain monotonic deque (sliding window maximum)
 * 
 * Key insight: avoid recomputing entire window each step.
 * Add new element, remove old element → O(n) total instead of O(n*k).
 * 
 * Compile: g++ -std=c++20 -Wall -pthread sliding_window.cpp -o sliding_window
 */

#include <iostream>
#include <vector>
#include <deque>
#include <unordered_map>
#include <string>
#include <algorithm>

// === Pattern 1: Fixed Window - Maximum Sum of K Elements ===
int maxSumSubarray(const std::vector<int>& nums, int k) {
    int window_sum = 0;
    // Initialize first window
    for (int i = 0; i < k; ++i) window_sum += nums[i];

    int max_sum = window_sum;
    // Slide: add right element, remove left element
    for (size_t i = k; i < nums.size(); ++i) {
        window_sum += nums[i] - nums[i - k];
        max_sum = std::max(max_sum, window_sum);
    }
    return max_sum;
}

// === Pattern 2: Variable Window - Longest Substring Without Repeating (LC 3) ===
int lengthOfLongestSubstring(const std::string& s) {
    std::unordered_map<char, int> char_index;  // char → last seen index
    int max_len = 0, left = 0;

    for (int right = 0; right < static_cast<int>(s.size()); ++right) {
        char c = s[right];
        // If char seen and within current window, shrink from left
        if (char_index.count(c) && char_index[c] >= left) {
            left = char_index[c] + 1;  // Jump left past the duplicate
        }
        char_index[c] = right;
        max_len = std::max(max_len, right - left + 1);
    }
    return max_len;
}

// === Pattern 3: Window with Deque - Sliding Window Maximum (LC 239) ===
// Monotonic decreasing deque: front is always the max in current window
std::vector<int> maxSlidingWindow(const std::vector<int>& nums, int k) {
    std::deque<int> dq;  // Stores indices, front = index of max
    std::vector<int> result;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        // Remove elements outside window from front
        while (!dq.empty() && dq.front() <= i - k) {
            dq.pop_front();
        }
        // Remove smaller elements from back (they'll never be max)
        while (!dq.empty() && nums[dq.back()] <= nums[i]) {
            dq.pop_back();
        }
        dq.push_back(i);

        // Window is full, record the max (front of deque)
        if (i >= k - 1) {
            result.push_back(nums[dq.front()]);
        }
    }
    return result;
}

int main() {
    std::cout << "=== Sliding Window Patterns ===\n\n";

    // Pattern 1: Fixed Window
    std::cout << "--- Pattern 1: Fixed Window (Max Sum of K=3 elements) ---\n";
    std::vector<int> arr = {2, 1, 5, 1, 3, 2};
    std::cout << "Array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\nMax sum (k=3): " << maxSumSubarray(arr, 3) << " (expected: 9 → [5,1,3])\n\n";

    // Pattern 2: Variable Window
    std::cout << "--- Pattern 2: Variable Window (Longest Substring No Repeat) ---\n";
    std::string s1 = "abcabcbb", s2 = "bbbbb", s3 = "pwwkew";
    std::cout << "\"" << s1 << "\" → " << lengthOfLongestSubstring(s1) << " (expected: 3, \"abc\")\n";
    std::cout << "\"" << s2 << "\" → " << lengthOfLongestSubstring(s2) << " (expected: 1)\n";
    std::cout << "\"" << s3 << "\" → " << lengthOfLongestSubstring(s3) << " (expected: 3, \"wke\")\n\n";

    // Pattern 3: Deque Window
    std::cout << "--- Pattern 3: Monotonic Deque (Sliding Window Maximum, k=3) ---\n";
    std::vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    std::cout << "Array: ";
    for (int x : nums) std::cout << x << " ";
    auto result = maxSlidingWindow(nums, 3);
    std::cout << "\nWindow maxes: ";
    for (int x : result) std::cout << x << " ";
    std::cout << "(expected: 3 3 5 5 6 7)\n";

    return 0;
}
