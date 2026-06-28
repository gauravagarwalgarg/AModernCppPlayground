/**************************************************************************************************
 * Topic: Prototype Pattern Polymorphic Deep Copy
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★
 * What interviewers look for:
 *   - Virtual clone() returning unique_ptr<Base>
 *   - Covariant return types
 *   - Why copy constructors can't be virtual
 *   - Registry pattern with prototypes for factory replacement
 *
 * Day-to-day application:
 *   - Config objects that are templates for runtime instances
 *   - Document/graphic editors: "stamp" tool clones a template shape
 *   - Game: spawning enemies from pre-configured prototypes
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o prototype_pattern prototype_pattern.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// ============================================================================
// Base interface with clone (Prototype pattern)
// ============================================================================

class Document {
public:
    virtual ~Document() = default;
    virtual std::unique_ptr<Document> clone() const = 0;
    virtual std::string render() const = 0;
    virtual void set_content(std::string content) = 0;
};

// ============================================================================
// Concrete document types
// ============================================================================

class MarkdownDoc : public Document {
public:
    explicit MarkdownDoc(std::string content = "") : content_(std::move(content)) {}

    std::unique_ptr<Document> clone() const override {
        return std::make_unique<MarkdownDoc>(*this);  // Copy-construct
    }

    std::string render() const override {
        return "# " + content_;
    }

    void set_content(std::string content) override {
        content_ = std::move(content);
    }

private:
    std::string content_;
};

class HTMLDoc : public Document {
public:
    explicit HTMLDoc(std::string content = "", std::string css_class = "default")
        : content_(std::move(content)), css_class_(std::move(css_class)) {}

    std::unique_ptr<Document> clone() const override {
        return std::make_unique<HTMLDoc>(*this);
    }

    std::string render() const override {
        return "<div class=\"" + css_class_ + "\">" + content_ + "</div>";
    }

    void set_content(std::string content) override {
        content_ = std::move(content);
    }

private:
    std::string content_;
    std::string css_class_;
};

// ============================================================================
// Prototype Registry Factory replacement using pre-configured prototypes
// ============================================================================

class DocumentRegistry {
public:
    void register_prototype(std::string const& name, std::unique_ptr<Document> prototype) {
        registry_[name] = std::move(prototype);
    }

    std::unique_ptr<Document> create(std::string const& name) const {
        auto it = registry_.find(name);
        if (it == registry_.end()) return nullptr;
        return it->second->clone();  // Clone the prototype
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Document>> registry_;
};

// ============================================================================
// Usage
// ============================================================================

int main() {
    // Setup registry with pre-configured prototypes
    DocumentRegistry registry;
    registry.register_prototype("blog-post", std::make_unique<MarkdownDoc>("Template"));
    registry.register_prototype("landing-page",
        std::make_unique<HTMLDoc>("Welcome", "hero-section"));

    // Clone prototypes and customize
    auto doc1 = registry.create("blog-post");
    doc1->set_content("My First Blog Post");
    std::cout << doc1->render() << '\n';

    auto doc2 = registry.create("landing-page");
    doc2->set_content("Buy Now!");
    std::cout << doc2->render() << '\n';

    // Original prototype unchanged
    auto doc3 = registry.create("blog-post");
    assert(doc3->render() == "# Template");

    // Verify clones are independent
    assert(doc1->render() == "# My First Blog Post");
    assert(doc2->render() == "<div class=\"hero-section\">Buy Now!</div>");

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
