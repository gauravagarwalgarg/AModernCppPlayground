/*
 * What: Bounded buffer with mutex+condvar. Multiple producers, multiple consumers.
 *       Classic synchronization pattern demonstrating correct coordination.
 *
 * Why: The bounded buffer (blocking queue) is the foundation of message-passing
 *      systems. It decouples producers from consumers, handles backpressure
 *      naturally, and is used in thread pools, logging, and event systems.
 *
 * Interviewers look for:
 *   - Correct use of TWO condition variables (not_full, not_empty)
 *   - Proper predicate to handle spurious wakeups
 *   - Understanding of why a single cv for both conditions is problematic
 *   - Clean shutdown signaling to unblock waiting threads
 *
 * Pitfalls:
 *   - Using one condition_variable for both conditions => missed wakeups
 *   - notify_one vs notify_all: notify_one suffices for single-item changes
 *   - Forgetting to check buffer state AFTER wakeup (spurious wakeup)
 *   - Shutdown: must unblock all waiting threads to allow clean exit
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <atomic>
#include <cassert>

template <typename T>
class BoundedBuffer {
    std::queue<T> buffer_;
    size_t capacity_;
    std::mutex mtx_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;

public:
    explicit BoundedBuffer(size_t cap) : capacity_(cap) {}

    void produce(const T& item) {
        std::unique_lock<std::mutex> lock(mtx_);
        not_full_.wait(lock, [this] { return buffer_.size() < capacity_; });
        buffer_.push(item);
        not_empty_.notify_one();
    }

    T consume() {
        std::unique_lock<std::mutex> lock(mtx_);
        not_empty_.wait(lock, [this] { return !buffer_.empty(); });
        T item = buffer_.front();
        buffer_.pop();
        not_full_.notify_one();
        return item;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx_);
        return buffer_.size();
    }
};

int main() {
    constexpr int BUFFER_SIZE = 5;
    constexpr int NUM_PRODUCERS = 3;
    constexpr int NUM_CONSUMERS = 2;
    constexpr int ITEMS_PER_PRODUCER = 100;
    constexpr int TOTAL_ITEMS = NUM_PRODUCERS * ITEMS_PER_PRODUCER;

    BoundedBuffer<int> buffer(BUFFER_SIZE);
    std::atomic<int> produced_count{0};
    std::atomic<int> consumed_count{0};

    // Multiple producers
    std::vector<std::thread> producers;
    for (int p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
                buffer.produce(p * 1000 + i);
                produced_count.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // Multiple consumers
    std::vector<std::thread> consumers;
    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&]() {
            while (consumed_count.load(std::memory_order_relaxed) < TOTAL_ITEMS) {
                if (consumed_count.fetch_add(1, std::memory_order_acq_rel) < TOTAL_ITEMS) {
                    buffer.consume();
                } else {
                    consumed_count.fetch_sub(1, std::memory_order_relaxed);
                    break;
                }
            }
        });
    }

    for (auto& p : producers) p.join();
    for (auto& c : consumers) c.join();

    assert(produced_count.load() == TOTAL_ITEMS);
    assert(consumed_count.load() == TOTAL_ITEMS);

    std::cout << "Bounded buffer capacity: " << BUFFER_SIZE << "\n";
    std::cout << "Producers: " << NUM_PRODUCERS << " x " << ITEMS_PER_PRODUCER << " items\n";
    std::cout << "Consumers: " << NUM_CONSUMERS << "\n";
    std::cout << "Total produced: " << produced_count.load() << "\n";
    std::cout << "Total consumed: " << consumed_count.load() << "\n";

    std::cout << "All producer-consumer tests passed!\n";
    return 0;
}
