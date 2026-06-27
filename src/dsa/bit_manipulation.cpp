/**
 * @file bit_manipulation.cpp
 * @brief Bit Manipulation Tricks - Essential Interview Patterns
 * 
 * Key operations:
 *   - Check power of 2: n & (n-1) == 0
 *   - Count set bits: Brian Kernighan's algorithm
 *   - Find single number: XOR all elements (pairs cancel)
 *   - Get/Set/Clear/Toggle specific bit
 *   - Swap without temp: XOR swap
 * 
 * Why bits matter in systems:
 *   - Flags/permissions (rwx in file systems)
 *   - Network protocols (IP headers, masks)
 *   - Hardware registers (embedded)
 *   - Bloom filters, hash maps
 * 
 * Compile: g++ -std=c++20 -Wall -pthread bit_manipulation.cpp -o bit_manipulation
 */

#include <iostream>
#include <vector>
#include <cstdint>
#include <bitset>

// === Trick 1: Check if power of 2 ===
// Power of 2 has exactly one bit set: 1000...0
// n-1 flips all bits after that one: 0111...1
// AND gives 0 only for powers of 2
bool isPowerOfTwo(int n) {
    return n > 0 && (n & (n - 1)) == 0;
}

// === Trick 2: Count set bits (Brian Kernighan's) ===
// n & (n-1) clears the lowest set bit. Count how many times until 0.
int countSetBits(int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // Clear lowest set bit
        ++count;
    }
    return count;
}

// === Trick 3: Find single number (XOR) ===
// XOR properties: a^a=0, a^0=a, commutative, associative
// All pairs cancel, leaving the single element
int singleNumber(const std::vector<int>& nums) {
    int result = 0;
    for (int num : nums) {
        result ^= num;
    }
    return result;
}

// === Trick 4: Get/Set/Clear/Toggle bit at position ===
bool getBit(int n, int pos)    { return (n >> pos) & 1; }
int  setBit(int n, int pos)    { return n | (1 << pos); }
int  clearBit(int n, int pos)  { return n & ~(1 << pos); }
int  toggleBit(int n, int pos) { return n ^ (1 << pos); }

// === Trick 5: Swap without temp ===
void xorSwap(int& a, int& b) {
    if (&a == &b) return;  // Guard against self-swap
    a ^= b;
    b ^= a;
    a ^= b;
}

// === Bonus: Find two non-repeating numbers ===
// All numbers appear twice except two. XOR all → xor of the two unique numbers.
// Find a set bit in xor_all → partition into two groups → XOR each group.
std::pair<int,int> findTwoSingles(const std::vector<int>& nums) {
    int xor_all = 0;
    for (int n : nums) xor_all ^= n;

    // Find rightmost set bit (differentiating bit)
    int diff_bit = xor_all & (-xor_all);

    int a = 0, b = 0;
    for (int n : nums) {
        if (n & diff_bit) a ^= n;
        else b ^= n;
    }
    return {a, b};
}

int main() {
    std::cout << "=== Bit Manipulation Tricks ===\n\n";

    // Trick 1: Power of 2
    std::cout << "--- Power of 2 Check ---\n";
    for (int n : {1, 2, 3, 4, 16, 18, 64}) {
        std::cout << n << ": " << std::boolalpha << isPowerOfTwo(n) << "  ";
    }
    std::cout << "\n\n";

    // Trick 2: Count set bits
    std::cout << "--- Count Set Bits ---\n";
    for (int n : {0, 1, 7, 15, 255}) {
        std::cout << n << " (" << std::bitset<8>(n) << "): " << countSetBits(n) << " bits\n";
    }
    std::cout << "\n";

    // Trick 3: Single number
    std::cout << "--- Find Single Number (XOR) ---\n";
    std::vector<int> nums = {4, 1, 2, 1, 2};
    std::cout << "Array: [4,1,2,1,2] → single = " << singleNumber(nums) << "\n\n";

    // Trick 4: Bit operations
    std::cout << "--- Get/Set/Clear/Toggle Bit ---\n";
    int val = 0b1010;  // 10
    std::cout << "Value: " << std::bitset<4>(val) << " (10)\n";
    std::cout << "getBit(pos=1): " << getBit(val, 1) << "\n";
    std::cout << "setBit(pos=2): " << std::bitset<4>(setBit(val, 2)) << " (14)\n";
    std::cout << "clearBit(pos=3): " << std::bitset<4>(clearBit(val, 3)) << " (2)\n";
    std::cout << "toggleBit(pos=0): " << std::bitset<4>(toggleBit(val, 0)) << " (11)\n\n";

    // Trick 5: XOR swap
    std::cout << "--- XOR Swap ---\n";
    int a = 42, b = 99;
    std::cout << "Before: a=" << a << ", b=" << b << "\n";
    xorSwap(a, b);
    std::cout << "After:  a=" << a << ", b=" << b << "\n\n";

    // Bonus: Two singles
    std::cout << "--- Find Two Non-Repeating Numbers ---\n";
    std::vector<int> nums2 = {1, 2, 3, 1, 2, 5};
    auto [x, y] = findTwoSingles(nums2);
    std::cout << "Array: [1,2,3,1,2,5] → singles = {" << x << ", " << y << "}\n";

    return 0;
}
