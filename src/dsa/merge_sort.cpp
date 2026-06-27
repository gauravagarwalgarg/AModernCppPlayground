/**************************************************************************************************
 * Topic: Merge Sort Divide and Conquer Sorting
 *
 * Interview Relevance: ★★★★★
 * Time: O(n log n) guaranteed (worst/avg/best)
 * Space: O(n) auxiliary
 *
 * What interviewers look for:
 *   - Stable sort (preserves order of equal elements)
 *   - Divide & conquer paradigm
 *   - Counting inversions (classic follow-up)
 *   - When to prefer over quicksort (external sort, linked lists, stability)
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o merge_sort merge_sort.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

// ============================================================================
// Classic merge sort with inversion counting
// ============================================================================

template <typename T>
long long merge_and_count(std::vector<T>& arr, int left, int mid, int right) {
    std::vector<T> temp;
    temp.reserve(right - left + 1);

    int i = left, j = mid + 1;
    long long inversions = 0;

    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) {
            temp.push_back(arr[i++]);
        } else {
            // arr[i] > arr[j]: all remaining elements in left half are inversions
            inversions += (mid - i + 1);
            temp.push_back(arr[j++]);
        }
    }
    while (i <= mid) temp.push_back(arr[i++]);
    while (j <= right) temp.push_back(arr[j++]);

    std::copy(temp.begin(), temp.end(), arr.begin() + left);
    return inversions;
}

template <typename T>
long long merge_sort(std::vector<T>& arr, int left, int right) {
    if (left >= right) return 0;

    int mid = left + (right - left) / 2;
    long long inversions = 0;

    inversions += merge_sort(arr, left, mid);
    inversions += merge_sort(arr, mid + 1, right);
    inversions += merge_and_count(arr, left, mid, right);

    return inversions;
}

template <typename T>
long long merge_sort(std::vector<T>& arr) {
    if (arr.empty()) return 0;
    return merge_sort(arr, 0, static_cast<int>(arr.size()) - 1);
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Merge Sort with Inversion Count ===\n\n";

    // Test 1: Basic sort
    std::vector<int> arr1 = {38, 27, 43, 3, 9, 82, 10};
    std::cout << "Before: ";
    for (int x : arr1) std::cout << x << " ";
    std::cout << '\n';

    long long inv1 = merge_sort(arr1);
    std::cout << "After:  ";
    for (int x : arr1) std::cout << x << " ";
    std::cout << "\nInversions: " << inv1 << '\n';

    assert(std::is_sorted(arr1.begin(), arr1.end()));

    // Test 2: Already sorted (0 inversions)
    std::vector<int> arr2 = {1, 2, 3, 4, 5};
    long long inv2 = merge_sort(arr2);
    assert(inv2 == 0);
    assert(std::is_sorted(arr2.begin(), arr2.end()));

    // Test 3: Reverse sorted (maximum inversions = n*(n-1)/2)
    std::vector<int> arr3 = {5, 4, 3, 2, 1};
    long long inv3 = merge_sort(arr3);
    assert(inv3 == 10);  // 5*4/2 = 10
    assert(std::is_sorted(arr3.begin(), arr3.end()));

    // Test 4: Stability (equal elements preserve order)
    struct Pair {
        int key;
        int original_index;
        bool operator<=(Pair const& other) const { return key <= other.key; }
    };
    std::vector<Pair> arr4 = {{3, 0}, {1, 1}, {3, 2}, {1, 3}, {2, 4}};
    merge_sort(arr4, 0, 4);
    // Check stability: among key=1, index 1 should come before index 3
    assert(arr4[0].key == 1 && arr4[0].original_index == 1);
    assert(arr4[1].key == 1 && arr4[1].original_index == 3);
    assert(arr4[2].key == 2);
    assert(arr4[3].key == 3 && arr4[3].original_index == 0);
    assert(arr4[4].key == 3 && arr4[4].original_index == 2);

    std::cout << "\nAll assertions passed. Sort is stable.\n";
    return EXIT_SUCCESS;
}
