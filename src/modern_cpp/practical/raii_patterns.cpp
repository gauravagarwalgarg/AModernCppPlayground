/**************************************************************************************************
 * Topic: RAII Patterns for Day-to-Day C++ Beyond File Handles
 * 
 * Interview Relevance: ★★★★★
 * What interviewers look for:
 *   - RAII applied to various resource types (not just files)
 *   - Scope guards for transactional safety
 *   - Custom deleters with unique_ptr for C APIs
 *   - Lock guards, connection pools, transaction commit/rollback
 *
 * Day-to-day application:
 *   - Database transactions: auto-rollback on exception
 *   - GPU resource management: buffers, textures, shaders
 *   - Network connections: auto-disconnect on scope exit
 *   - Timer guards for performance measurement
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o raii_patterns raii_patterns.cpp
 **************************************************************************************************/

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

// ============================================================================
// 1. ScopeGuard Execute cleanup on scope exit (C++17 CTAD-friendly)
// ============================================================================

template <typename F>
class ScopeGuard {
public:
    explicit ScopeGuard(F&& fn) : fn_(std::forward<F>(fn)), active_(true) {}
    ~ScopeGuard() {
        if (active_) fn_();
    }

    // Dismiss: cancel the cleanup (e.g., after successful commit)
    void dismiss() noexcept { active_ = false; }

    // Non-copyable, movable
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;
    ScopeGuard(ScopeGuard&& other) noexcept
        : fn_(std::move(other.fn_)), active_(other.active_) {
        other.active_ = false;
    }

private:
    F fn_;
    bool active_;
};

// Deduction guide for C++17 CTAD
template <typename F>
ScopeGuard(F) -> ScopeGuard<F>;

// ============================================================================
// 2. TimerGuard RAII performance measurement
// ============================================================================

class TimerGuard {
public:
    explicit TimerGuard(std::string label)
        : label_(std::move(label)), start_(std::chrono::steady_clock::now()) {}

    ~TimerGuard() {
        auto end = std::chrono::steady_clock::now();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
        std::cout << "[TIMER] " << label_ << ": " << us.count() << " µs\n";
    }

    TimerGuard(TimerGuard const&) = delete;
    TimerGuard& operator=(TimerGuard const&) = delete;

private:
    std::string label_;
    std::chrono::steady_clock::time_point start_;
};

// ============================================================================
// 3. unique_ptr with custom deleter Wrap C APIs safely
// ============================================================================

// Simulated C API
struct CHandle {
    int id;
    bool open;
};

CHandle* c_api_open(int id) {
    auto* h = new CHandle{id, true};
    std::cout << "  [C API] Opened handle " << id << '\n';
    return h;
}

void c_api_close(CHandle* h) {
    if (h) {
        std::cout << "  [C API] Closed handle " << h->id << '\n';
        h->open = false;
        delete h;
    }
}

// RAII wrapper using unique_ptr + custom deleter
using SafeHandle = std::unique_ptr<CHandle, decltype(&c_api_close)>;

SafeHandle make_safe_handle(int id) {
    return SafeHandle{c_api_open(id), &c_api_close};
}

// ============================================================================
// 4. Transaction-style RAII Commit or rollback
// ============================================================================

class DatabaseTransaction {
public:
    explicit DatabaseTransaction(std::string name)
        : name_(std::move(name)), committed_(false) {
        std::cout << "  [DB] BEGIN " << name_ << '\n';
    }

    ~DatabaseTransaction() {
        if (!committed_) {
            std::cout << "  [DB] ROLLBACK " << name_ << '\n';
        }
    }

    void commit() {
        std::cout << "  [DB] COMMIT " << name_ << '\n';
        committed_ = true;
    }

    bool is_committed() const { return committed_; }

    DatabaseTransaction(DatabaseTransaction const&) = delete;
    DatabaseTransaction& operator=(DatabaseTransaction const&) = delete;

private:
    std::string name_;
    bool committed_;
};

// ============================================================================
// Demonstration
// ============================================================================

void demonstrate_scope_guard() {
    std::cout << "\n--- ScopeGuard ---\n";
    int resource = 0;

    {
        resource = 42;
        ScopeGuard guard([&] {
            resource = 0;
            std::cout << "  ScopeGuard: resource cleaned up\n";
        });
        std::cout << "  Resource in use: " << resource << '\n';
        // guard fires at end of scope
    }
    assert(resource == 0);

    // Dismiss example
    {
        resource = 100;
        ScopeGuard guard([&] { resource = 0; });
        guard.dismiss();  // Cancel cleanup we're keeping the resource
    }
    assert(resource == 100);  // Not cleaned up!
    std::cout << "  Dismissed guard: resource kept at " << resource << '\n';
}

void demonstrate_timer() {
    std::cout << "\n--- TimerGuard ---\n";
    {
        TimerGuard timer("sleep 1ms");
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void demonstrate_c_api_wrapper() {
    std::cout << "\n--- C API Wrapper (unique_ptr + custom deleter) ---\n";
    {
        auto h1 = make_safe_handle(1);
        auto h2 = make_safe_handle(2);
        assert(h1->open);
        assert(h2->open);
        // Both handles automatically closed at scope exit
    }
}

void demonstrate_transaction() {
    std::cout << "\n--- Transaction RAII ---\n";

    // Successful transaction
    {
        DatabaseTransaction txn("insert_user");
        // ... do work ...
        txn.commit();
        assert(txn.is_committed());
    }

    // Failed transaction (auto-rollback)
    {
        DatabaseTransaction txn("transfer_funds");
        // ... exception or early return ...
        // Destructor rolls back automatically
    }
}

int main() {
    std::cout << "=== RAII Patterns for Day-to-Day C++ ===\n";

    demonstrate_scope_guard();
    demonstrate_timer();
    demonstrate_c_api_wrapper();
    demonstrate_transaction();

    std::cout << "\nAll demonstrations complete.\n";
    return EXIT_SUCCESS;
}
