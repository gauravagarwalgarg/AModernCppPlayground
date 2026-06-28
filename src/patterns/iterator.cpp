/**************************************************************************************************
 * Topic: Iterator Pattern Custom Traversal of Collections
 *
 * Interview Relevance: ★★★★
 * What interviewers look for:
 *   - STL iterator concepts (begin/end, input/output/forward/bidirectional/random)
 *   - Writing a custom iterator for a custom container
 *   - Range-based for loop compatibility
 *   - C++20 ranges and sentinels
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o iterator iterator.cpp
 **************************************************************************************************/

#include <array>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

// ============================================================================
// A custom container: Ring Buffer with a custom iterator
// ============================================================================

template <typename T, size_t Capacity>
class RingBuffer {
public:
    // ---- Custom Iterator (Forward Iterator) ----
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T const*;
        using reference = T const&;

        Iterator(T const* data, size_t head, size_t count, size_t index)
            : data_(data), head_(head), count_(count), index_(index) {}

        reference operator*() const {
            return data_[(head_ + index_) % Capacity];
        }
        pointer operator->() const { return &(**this); }

        Iterator& operator++() { ++index_; return *this; }
        Iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }

        bool operator==(Iterator const& other) const { return index_ == other.index_; }
        bool operator!=(Iterator const& other) const { return !(*this == other); }

    private:
        T const* data_;
        size_t head_;
        size_t count_;
        size_t index_;
    };

    // ---- Container interface ----
    void push(T value) {
        data_[(head_ + size_) % Capacity] = std::move(value);
        if (size_ < Capacity) {
            ++size_;
        } else {
            head_ = (head_ + 1) % Capacity;  // Overwrite oldest
        }
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }

    T const& front() const { return data_[head_]; }
    T const& back() const { return data_[(head_ + size_ - 1) % Capacity]; }

    // Range-based for loop support
    Iterator begin() const { return Iterator(data_.data(), head_, size_, 0); }
    Iterator end() const { return Iterator(data_.data(), head_, size_, size_); }

private:
    std::array<T, Capacity> data_{};
    size_t head_{0};
    size_t size_{0};
};

// ============================================================================
// Generic algorithms work with our custom iterator
// ============================================================================

template <typename Iter>
auto sum(Iter begin, Iter end) {
    typename std::iterator_traits<Iter>::value_type total{};
    for (auto it = begin; it != end; ++it) {
        total += *it;
    }
    return total;
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Iterator Pattern (Custom Ring Buffer) ===\n\n";

    RingBuffer<int, 5> ring;

    // Push 7 items into capacity-5 buffer (overwrites oldest 2)
    for (int i = 1; i <= 7; ++i) {
        ring.push(i * 10);
    }

    // Should contain: 30, 40, 50, 60, 70 (oldest 10, 20 overwritten)
    std::cout << "Ring buffer contents: ";
    for (auto val : ring) {  // Range-based for loop works!
        std::cout << val << " ";
    }
    std::cout << '\n';

    assert(ring.size() == 5);
    assert(ring.front() == 30);
    assert(ring.back() == 70);

    // Our generic sum works with the custom iterator
    auto total = sum(ring.begin(), ring.end());
    assert(total == 30 + 40 + 50 + 60 + 70);
    std::cout << "Sum: " << total << '\n';

    // Works with STL algorithms too
    std::vector<int> vec(ring.begin(), ring.end());
    assert(vec.size() == 5);
    assert(vec[0] == 30);
    assert(vec[4] == 70);

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
