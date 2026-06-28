/**************************************************************************************************
 * Topic: Composite Pattern Tree Structures with Uniform Interface
 *
 * Interview Relevance: ★★★★
 * What interviewers look for:
 *   - Treating leaves and composites uniformly via base interface
 *   - Recursive traversal of tree structures
 *   - Modern C++: unique_ptr ownership, variant-based composites
 *   - Real-world: file systems, UI widget trees, org charts, expression trees
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o composite composite.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

// ============================================================================
// Component interface common to both leaf and composite nodes
// ============================================================================

class FileSystemEntry {
public:
    virtual ~FileSystemEntry() = default;
    virtual std::string name() const = 0;
    virtual size_t size() const = 0;
    virtual void print(int indent = 0) const = 0;
    virtual bool is_directory() const { return false; }
};

// ============================================================================
// Leaf a file (terminal node)
// ============================================================================

class File : public FileSystemEntry {
public:
    File(std::string name, size_t size) : name_(std::move(name)), size_(size) {}

    std::string name() const override { return name_; }
    size_t size() const override { return size_; }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << name_ << " (" << size_ << " bytes)\n";
    }

private:
    std::string name_;
    size_t size_;
};

// ============================================================================
// Composite a directory (contains other entries)
// ============================================================================

class Directory : public FileSystemEntry {
public:
    explicit Directory(std::string name) : name_(std::move(name)) {}

    std::string name() const override { return name_; }

    // Size is the sum of all children (recursive)
    size_t size() const override {
        return std::accumulate(children_.begin(), children_.end(), size_t{0},
                               [](size_t sum, auto const& child) {
                                   return sum + child->size();
                               });
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "[" << name_ << "/] ("
                  << size() << " bytes)\n";
        for (auto const& child : children_) {
            child->print(indent + 2);
        }
    }

    bool is_directory() const override { return true; }

    void add(std::unique_ptr<FileSystemEntry> entry) {
        children_.push_back(std::move(entry));
    }

    size_t count() const { return children_.size(); }

private:
    std::string name_;
    std::vector<std::unique_ptr<FileSystemEntry>> children_;
};

// ============================================================================
// Client code works uniformly with both files and directories
// ============================================================================

size_t total_size(FileSystemEntry const& entry) {
    return entry.size();  // Polymorphic recursion handled inside Directory
}

int main() {
    // Build a file system tree
    auto root = std::make_unique<Directory>("project");

    auto src = std::make_unique<Directory>("src");
    src->add(std::make_unique<File>("main.cpp", 2048));
    src->add(std::make_unique<File>("utils.cpp", 1024));
    src->add(std::make_unique<File>("utils.h", 512));

    auto build = std::make_unique<Directory>("build");
    build->add(std::make_unique<File>("app.o", 8192));
    build->add(std::make_unique<File>("app", 32768));

    root->add(std::move(src));
    root->add(std::move(build));
    root->add(std::make_unique<File>("CMakeLists.txt", 256));

    // Uniform interface
    std::cout << "=== File System (Composite Pattern) ===\n";
    root->print();

    // Assertions
    assert(root->size() == 2048 + 1024 + 512 + 8192 + 32768 + 256);
    assert(root->is_directory());
    assert(root->count() == 3);

    std::cout << "\nTotal size: " << total_size(*root) << " bytes\n";
    std::cout << "All assertions passed.\n";
    return EXIT_SUCCESS;
}
