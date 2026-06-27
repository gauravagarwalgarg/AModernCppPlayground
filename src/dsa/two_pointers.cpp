/**
 * @file two_pointers.cpp
 * @brief Two Pointer Patterns - Three Core Variants
 * 
 * Patterns:
 *   1. Opposite ends: converge from both sides (container with most water)
 *   2. Fast/slow: detect cycles, find middle (linked list cycle detection)
 *   3. Partitioning: Dutch national flag (3-way partition)
 * 
 * When to use: sorted arrays, linked lists, partitioning problems.
 * Time: O(n), Space: O(1) - the beauty of two pointers.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread two_pointers.cpp -o two_pointers
 */

#include <iostream>
#include <vector>
#include <algorithm>

// === Pattern 1: Opposite Ends - Container With Most Water (LC 11) ===
// Two pointers converge from both ends, move the shorter side inward
int maxArea(const std::vector<int>& height) {
    int left = 0, right = static_cast<int>(height.size()) - 1;
    int max_area = 0;

    while (left < right) {
        int width = right - left;
        int h = std::min(height[left], height[right]);
        max_area = std::max(max_area, width * h);

        // Move the shorter side - it's the bottleneck
        if (height[left] < height[right]) ++left;
        else --right;
    }
    return max_area;
}

// === Pattern 2: Fast/Slow - Linked List Cycle Detection (Floyd's) ===
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int v, ListNode* n = nullptr) : val(v), next(n) {}
};

bool hasCycle(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;          // 1 step
        fast = fast->next->next;    // 2 steps
        if (slow == fast) return true;  // They meet → cycle!
    }
    return false;  // fast reached end → no cycle
}

// Find cycle start (follow-up): reset one pointer to head, both move 1 step
ListNode* findCycleStart(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;

    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            // Reset slow to head, advance both by 1
            slow = head;
            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }
            return slow;  // Meeting point = cycle start
        }
    }
    return nullptr;
}

// === Pattern 3: Partitioning - Dutch National Flag (LC 75) ===
// 3-way partition: all 0s left, all 2s right, all 1s middle
void sortColors(std::vector<int>& nums) {
    int low = 0, mid = 0, high = static_cast<int>(nums.size()) - 1;

    while (mid <= high) {
        if (nums[mid] == 0) {
            std::swap(nums[low++], nums[mid++]);
        } else if (nums[mid] == 1) {
            ++mid;
        } else {  // nums[mid] == 2
            std::swap(nums[mid], nums[high--]);
            // Don't advance mid - swapped element needs checking
        }
    }
}

int main() {
    std::cout << "=== Two Pointer Patterns ===\n\n";

    // Pattern 1: Container With Most Water
    std::cout << "--- Pattern 1: Opposite Ends (Container With Most Water) ---\n";
    std::vector<int> heights = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    std::cout << "Heights: ";
    for (int h : heights) std::cout << h << " ";
    std::cout << "\nMax area: " << maxArea(heights) << " (expected: 49)\n\n";

    // Pattern 2: Cycle Detection
    std::cout << "--- Pattern 2: Fast/Slow (Cycle Detection) ---\n";
    // Create: 1→2→3→4→5→(back to 3)
    ListNode n5(5), n4(4, &n5), n3(3, &n4), n2(2, &n3), n1(1, &n2);
    n5.next = &n3;  // Create cycle

    std::cout << "List 1→2→3→4→5→(3): has cycle = " << std::boolalpha << hasCycle(&n1) << "\n";
    ListNode* cycle_start = findCycleStart(&n1);
    std::cout << "Cycle starts at node with value: " << cycle_start->val << "\n";

    // No cycle
    n5.next = nullptr;
    std::cout << "After removing cycle: has cycle = " << hasCycle(&n1) << "\n\n";

    // Pattern 3: Dutch National Flag
    std::cout << "--- Pattern 3: Partitioning (Dutch National Flag) ---\n";
    std::vector<int> colors = {2, 0, 2, 1, 1, 0};
    std::cout << "Before: ";
    for (int c : colors) std::cout << c << " ";
    sortColors(colors);
    std::cout << "\nAfter:  ";
    for (int c : colors) std::cout << c << " ";
    std::cout << " (expected: 0 0 1 1 2 2)\n";

    return 0;
}
