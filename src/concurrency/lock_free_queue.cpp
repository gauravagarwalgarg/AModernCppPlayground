/*
 * What: Single-Producer Single-Consumer (SPSC) lock-free ring buffer using atomics.
 *       Key HFT data structure for inter-thread communication.
 *
 * Why: In HFT, the market data thread must pass messages to the strategy thread
 *      with minimal latency. Lock-free SPSC queues achieve this because:
 *      - No syscalls (unlike mutex which may call futex)
 *      - No context switches (no blocking)
 *      - Cache-friendly (sequential memory access, power-of-2 sizing)
 *      - Bounded memory (no allocations in hot path)
 *
 * Interviewers look for:
 *   - Understanding of SPSC constraint (only one writer, one reader)
 *   - Correct use of acquire/release for head/tail synchronization
 *   - Power-of-2 capacity for fast modulo (bitwise AND)
 *   - Why this is superior to mutex-based queue in latency-critical paths
 *
 * Pitfalls:
 *   - NOT safe for multiple producers or multiple consumers
 *   - Must handle full/empty conditions correctly
 *   - Padding to avoid false sharing between head and tail
 *   - Size must be power of 2 for mask trick to work
 */

#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <cassert>
#include <cstddef>

template <typename T, size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");
    static constexpr size_t MASK = Capacity - 1;

    // Padding to avoid false sharing between producer and consumer cache lines
    alignas(64) std::atomic<size_t> head_{0}; // written by consumer
    alignas(64) std::atomic<size_t> tail_{0}; // written by producer
    alignas(64) T buffer_[Capacity];

public:
    bool push(const T& item) {
        const size_t tail = tail_.load(std::memory_order_relaxed);
        const size_t next_tail = (tail + 1) & MASK;

        // Check if full: next write position would overlap read position
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false; // queue full
        }

        buffer_[tail] = item;
        tail_.store(next_tail, std::memory_order_release); // publish
        return true;
    }

    bool pop(T& item) {
        const size_t head = head_.load(std::memory_order_relaxed);

        // Check if empty: read position equals write position
        if (head == tail_.load(std::memory_order_acquire)) {
            return false; // queue empty
        }

        item = buffer_[head];
        head_.store((head + 1) & MASK, std::memory_order_release); // consume
        return true;
    }

    size_t size() const {
        auto tail = tail_.load(std::memory_order_acquire);
        auto head = head_.load(std::memory_order_acquire);
        return (tail - head) & MASK;
    }

    bool empty() const { return size() == 0; }
};

int main() {
    constexpr size_t QUEUE_SIZE = 1024; // must be power of 2
    constexpr int NUM_MESSAGES = 100000;

    SPSCQueue<int, QUEUE_SIZE> queue;

    // Single producer thread
    std::thread producer([&]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            while (!queue.push(i)) {
                // spin until space available (backpressure)
            }
        }
    });

    // Single consumer thread
    std::vector<int> received;
    received.reserve(NUM_MESSAGES);
    std::thread consumer([&]() {
        int val;
        int count = 0;
        while (count < NUM_MESSAGES) {
            if (queue.pop(val)) {
                received.push_back(val);
                ++count;
            }
        }
    });

    producer.join();
    consumer.join();

    // Verify correctness: all messages received in order
    assert(received.size() == NUM_MESSAGES);
    for (int i = 0; i < NUM_MESSAGES; ++i) {
        assert(received[i] == i);
    }

    std::cout << "SPSC Lock-free queue: " << NUM_MESSAGES << " messages transferred correctly\n";
    std::cout << "Queue capacity: " << QUEUE_SIZE << " (power of 2 for fast modulo)\n";
    std::cout << "Ordering preserved: YES (FIFO guaranteed for SPSC)\n";

    std::cout << "\nAll lock-free queue tests passed!\n";
    return 0;
}
