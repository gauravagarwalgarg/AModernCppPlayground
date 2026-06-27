/**
 * @file trie.cpp
 * @brief Trie (Prefix Tree) - Insert, Search, StartsWith, Autocomplete
 * 
 * A Trie stores strings character-by-character in a tree structure.
 * Each edge represents a character, each node can mark end-of-word.
 * 
 * Complexity: O(L) for insert/search/prefix where L = word length
 * Space: O(ALPHABET * N * L) worst case, but shared prefixes save space
 * 
 * Use cases:
 *   - Autocomplete (prefix search): type "app" → suggest "apple", "application"
 *   - Spell checking: is this word in the dictionary?
 *   - IP routing: longest prefix match
 *   - Word games: valid prefix checking for pruning
 * 
 * Compile: g++ -std=c++20 -Wall -pthread trie.cpp -o trie
 */

#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <memory>

class Trie {
    struct TrieNode {
        std::array<std::unique_ptr<TrieNode>, 26> children{};
        bool is_end = false;
    };

    std::unique_ptr<TrieNode> root_ = std::make_unique<TrieNode>();

public:
    // Insert a word into the trie - O(L)
    void insert(const std::string& word) {
        TrieNode* node = root_.get();
        for (char c : word) {
            int idx = c - 'a';
            if (!node->children[idx]) {
                node->children[idx] = std::make_unique<TrieNode>();
            }
            node = node->children[idx].get();
        }
        node->is_end = true;
    }

    // Search for exact word - O(L)
    bool search(const std::string& word) const {
        const TrieNode* node = traverse(word);
        return node && node->is_end;
    }

    // Check if any word starts with prefix - O(L)
    bool startsWith(const std::string& prefix) const {
        return traverse(prefix) != nullptr;
    }

    // Autocomplete: find all words with given prefix
    std::vector<std::string> autocomplete(const std::string& prefix, int max_results = 5) const {
        const TrieNode* node = traverse(prefix);
        std::vector<std::string> results;
        if (node) {
            std::string current = prefix;
            collectWords(node, current, results, max_results);
        }
        return results;
    }

private:
    // Traverse to the node representing the last char of key
    const TrieNode* traverse(const std::string& key) const {
        const TrieNode* node = root_.get();
        for (char c : key) {
            int idx = c - 'a';
            if (!node->children[idx]) return nullptr;
            node = node->children[idx].get();
        }
        return node;
    }

    // DFS to collect all words from a given node
    void collectWords(const TrieNode* node, std::string& current,
                      std::vector<std::string>& results, int max_results) const {
        if (static_cast<int>(results.size()) >= max_results) return;
        if (node->is_end) results.push_back(current);

        for (int i = 0; i < 26; ++i) {
            if (node->children[i]) {
                current.push_back('a' + i);
                collectWords(node->children[i].get(), current, results, max_results);
                current.pop_back();
            }
        }
    }
};

int main() {
    std::cout << "=== Trie (Prefix Tree) ===\n\n";
    Trie trie;

    // Insert dictionary
    std::vector<std::string> words = {
        "apple", "app", "application", "apply", "apt",
        "bat", "batch", "bath", "banana"
    };
    for (const auto& w : words) trie.insert(w);
    std::cout << "Inserted: ";
    for (const auto& w : words) std::cout << w << " ";
    std::cout << "\n\n";

    // Search
    std::cout << "--- Search ---\n";
    std::cout << "search(\"apple\"): " << std::boolalpha << trie.search("apple") << "\n";
    std::cout << "search(\"app\"): " << trie.search("app") << "\n";
    std::cout << "search(\"appl\"): " << trie.search("appl") << " (prefix, not complete word)\n\n";

    // Prefix check
    std::cout << "--- Starts With ---\n";
    std::cout << "startsWith(\"app\"): " << trie.startsWith("app") << "\n";
    std::cout << "startsWith(\"bat\"): " << trie.startsWith("bat") << "\n";
    std::cout << "startsWith(\"xyz\"): " << trie.startsWith("xyz") << "\n\n";

    // Autocomplete
    std::cout << "--- Autocomplete ---\n";
    auto suggestions = trie.autocomplete("app");
    std::cout << "autocomplete(\"app\"): ";
    for (const auto& s : suggestions) std::cout << s << " ";
    std::cout << "\n";

    suggestions = trie.autocomplete("ba");
    std::cout << "autocomplete(\"ba\"): ";
    for (const auto& s : suggestions) std::cout << s << " ";
    std::cout << "\n";

    return 0;
}
