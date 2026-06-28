/**************************************************************************************************
 * Topic: Longest Common Subsequence (LCS) Classic 2D DP
 *
 * Interview Relevance: ★★★★★
 * Time: O(m * n)
 * Space: O(min(m, n)) with optimization
 *
 * What interviewers look for:
 *   - State: dp[i][j] = LCS length of first i chars and first j chars
 *   - Transition: match → dp[i-1][j-1] + 1; no match → max(dp[i-1][j], dp[i][j-1])
 *   - Space optimization to O(min(m,n))
 *   - Sequence reconstruction (backtrack through the DP table)
 *   - Related: edit distance, diff algorithms
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o longest_common_subsequence longest_common_subsequence.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// ============================================================================
// Full DP with sequence reconstruction
// ============================================================================

std::string lcs_with_sequence(std::string_view a, std::string_view b) {
    int m = static_cast<int>(a.size());
    int n = static_cast<int>(b.size());

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (a[i - 1] == b[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] = std::max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }

    // Backtrack to find the actual subsequence
    std::string result;
    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (a[i - 1] == b[j - 1]) {
            result += a[i - 1];
            --i; --j;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            --i;
        } else {
            --j;
        }
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// ============================================================================
// Space-optimized version (length only)
// ============================================================================

int lcs_length(std::string_view a, std::string_view b) {
    // Use the shorter string for the DP row to minimize space
    if (a.size() < b.size()) std::swap(a, b);

    int n = static_cast<int>(b.size());
    std::vector<int> prev(n + 1, 0), curr(n + 1, 0);

    for (char ca : a) {
        for (int j = 1; j <= n; ++j) {
            if (ca == b[j - 1]) {
                curr[j] = prev[j - 1] + 1;
            } else {
                curr[j] = std::max(prev[j], curr[j - 1]);
            }
        }
        std::swap(prev, curr);
        std::fill(curr.begin(), curr.end(), 0);
    }

    return prev[n];
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Longest Common Subsequence ===\n\n";

    // Test 1
    std::string a = "ABCBDAB";
    std::string b = "BDCABA";
    auto lcs = lcs_with_sequence(a, b);
    std::cout << "a = \"" << a << "\"\n";
    std::cout << "b = \"" << b << "\"\n";
    std::cout << "LCS = \"" << lcs << "\" (length " << lcs.size() << ")\n";
    assert(lcs.size() == 4);
    assert(lcs_length(a, b) == 4);

    // Test 2: identical strings
    assert(lcs_length("abc", "abc") == 3);

    // Test 3: no common chars
    assert(lcs_length("abc", "xyz") == 0);

    // Test 4: empty string
    assert(lcs_length("", "abc") == 0);

    // Test 5: real-world diff example
    std::string old_code = "int main() { return 0; }";
    std::string new_code = "int main() { return 1; }";
    auto common = lcs_with_sequence(old_code, new_code);
    std::cout << "\nCommon between old and new code: \"" << common << "\"\n";
    std::cout << "Length: " << common.size() << "/" << old_code.size() << '\n';

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
