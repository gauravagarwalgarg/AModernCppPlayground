/*
 * What: Thread-safe queue with mutex + condition_variable. push(), pop() (blocking),
 *       try_pop() (non-blocking). Foundation for thread pools and message passing.
 *
 * Why: A thread-safe queue decouples producers from consumers safely. It's the
 *      building block for thread pools (task queue), logging systems (log queue),
 *      and actor-model message passing in concurrent applications.
 *
 * Interviewers look for:
 *   - Correct locking granularity (lock only what's needed)
 *   - Blocking pop with condition_variable (efficient waiting)
 *   - Non-blocking try_pop for latency-sensitive consumers
 *   - Clean shutdown mechanism to unblock waiting threads
 *
 * Pitfalls:
 *   - Holding the lock while notifying (correct but suboptimal)
 *   - Missing predicate in wait() => spurious wakeup bugs
 *   - No shutdown mechanism => threads block forever on destruction
 *   - Lock contention under high throughput (consider lock-free for HFT)
 */

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <optional>
#include <atomic>
#include <cassert>

template <typename T>
class ThreadSafeQueue {
    std::queue<T> queue_;
    mutable std::mutex mtx_;
    std::condition_variable cv_;
    bool closed_ = false;

public:
    void push(T item) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (closed_) throw std::runtime_error("push on closed queue");
            queue_.push(std::move(item));
        }
        cv_.notify_one();
    }

    // Blocking pop: waits until item available or queue closed
    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty() || closed_; });
        if (queue_.empty()) return std::nullopt; // closed and empty
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    // Non-blocking pop: returns immediately
    std::optional<T> try_pop() {
        std::lock_guard<std::mutex> lock(mtx_);
        if (queue_.empty()) return std::nullopt;
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    void close() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            closed_ = true;
        }
        cv_.notify_all(); // unblock all waiting consumers
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx_);
        return queue_.empty();
    }
};

int main() {
    ThreadSafeQueue<int> queue;
    constexpr int NUM_ITEMS = 1000;
    constexpr int NUM_PRODUCERS = 4;
    constexpr int NUM_CONSUMERS = 2;
    std::atomic<int> consumed{0};

    // Producers
    std::vector<std::thread> producers;
    for (int p = 0; p < NUM_PRODUCERS; ++p) {
        producers.emplace_back([&, p]() {
            for (int i = 0; i < NUM_ITEMS; ++i) {
                queue.push(p * NUM_ITEMS + i);
            }
        });
    }

    // Consumers using blocking pop
    std::vector<std::thread> consumers;
    for (int c = 0; c < NUM_CONSUMERS; ++c) {
        consumers.emplace_back([&]() {
            while (true) {
                auto item = queue.pop();
                if (!item.has_value()) break; // queue closed and empty
                consumed.fetch_add(1, std::memory_order_relaxed);
            }
        });
    }

    // Wait for producers to finish, then close queue
    for (auto& p : producers) p.join();
    queue.close(); // signal consumers to stop
    for (auto& c : consumers) c.join();

    assert(consumed.load() == NUM_PRODUCERS * NUM_ITEMS);
    std::cout << "Produced: " << NUM_PRODUCERS * NUM_ITEMS << "\n";
    std::cout << "Consumed: " << consumed.load() << "\n";

    // Test try_pop (non-blocking)
    ThreadSafeQueue<std::string> sq;
    sq.push("hello");
    auto val = sq.try_pop();
    assert(val.has_value() && val.value() == "hello");
    assert(!sq.try_pop().has_value()); // empty

    std::cout << "All thread-safe queue tests passed!\n";
    return 0;
}
