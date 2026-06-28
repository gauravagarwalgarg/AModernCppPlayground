/*
 * What: C++ memory orderings: relaxed, acquire, release, seq_cst.
 *       Counter with relaxed ordering. Producer-consumer with acquire/release.
 *
 * Why: The C++ memory model defines how memory operations become visible across
 *      threads. In HFT, choosing the weakest sufficient ordering (e.g., relaxed
 *      for counters, acquire/release for message passing) eliminates unnecessary
 *      memory fences and can save 10-50ns per operation on x86.
 *
 * Interviewers look for:
 *   - Understanding of happens-before relationships
 *   - When relaxed is safe (independent counters, statistics)
 *   - Acquire/release pairs for producer-consumer (publish pattern)
 *   - seq_cst as the safe default and when it's overkill
 *
 * Pitfalls:
 *   - Relaxed ordering provides NO synchronization between threads
 *   - Acquire/release must be paired on the SAME atomic variable
 *   - x86 is strongly ordered - bugs may hide until porting to ARM
 *   - seq_cst has global total order overhead (MFENCE on x86)
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <cassert>

// 1. Relaxed ordering: fine for simple counters (no ordering guarantees needed)
std::atomic<int> stats_counter{0};

void count_events(int n) {
    for (int i = 0; i < n; ++i) {
        // Relaxed: just need atomicity, not ordering with other data
        stats_counter.fetch_add(1, std::memory_order_relaxed);
    }
}

// 2. Acquire/Release: producer-consumer message passing pattern
struct Message {
    int x = 0, y = 0, z = 0;
};

Message payload;                          // non-atomic shared data
std::atomic<bool> ready{false};           // synchronization flag

void producer() {
    // Write payload BEFORE signaling ready
    payload = {1, 2, 3};
    // Release: all writes before this are visible to acquiring thread
    ready.store(true, std::memory_order_release);
}

void consumer() {
    // Acquire: sees all writes that happened before the release store
    while (!ready.load(std::memory_order_acquire)) {
        // spin-wait
    }
    // Guaranteed to see payload = {1, 2, 3}
    assert(payload.x == 1 && payload.y == 2 && payload.z == 3);
    std::cout << "Consumer sees: {" << payload.x << ", "
              << payload.y << ", " << payload.z << "}\n";
}

// 3. Sequential consistency: strongest ordering (default)
std::atomic<bool> flag_a{false}, flag_b{false};
std::atomic<int> observer_count{0};

// Dekker-style pattern: needs seq_cst for correctness on ARM
void thread_a() {
    flag_a.store(true, std::memory_order_seq_cst);
    if (!flag_b.load(std::memory_order_seq_cst)) {
        observer_count.fetch_add(1, std::memory_order_relaxed);
    }
}

void thread_b() {
    flag_b.store(true, std::memory_order_seq_cst);
    if (!flag_a.load(std::memory_order_seq_cst)) {
        observer_count.fetch_add(1, std::memory_order_relaxed);
    }
}

int main() {
    // 1. Relaxed counter - order doesn't matter, just final count
    constexpr int N = 4, PER = 10000;
    std::vector<std::thread> threads;
    for (int i = 0; i < N; ++i) threads.emplace_back(count_events, PER);
    for (auto& t : threads) t.join();
    assert(stats_counter.load() == N * PER);
    std::cout << "Relaxed counter: " << stats_counter.load() << "\n\n";

    // 2. Acquire/Release message passing
    std::thread t_prod(producer);
    std::thread t_cons(consumer);
    t_prod.join();
    t_cons.join();
    std::cout << "Acquire/Release message passing: correct!\n\n";

    // 3. Seq_cst - at most one thread enters critical section
    // (With weaker orderings, BOTH could enter on ARM)
    for (int trial = 0; trial < 1000; ++trial) {
        flag_a = false; flag_b = false; observer_count = 0;
        std::thread ta(thread_a);
        std::thread tb(thread_b);
        ta.join(); tb.join();
        // With seq_cst, at least one thread must see the other's flag
        assert(observer_count <= 1);
    }
    std::cout << "Seq_cst: Dekker's pattern validated over 1000 trials\n";

    std::cout << "\nAll memory model tests passed!\n";
    return 0;
}
