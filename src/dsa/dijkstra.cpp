/**************************************************************************************************
 * Topic: Dijkstra's Algorithm Single-Source Shortest Path
 *
 * Interview Relevance: ★★★★★
 * Time: O((V + E) log V) with priority queue
 * Space: O(V)
 *
 * What interviewers look for:
 *   - Correct priority queue usage (min-heap)
 *   - Handling visited nodes (lazy deletion vs decrease-key)
 *   - When Dijkstra fails (negative edges)
 *   - Path reconstruction
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o dijkstra dijkstra.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <queue>
#include <vector>

constexpr int INF = std::numeric_limits<int>::max();

struct Edge {
    int to;
    int weight;
};

using Graph = std::vector<std::vector<Edge>>;

// Returns {distances, predecessors} from source
std::pair<std::vector<int>, std::vector<int>> dijkstra(Graph const& graph, int source) {
    int n = static_cast<int>(graph.size());
    std::vector<int> dist(n, INF);
    std::vector<int> prev(n, -1);

    // Min-heap: {distance, node}
    using PQEntry = std::pair<int, int>;
    std::priority_queue<PQEntry, std::vector<PQEntry>, std::greater<>> pq;

    dist[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        // Lazy deletion: skip if we've already found a shorter path
        if (d > dist[u]) continue;

        for (auto const& [v, w] : graph[u]) {
            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    return {dist, prev};
}

// Reconstruct path from source to target
std::vector<int> reconstruct_path(std::vector<int> const& prev, int source, int target) {
    std::vector<int> path;
    for (int v = target; v != -1; v = prev[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    if (path.front() != source) return {};  // No path exists
    return path;
}

int main() {
    std::cout << "=== Dijkstra's Shortest Path ===\n\n";

    // Build graph:
    //   0 --4--> 1 --1--> 3
    //   |        ^        ^
    //   2        |        |
    //   |        1        3
    //   v        |        |
    //   2 -------+--5---> 3
    Graph graph(4);
    graph[0].push_back({1, 4});
    graph[0].push_back({2, 2});
    graph[2].push_back({1, 1});
    graph[2].push_back({3, 5});
    graph[1].push_back({3, 1});

    auto [dist, prev] = dijkstra(graph, 0);

    std::cout << "Distances from node 0:\n";
    for (int i = 0; i < 4; ++i) {
        std::cout << "  to " << i << ": " << dist[i] << '\n';
    }

    auto path = reconstruct_path(prev, 0, 3);
    std::cout << "\nShortest path 0→3: ";
    for (int v : path) std::cout << v << " ";
    std::cout << "(cost: " << dist[3] << ")\n";

    // Assertions
    assert(dist[0] == 0);
    assert(dist[1] == 3);  // 0→2→1
    assert(dist[2] == 2);  // 0→2
    assert(dist[3] == 4);  // 0→2→1→3
    assert(path == (std::vector<int>{0, 2, 1, 3}));

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
