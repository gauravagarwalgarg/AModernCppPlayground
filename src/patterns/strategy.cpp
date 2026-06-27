// strategy.cpp - Strategy Pattern
// Interview Relevance: Demonstrates runtime algorithm selection, dependency inversion,
// and Open/Closed principle. Tests understanding of polymorphism vs conditionals.
// Real use: sorting algorithms, compression strategies, payment methods.
// Compile: g++ -std=c++20 -Wall strategy.cpp -o strategy

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

class SortStrategy {
public:
    virtual ~SortStrategy() = default;
    virtual void sort(std::vector<int>& data) = 0;
    virtual std::string name() const = 0;
};

class BubbleSort : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        for (size_t i = 0; i < data.size(); ++i)
            for (size_t j = 0; j + 1 < data.size() - i; ++j)
                if (data[j] > data[j + 1])
                    std::swap(data[j], data[j + 1]);
    }
    std::string name() const override { return "BubbleSort"; }
};

class QuickSort : public SortStrategy {
    void qsort(std::vector<int>& d, int lo, int hi) {
        if (lo >= hi) return;
        int pivot = d[hi], i = lo;
        for (int j = lo; j < hi; ++j)
            if (d[j] < pivot) std::swap(d[i++], d[j]);
        std::swap(d[i], d[hi]);
        qsort(d, lo, i - 1);
        qsort(d, i + 1, hi);
    }
public:
    void sort(std::vector<int>& data) override {
        if (!data.empty()) qsort(data, 0, static_cast<int>(data.size()) - 1);
    }
    std::string name() const override { return "QuickSort"; }
};

class MergeSort : public SortStrategy {
public:
    void sort(std::vector<int>& data) override {
        if (data.size() <= 1) return;
        auto mid = data.begin() + data.size() / 2;
        std::vector<int> left(data.begin(), mid), right(mid, data.end());
        sort(left); sort(right);
        std::merge(left.begin(), left.end(), right.begin(), right.end(), data.begin());
    }
    std::string name() const override { return "MergeSort"; }
};

// Context - holds a strategy and delegates sorting
class Sorter {
    std::unique_ptr<SortStrategy> strategy_;
public:
    void setStrategy(std::unique_ptr<SortStrategy> s) { strategy_ = std::move(s); }

    void sort(std::vector<int>& data) {
        std::cout << "Sorting with " << strategy_->name() << ": ";
        strategy_->sort(data);
        for (int x : data) std::cout << x << " ";
        std::cout << "\n";
    }
};

int main() {
    Sorter sorter;
    std::vector<int> expected = {1, 2, 3, 5, 8};

    // Runtime algorithm swap - same interface, different behavior
    std::vector<int> data1 = {5, 3, 8, 1, 2};
    sorter.setStrategy(std::make_unique<BubbleSort>());
    sorter.sort(data1);
    assert(data1 == expected);

    std::vector<int> data2 = {5, 3, 8, 1, 2};
    sorter.setStrategy(std::make_unique<QuickSort>());
    sorter.sort(data2);
    assert(data2 == expected);

    std::vector<int> data3 = {5, 3, 8, 1, 2};
    sorter.setStrategy(std::make_unique<MergeSort>());
    sorter.sort(data3);
    assert(data3 == expected);

    std::cout << "\nKey insight: Client code (Sorter) doesn't change when new\n";
    std::cout << "algorithms are added. Strategy encapsulates the 'how'.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
