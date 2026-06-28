/**
 * @file segment_tree.cpp
 * @brief Segment Tree - Range Sum Query with Point Updates
 * 
 * A Segment Tree is a binary tree where each node stores aggregate info
 * about a range of the underlying array.
 * 
 * Operations (all O(log n)):
 *   - build(arr): construct tree from array
 *   - update(idx, val): change a single element
 *   - query(l, r): get aggregate (sum/min/max) over range [l, r]
 * 
 * Structure: stored as array of size 4*n (complete binary tree)
 *   - Node i: children at 2*i and 2*i+1
 *   - Represents range that is split in half at each level
 * 
 * Use cases: range sum/min/max with updates, count inversions,
 * interval scheduling, computational geometry.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread segment_tree.cpp -o segment_tree
 */

#include <iostream>
#include <vector>
#include <functional>

template <typename T, typename Op = std::plus<T>>
class SegmentTree {
    std::vector<T> tree_;
    int n_;
    T identity_;  // Identity element (0 for sum, INT_MAX for min, etc.)
    Op op_;

public:
    SegmentTree(const std::vector<T>& arr, T identity = T{}, Op op = Op{})
        : n_(arr.size()), identity_(identity), op_(op) {
        tree_.resize(4 * n_, identity_);
        build(arr, 1, 0, n_ - 1);
    }

    // Point update: set arr[idx] += val (or replace)
    void update(int idx, T val) {
        update(1, 0, n_ - 1, idx, val);
    }

    // Range query: get op(arr[l..r])
    T query(int l, int r) const {
        return query(1, 0, n_ - 1, l, r);
    }

    int size() const { return n_; }

private:
    void build(const std::vector<T>& arr, int node, int start, int end) {
        if (start == end) {
            tree_[node] = arr[start];
            return;
        }
        int mid = (start + end) / 2;
        build(arr, 2 * node, start, mid);
        build(arr, 2 * node + 1, mid + 1, end);
        tree_[node] = op_(tree_[2 * node], tree_[2 * node + 1]);
    }

    void update(int node, int start, int end, int idx, T val) {
        if (start == end) {
            tree_[node] += val;  // Add val (for point increment)
            return;
        }
        int mid = (start + end) / 2;
        if (idx <= mid) update(2 * node, start, mid, idx, val);
        else update(2 * node + 1, mid + 1, end, idx, val);
        tree_[node] = op_(tree_[2 * node], tree_[2 * node + 1]);
    }

    T query(int node, int start, int end, int l, int r) const {
        if (r < start || end < l) return identity_;        // Out of range
        if (l <= start && end <= r) return tree_[node];    // Fully within range
        int mid = (start + end) / 2;
        return op_(query(2 * node, start, mid, l, r),
                   query(2 * node + 1, mid + 1, end, l, r));
    }
};

int main() {
    std::cout << "=== Segment Tree - Range Sum Query ===\n\n";

    std::vector<int> arr = {1, 3, 5, 7, 9, 11};
    std::cout << "Array: ";
    for (int x : arr) std::cout << x << " ";
    std::cout << "\n\n";

    SegmentTree<int> st(arr);

    // Range queries
    std::cout << "--- Range Sum Queries ---\n";
    std::cout << "sum[0..2] = " << st.query(0, 2) << " (expected: 9 = 1+3+5)\n";
    std::cout << "sum[1..4] = " << st.query(1, 4) << " (expected: 24 = 3+5+7+9)\n";
    std::cout << "sum[0..5] = " << st.query(0, 5) << " (expected: 36 = total)\n";
    std::cout << "sum[3..3] = " << st.query(3, 3) << " (expected: 7 = single element)\n\n";

    // Point update
    std::cout << "--- Point Update: arr[2] += 5 (5 → 10) ---\n";
    st.update(2, 5);
    std::cout << "sum[0..2] = " << st.query(0, 2) << " (expected: 14 = 1+3+10)\n";
    std::cout << "sum[0..5] = " << st.query(0, 5) << " (expected: 41)\n\n";

    // Segment Tree for Range Minimum
    std::cout << "--- Range Minimum Query ---\n";
    auto minOp = [](int a, int b) { return std::min(a, b); };
    SegmentTree<int, decltype(minOp)> minTree(arr, INT32_MAX, minOp);
    std::cout << "min[0..5] = " << minTree.query(0, 5) << " (expected: 1)\n";
    std::cout << "min[2..4] = " << minTree.query(2, 4) << " (expected: 5)\n";

    return 0;
}
