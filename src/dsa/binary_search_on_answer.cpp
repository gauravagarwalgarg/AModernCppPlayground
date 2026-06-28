/**
 * @file binary_search_on_answer.cpp
 * @brief Binary Search on Answer Space - Template Pattern
 * 
 * Key insight: Instead of searching for the element, search the ANSWER SPACE.
 * Template: define check(mid) that returns true/false → binary search on [lo, hi].
 * 
 * Pattern:
 *   while (lo < hi) {
 *       mid = lo + (hi - lo) / 2;
 *       if (feasible(mid)) hi = mid;     // answer could be mid or smaller
 *       else lo = mid + 1;               // need larger answer
 *   }
 *   return lo;  // minimum feasible answer
 * 
 * Problems:
 *   1. Koko Eating Bananas (LC 875): min speed to eat all bananas in h hours
 *   2. Split Array Largest Sum (LC 410): min largest subarray sum with m splits
 * 
 * Compile: g++ -std=c++20 -Wall -pthread binary_search_on_answer.cpp -o binary_search_on_answer
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

// === Problem 1: Koko Eating Bananas (LC 875) ===
// Koko eats at speed k bananas/hour. Each pile takes ceil(pile/k) hours.
// Find minimum k to finish all piles within h hours.
int minEatingSpeed(const std::vector<int>& piles, int h) {
    int lo = 1;
    int hi = *std::max_element(piles.begin(), piles.end());

    // check: can Koko finish all piles at speed `mid` in h hours?
    auto canFinish = [&](int speed) -> bool {
        long hours_needed = 0;
        for (int pile : piles) {
            hours_needed += (pile + speed - 1) / speed;  // ceil division
        }
        return hours_needed <= h;
    };

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (canFinish(mid)) {
            hi = mid;       // feasible → try smaller speed
        } else {
            lo = mid + 1;   // not feasible → need faster
        }
    }
    return lo;  // Minimum feasible speed
}

// === Problem 2: Split Array Largest Sum (LC 410) ===
// Split array into m subarrays to minimize the largest subarray sum.
// Binary search on the answer: what's the minimum possible "largest sum"?
int splitArray(const std::vector<int>& nums, int m) {
    // Answer range: [max_element, total_sum]
    int lo = *std::max_element(nums.begin(), nums.end());
    int hi = std::accumulate(nums.begin(), nums.end(), 0);

    // check: can we split into ≤ m subarrays where each sum ≤ max_sum?
    auto canSplit = [&](int max_sum) -> bool {
        int splits = 1, current_sum = 0;
        for (int num : nums) {
            if (current_sum + num > max_sum) {
                ++splits;
                current_sum = num;
                if (splits > m) return false;
            } else {
                current_sum += num;
            }
        }
        return true;
    };

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (canSplit(mid)) {
            hi = mid;       // feasible → try smaller max_sum
        } else {
            lo = mid + 1;   // not feasible → need larger allowed sum
        }
    }
    return lo;
}

int main() {
    std::cout << "=== Binary Search on Answer Space ===\n\n";

    // Problem 1: Koko Eating Bananas
    std::cout << "--- Koko Eating Bananas (LC 875) ---\n";
    std::vector<int> piles1 = {3, 6, 7, 11};
    std::cout << "Piles: [3,6,7,11], Hours: 8\n";
    std::cout << "Min speed: " << minEatingSpeed(piles1, 8) << " (expected: 4)\n";

    std::vector<int> piles2 = {30, 11, 23, 4, 20};
    std::cout << "Piles: [30,11,23,4,20], Hours: 5\n";
    std::cout << "Min speed: " << minEatingSpeed(piles2, 5) << " (expected: 30)\n";

    std::vector<int> piles3 = {30, 11, 23, 4, 20};
    std::cout << "Piles: [30,11,23,4,20], Hours: 6\n";
    std::cout << "Min speed: " << minEatingSpeed(piles3, 6) << " (expected: 23)\n\n";

    // Problem 2: Split Array Largest Sum
    std::cout << "--- Split Array Largest Sum (LC 410) ---\n";
    std::vector<int> nums1 = {7, 2, 5, 10, 8};
    std::cout << "Array: [7,2,5,10,8], m=2\n";
    std::cout << "Min largest sum: " << splitArray(nums1, 2) << " (expected: 18 → [7,2,5|10,8])\n";

    std::vector<int> nums2 = {1, 2, 3, 4, 5};
    std::cout << "Array: [1,2,3,4,5], m=2\n";
    std::cout << "Min largest sum: " << splitArray(nums2, 2) << " (expected: 9 → [1,2,3|4,5])\n\n";

    std::cout << "=== Template ===\n";
    std::cout << "while (lo < hi) {\n";
    std::cout << "    mid = lo + (hi-lo)/2;\n";
    std::cout << "    if (feasible(mid)) hi = mid;\n";
    std::cout << "    else lo = mid + 1;\n";
    std::cout << "} // lo = minimum feasible answer\n";

    return 0;
}
