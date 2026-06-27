/**
 * @file topological_sort.cpp
 * @brief Topological Sort - Kahn's BFS & DFS Approaches + Cycle Detection
 * 
 * Topological ordering: linear order of vertices such that for every edge u→v,
 * u comes before v. Only possible for DAGs (Directed Acyclic Graphs).
 * 
 * Two approaches:
 *   1. Kahn's Algorithm (BFS): Remove nodes with in-degree 0 iteratively.
 *      Detects cycle if not all nodes processed.
 *   2. DFS-based: Post-order DFS, reverse the result.
 *      Detects cycle via "currently visiting" state (back edge).
 * 
 * Classic problem: Course Schedule (LC 207/210)
 * Applications: build systems (make), task scheduling, dependency resolution
 * 
 * Compile: g++ -std=c++20 -Wall -pthread topological_sort.cpp -o topological_sort
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

// === Kahn's Algorithm (BFS) - Course Schedule II (LC 210) ===
std::vector<int> topoSortKahn(int numNodes, const std::vector<std::pair<int,int>>& edges) {
    std::vector<std::vector<int>> adj(numNodes);
    std::vector<int> in_degree(numNodes, 0);

    for (const auto& [from, to] : edges) {
        adj[from].push_back(to);
        ++in_degree[to];
    }

    // Start with all nodes having in-degree 0
    std::queue<int> q;
    for (int i = 0; i < numNodes; ++i) {
        if (in_degree[i] == 0) q.push(i);
    }

    std::vector<int> order;
    while (!q.empty()) {
        int node = q.front();
        q.pop();
        order.push_back(node);

        for (int neighbor : adj[node]) {
            if (--in_degree[neighbor] == 0) {
                q.push(neighbor);
            }
        }
    }

    // If not all nodes processed → cycle exists
    if (static_cast<int>(order.size()) != numNodes) return {};  // Cycle!
    return order;
}

// === DFS-based Topological Sort ===
class DFSTopoSort {
    enum class State { Unvisited, Visiting, Visited };

    std::vector<std::vector<int>> adj_;
    std::vector<State> state_;
    std::vector<int> order_;
    bool has_cycle_ = false;

    void dfs(int node) {
        if (has_cycle_) return;
        state_[node] = State::Visiting;  // Mark as "in current path"

        for (int neighbor : adj_[node]) {
            if (state_[neighbor] == State::Visiting) {
                has_cycle_ = true;  // Back edge → cycle!
                return;
            }
            if (state_[neighbor] == State::Unvisited) {
                dfs(neighbor);
            }
        }

        state_[node] = State::Visited;
        order_.push_back(node);  // Post-order: add after all descendants
    }

public:
    std::vector<int> sort(int numNodes, const std::vector<std::pair<int,int>>& edges) {
        adj_.assign(numNodes, {});
        state_.assign(numNodes, State::Unvisited);
        order_.clear();
        has_cycle_ = false;

        for (const auto& [from, to] : edges) {
            adj_[from].push_back(to);
        }

        for (int i = 0; i < numNodes; ++i) {
            if (state_[i] == State::Unvisited) {
                dfs(i);
            }
        }

        if (has_cycle_) return {};
        std::reverse(order_.begin(), order_.end());  // Reverse post-order
        return order_;
    }
};

int main() {
    std::cout << "=== Topological Sort ===\n\n";

    // Course Schedule: 6 courses, prerequisites as edges
    // 0→1, 0→2, 1→3, 2→3, 3→4, 4→5
    int n = 6;
    std::vector<std::pair<int,int>> edges = {{0,1}, {0,2}, {1,3}, {2,3}, {3,4}, {4,5}};

    std::cout << "--- Kahn's Algorithm (BFS) ---\n";
    std::cout << "Graph: 0→1, 0→2, 1→3, 2→3, 3→4, 4→5\n";
    auto kahn_order = topoSortKahn(n, edges);
    std::cout << "Order: ";
    for (int x : kahn_order) std::cout << x << " ";
    std::cout << "\n\n";

    std::cout << "--- DFS-based Topological Sort ---\n";
    DFSTopoSort dfs_sorter;
    auto dfs_order = dfs_sorter.sort(n, edges);
    std::cout << "Order: ";
    for (int x : dfs_order) std::cout << x << " ";
    std::cout << "\n\n";

    // Cycle detection
    std::cout << "--- Cycle Detection ---\n";
    std::vector<std::pair<int,int>> cyclic_edges = {{0,1}, {1,2}, {2,0}};
    auto result = topoSortKahn(3, cyclic_edges);
    std::cout << "Graph with cycle (0→1→2→0): ";
    std::cout << (result.empty() ? "CYCLE DETECTED" : "valid order") << "\n";

    auto dfs_result = dfs_sorter.sort(3, cyclic_edges);
    std::cout << "DFS also detects: " << (dfs_result.empty() ? "CYCLE DETECTED" : "valid order") << "\n";

    return 0;
}
