/**************************************************************************************************
 * Topic: KMP (Knuth-Morris-Pratt) Linear String Pattern Matching
 *
 * Interview Relevance: ★★★★★
 * Time: O(n + m) where n = text length, m = pattern length
 * Space: O(m) for the failure function
 *
 * What interviewers look for:
 *   - Understanding the failure function (longest proper prefix-suffix)
 *   - Why it's O(n+m) not O(n*m)
 *   - Building the prefix table
 *   - Applications: repeated patterns, string periodicity
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o kmp_string_matching kmp_string_matching.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

// ============================================================================
// Build the failure function (prefix table / partial match table)
// lps[i] = length of longest proper prefix of pattern[0..i] that is also a suffix
// ============================================================================

std::vector<int> build_prefix_table(std::string_view pattern) {
    int m = static_cast<int>(pattern.size());
    std::vector<int> lps(m, 0);

    int len = 0;  // Length of previous longest prefix suffix
    int i = 1;

    while (i < m) {
        if (pattern[i] == pattern[len]) {
            lps[i] = ++len;
            ++i;
        } else if (len != 0) {
            len = lps[len - 1];  // Don't increment i; try shorter prefix
        } else {
            lps[i] = 0;
            ++i;
        }
    }

    return lps;
}

// ============================================================================
// KMP Search find all occurrences of pattern in text
// ============================================================================

std::vector<int> kmp_search(std::string_view text, std::string_view pattern) {
    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());

    if (m == 0) return {};
    if (m > n) return {};

    auto lps = build_prefix_table(pattern);
    std::vector<int> matches;

    int i = 0;  // Index in text
    int j = 0;  // Index in pattern

    while (i < n) {
        if (text[i] == pattern[j]) {
            ++i;
            ++j;
        }

        if (j == m) {
            matches.push_back(i - j);  // Found match at index (i - j)
            j = lps[j - 1];            // Continue searching for overlapping matches
        } else if (i < n && text[i] != pattern[j]) {
            if (j != 0) {
                j = lps[j - 1];  // Use failure function (don't reset i)
            } else {
                ++i;
            }
        }
    }

    return matches;
}

// ============================================================================
// Bonus: Find shortest period of a string using KMP prefix table
// ============================================================================

int shortest_period(std::string_view s) {
    auto lps = build_prefix_table(s);
    int n = static_cast<int>(s.size());
    int period = n - lps[n - 1];
    // Valid period only if it divides n evenly
    if (n % period == 0) return period;
    return n;  // The string itself is the period
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== KMP String Pattern Matching ===\n\n";

    // Test 1: Basic search
    std::string text = "AABAACAADAABAABA";
    std::string pattern = "AABA";

    auto matches = kmp_search(text, pattern);
    std::cout << "Text:    \"" << text << "\"\n";
    std::cout << "Pattern: \"" << pattern << "\"\n";
    std::cout << "Matches at positions: ";
    for (int pos : matches) std::cout << pos << " ";
    std::cout << '\n';

    assert(matches == (std::vector<int>{0, 9, 12}));

    // Test 2: No match
    auto no_match = kmp_search("hello world", "xyz");
    assert(no_match.empty());

    // Test 3: Overlapping matches
    auto overlapping = kmp_search("AAAA", "AA");
    assert(overlapping == (std::vector<int>{0, 1, 2}));

    // Test 4: Prefix table
    std::cout << "\nPrefix table for \"AABAACAABAA\":\n  ";
    auto lps = build_prefix_table("AABAACAABAA");
    for (int v : lps) std::cout << v << " ";
    std::cout << '\n';
    assert(lps == (std::vector<int>{0, 1, 0, 1, 2, 0, 1, 2, 3, 4, 5}));

    // Test 5: String periodicity
    assert(shortest_period("abcabcabc") == 3);   // "abc" repeated 3 times
    assert(shortest_period("aaaa") == 1);         // "a" repeated 4 times
    assert(shortest_period("abcab") == 5);        // Not periodic

    std::cout << "\nShortest period of \"abcabcabc\": " << shortest_period("abcabcabc") << '\n';
    std::cout << "Shortest period of \"aaaa\": " << shortest_period("aaaa") << '\n';

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
