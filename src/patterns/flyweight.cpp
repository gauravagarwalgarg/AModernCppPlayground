/**************************************************************************************************
 * Topic: Flyweight Pattern Share Common State to Save Memory
 *
 * Interview Relevance: ★★★
 * What interviewers look for:
 *   - Intrinsic state (shared) vs extrinsic state (per-instance)
 *   - Factory that caches and reuses flyweight objects
 *   - Real-world: text rendering (font glyphs), game objects (tree sprites),
 *     network packet headers
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o flyweight flyweight.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================
// Flyweight the shared (intrinsic) state
// ============================================================================

// In a text editor: CharacterStyle is shared across many characters
struct CharacterStyle {
    std::string font_family;
    int font_size;
    bool bold;
    bool italic;

    bool operator==(CharacterStyle const&) const = default;
};

// Custom hash for use in unordered_map
struct StyleHash {
    size_t operator()(CharacterStyle const& s) const {
        auto h1 = std::hash<std::string>{}(s.font_family);
        auto h2 = std::hash<int>{}(s.font_size);
        auto h3 = std::hash<bool>{}(s.bold);
        auto h4 = std::hash<bool>{}(s.italic);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

// ============================================================================
// Flyweight Factory caches shared state objects
// ============================================================================

class StyleFactory {
public:
    std::shared_ptr<CharacterStyle const> get_style(
        std::string font, int size, bool bold = false, bool italic = false) {

        CharacterStyle key{std::move(font), size, bold, italic};
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            ++cache_hits_;
            return it->second;
        }

        auto style = std::make_shared<CharacterStyle const>(std::move(key));
        cache_[*style] = style;
        return style;
    }

    size_t unique_styles() const { return cache_.size(); }
    size_t cache_hits() const { return cache_hits_; }

private:
    std::unordered_map<CharacterStyle, std::shared_ptr<CharacterStyle const>, StyleHash> cache_;
    size_t cache_hits_{0};
};

// ============================================================================
// Context the extrinsic state (unique per instance)
// ============================================================================

struct FormattedChar {
    char character;                              // Extrinsic: unique per char
    int position;                                // Extrinsic: unique per char
    std::shared_ptr<CharacterStyle const> style; // Intrinsic: SHARED (flyweight)
};

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    StyleFactory factory;

    // Simulate a document where most text uses the same few styles
    std::vector<FormattedChar> document;

    std::string text = "Hello, World! This is a flyweight demo.";

    // Most characters share "Arial 12pt" style
    auto default_style = factory.get_style("Arial", 12);

    for (int i = 0; i < static_cast<int>(text.size()); ++i) {
        document.push_back({text[i], i, default_style});
    }

    // A few characters are bold
    auto bold_style = factory.get_style("Arial", 12, true);
    for (int i = 0; i < 5; ++i) {  // "Hello" is bold
        document[i].style = bold_style;
    }

    // Request the same styles again should hit cache
    auto same_default = factory.get_style("Arial", 12);
    auto same_bold = factory.get_style("Arial", 12, true);

    std::cout << "=== Flyweight Pattern ===\n";
    std::cout << "Document length: " << document.size() << " characters\n";
    std::cout << "Unique styles allocated: " << factory.unique_styles() << '\n';
    std::cout << "Cache hits: " << factory.cache_hits() << '\n';

    // Verify sharing
    assert(factory.unique_styles() == 2);  // Only 2 unique styles
    assert(same_default.get() == default_style.get());  // Same pointer!
    assert(same_bold.get() == bold_style.get());
    assert(factory.cache_hits() == 2);

    // All 39 chars share just 2 style objects
    assert(document.size() == text.size());

    std::cout << "\nMemory saved: instead of " << document.size()
              << " style objects, only " << factory.unique_styles() << " exist.\n";
    std::cout << "All assertions passed.\n";
    return EXIT_SUCCESS;
}
