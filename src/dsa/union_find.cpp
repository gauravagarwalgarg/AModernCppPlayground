/**
 * @file union_find.cpp
 * @brief Union-Find (Disjoint Set Union) with Path Compression & Union by Rank
 * 
 * Operations:
 *   - find(x): returns root representative of x's set, O(α(n)) ≈ O(1)
 *   - unite(x, y): merges sets containing x and y
 *   - connected(x, y): checks if x and y are in same set
 * 
 * Optimizations:
 *   - Path compression: flatten tree during find (point directly to root)
 *   - Union by rank: attach shorter tree under taller tree (keep height O(log n))
 *   - Together: nearly O(1) per operation (inverse Ackermann function)
 * 
 * Applications: connected components, Kruskal's MST, redundant edges, cycle detection
 * 
 * Compile: g++ -std=c++20 -Wall -pthread union_find.cpp -o union_find
 */

#include <iostream>
#include <vector>
#include <numeric>

template <typename T = int>
class UnionFind {
    std::vector<int> parent_;
    std::vector<int> rank_;
    int components_;

public:
    explicit UnionFind(int n) : parent_(n), rank_(n, 0), components_(n) {
        std::iota(parent_.begin(), parent_.end(), 0);  // parent[i] = i
    }

    // Find with path compression - flattens tree for future queries
    int find(int x) {
        if (parent_[x] != x) {
            parent_[x] = find(parent_[x]);  // Path compression: point to root
        }
        return parent_[x];
    }

    // Union by rank - keeps tree balanced
    bool unite(int x, int y) {
        int rx = find(x), ry = find(y);
        if (rx == ry) return false;  // Already in same set

        // Attach smaller rank tree under root of larger rank tree
        if (rank_[rx] < rank_[ry]) std::swap(rx, ry);
        parent_[ry] = rx;
        if (rank_[rx] == rank_[ry]) ++rank_[rx];

        --components_;
        return true;
    }

    bool connected(int x, int y) { return find(x) == find(y); }
    int numComponents() const { return components_; }
};

// === Application 1: Count Connected Components ===
int countComponents(int n, const std::vector<std::pair<int,int>>& edges) {
    UnionFind<> uf(n);
    for (const auto& [u, v] : edges) {
        uf.unite(u, v);
    }
    return uf.numComponents();
}

// === Application 2: Find Redundant Edge (LC 684) ===
// In a tree with n nodes, there are n-1 edges. One extra edge creates a cycle.
// The redundant edge is the first edge that connects two already-connected nodes.
std::pair<int,int> findRedundantConnection(const std::vector<std::pair<int,int>>& edges) {
    int n = edges.size();
    UnionFind<> uf(n + 1);  // 1-indexed nodes

    for (const auto& [u, v] : edges) {
        if (!uf.unite(u, v)) {
            return {u, v};  // This edge creates a cycle!
        }
    }
    return {-1, -1};
}

int main() {
    std::cout << "=== Union-Find (Disjoint Set Union) ===\n\n";

    // Basic operations demo
    std::cout << "--- Basic Operations ---\n";
    UnionFind<> uf(7);
    std::cout << "Initial components: " << uf.numComponents() << "\n";

    uf.unite(0, 1);
    uf.unite(2, 3);
    uf.unite(4, 5);
    std::cout << "After unite(0,1), (2,3), (4,5): " << uf.numComponents() << " components\n";

    uf.unite(1, 2);  // Merges {0,1} and {2,3}
    std::cout << "After unite(1,2): " << uf.numComponents() << " components\n";
    std::cout << "connected(0,3) = " << std::boolalpha << uf.connected(0, 3) << "\n";
    std::cout << "connected(0,4) = " << uf.connected(0, 4) << "\n\n";

    // Application 1: Connected Components
    std::cout << "--- Count Connected Components ---\n";
    std::vector<std::pair<int,int>> graph = {{0,1}, {1,2}, {3,4}};
    std::cout << "5 nodes, edges: (0,1)(1,2)(3,4)\n";
    std::cout << "Components: " << countComponents(5, graph) << " (expected: 2)\n\n";

    // Application 2: Redundant Edge
    std::cout << "--- Find Redundant Connection (LC 684) ---\n";
    std::vector<std::pair<int,int>> tree_edges = {{1,2}, {1,3}, {2,3}};
    auto [u, v] = findRedundantConnection(tree_edges);
    std::cout << "Edges: (1,2)(1,3)(2,3)\n";
    std::cout << "Redundant: (" << u << "," << v << ") (expected: (2,3))\n";

    return 0;
}
