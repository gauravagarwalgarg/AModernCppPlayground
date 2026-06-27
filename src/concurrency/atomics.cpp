/*
 * What: std::atomic<int>, std::atomic<bool>. Compare-and-swap (CAS).
 *       Atomic flag for spinlock. fetch_add. Lock-free counter.
 *
 * Why: Atomics provide lock-free synchronization for simple shared state.
 *      In HFT, avoiding mutex overhead on hot paths (order counts, sequence
 *      numbers) is critical for sub-microsecond latency.
 *
 * Interviewers look for:
 *   - Understanding of atomicity guarantees vs mutex-based protection
 *   - CAS loop pattern (compare_exchange_weak in a loop)
 *   - Difference between weak and strong CAS (spurious failure on weak)
 *   - When atomics are sufficient vs when you need a mutex
 *
 * Pitfalls:
 *   - Atomic != thread-safe data structure (only individual operations are atomic)
 *   - compare_exchange_weak can fail spuriously (use in loop)
 *   - False sharing when atomics are on same cache line
 *   - Not all types are lock-free (check is_lock_free())
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <cassert>

// 1. Lock-free counter with fetch_add
std::atomic<int> counter{0};

void increment_counter(int n) {
    for (int i = 0; i < n; ++i) {
        counter.fetch_add(1, std::memory_order_relaxed);
    }
}

// 2. Spinlock using atomic_flag
class SpinLock {
    std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
public:
    void lock() {
        while (flag_.test_and_set(std::memory_order_acquire)) {
            // spin - in production, add pause/yield hint
        }
    }
    void unlock() {
        flag_.clear(std::memory_order_release);
    }
};

// 3. CAS-based lock-free max update
std::atomic<int> shared_max{0};

void update_max(int proposed) {
    int current = shared_max.load(std::memory_order_relaxed);
    // CAS loop: retry if another thread changed the value
    while (proposed > current &&
           !shared_max.compare_exchange_weak(current, proposed,
                                             std::memory_order_release,
                                             std::memory_order_relaxed)) {
        // current is updated by compare_exchange_weak on failure
    }
}

int main() {
    // 1. Atomic counter - no data race without mutex
    constexpr int THREADS = 8;
    constexpr int PER_THREAD = 10000;
    std::vector<std::thread> threads;
    for (int i = 0; i < THREADS; ++i)
        threads.emplace_back(increment_counter, PER_THREAD);
    for (auto& t : threads) t.join();
    assert(counter.load() == THREADS * PER_THREAD);
    std::cout << "Atomic counter: " << counter.load()
              << " (expected " << THREADS * PER_THREAD << ")\n";
    std::cout << "counter is lock-free: " << std::boolalpha
              << counter.is_lock_free() << "\n\n";

    // 2. Spinlock protecting shared resource
    SpinLock spinlock;
    int shared_val = 0;
    threads.clear();
    for (int i = 0; i < THREADS; ++i) {
        threads.emplace_back([&]() {
            for (int j = 0; j < 1000; ++j) {
                spinlock.lock();
                ++shared_val;
                spinlock.unlock();
            }
        });
    }
    for (auto& t : threads) t.join();
    assert(shared_val == THREADS * 1000);
    std::cout << "Spinlock-protected counter: " << shared_val << "\n\n";

    // 3. CAS-based max
    threads.clear();
    for (int i = 0; i < THREADS; ++i)
        threads.emplace_back(update_max, i * 10);
    for (auto& t : threads) t.join();
    assert(shared_max.load() == (THREADS - 1) * 10);
    std::cout << "CAS max: " << shared_max.load() << "\n";

    std::cout << "All atomics tests passed!\n";
    return 0;
}
