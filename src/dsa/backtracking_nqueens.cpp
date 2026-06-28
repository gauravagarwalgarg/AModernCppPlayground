/**************************************************************************************************
 * Topic: N-Queens Problem Backtracking with Constraint Pruning
 *
 * Interview Relevance: ★★★★★
 * Time: O(N!) in worst case, pruning makes it much faster
 * Space: O(N) for the board state
 *
 * What interviewers look for:
 *   - Backtracking framework: choose → explore → unchoose
 *   - Efficient conflict detection (column, diagonal sets)
 *   - Generating all solutions vs first solution
 *   - Optimization: using bitmasks for O(1) conflict checks
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o backtracking_nqueens backtracking_nqueens.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// N-Queens Solver using bitmask pruning
// ============================================================================

class NQueensSolver {
public:
    explicit NQueensSolver(int n) : n_(n), queens_(n) {}

    // Find all solutions
    std::vector<std::vector<int>> solve_all() {
        solutions_.clear();
        cols_ = diag1_ = diag2_ = 0;
        backtrack(0);
        return solutions_;
    }

    // Find just the count (faster no solution storage)
    int count_solutions() {
        count_ = 0;
        cols_ = diag1_ = diag2_ = 0;
        count_backtrack(0);
        return count_;
    }

private:
    int n_;
    std::vector<int> queens_;  // queens_[row] = column
    std::vector<std::vector<int>> solutions_;
    int count_{0};

    // Bitmasks for O(1) conflict detection
    uint32_t cols_{0};   // Columns occupied
    uint32_t diag1_{0};  // Main diagonals (row - col + n - 1)
    uint32_t diag2_{0};  // Anti-diagonals (row + col)

    bool is_safe(int row, int col) const {
        uint32_t c = 1u << col;
        uint32_t d1 = 1u << (row - col + n_ - 1);
        uint32_t d2 = 1u << (row + col);
        return !(cols_ & c) && !(diag1_ & d1) && !(diag2_ & d2);
    }

    void place(int row, int col) {
        queens_[row] = col;
        cols_ |= (1u << col);
        diag1_ |= (1u << (row - col + n_ - 1));
        diag2_ |= (1u << (row + col));
    }

    void remove(int row, int col) {
        cols_ ^= (1u << col);
        diag1_ ^= (1u << (row - col + n_ - 1));
        diag2_ ^= (1u << (row + col));
    }

    void backtrack(int row) {
        if (row == n_) {
            solutions_.push_back(queens_);
            return;
        }
        for (int col = 0; col < n_; ++col) {
            if (is_safe(row, col)) {
                place(row, col);       // Choose
                backtrack(row + 1);    // Explore
                remove(row, col);      // Unchoose (backtrack)
            }
        }
    }

    void count_backtrack(int row) {
        if (row == n_) { ++count_; return; }
        for (int col = 0; col < n_; ++col) {
            if (is_safe(row, col)) {
                place(row, col);
                count_backtrack(row + 1);
                remove(row, col);
            }
        }
    }
};

// ============================================================================
// Print a board for visualization
// ============================================================================

void print_board(std::vector<int> const& queens) {
    int n = static_cast<int>(queens.size());
    for (int row = 0; row < n; ++row) {
        for (int col = 0; col < n; ++col) {
            std::cout << (queens[row] == col ? " Q" : " .");
        }
        std::cout << '\n';
    }
}

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== N-Queens Backtracking ===\n\n";

    // Solve 8-Queens
    NQueensSolver solver8(8);
    int count8 = solver8.count_solutions();
    std::cout << "8-Queens: " << count8 << " solutions\n";
    assert(count8 == 92);

    // Solve 4-Queens (show all solutions)
    NQueensSolver solver4(4);
    auto solutions4 = solver4.solve_all();
    std::cout << "\n4-Queens: " << solutions4.size() << " solutions\n";
    assert(solutions4.size() == 2);

    std::cout << "\nSolution 1:\n";
    print_board(solutions4[0]);
    std::cout << "\nSolution 2:\n";
    print_board(solutions4[1]);

    // Known values
    NQueensSolver solver1(1);
    assert(solver1.count_solutions() == 1);
    NQueensSolver solver2(2);
    assert(solver2.count_solutions() == 0);
    NQueensSolver solver3(3);
    assert(solver3.count_solutions() == 0);

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
