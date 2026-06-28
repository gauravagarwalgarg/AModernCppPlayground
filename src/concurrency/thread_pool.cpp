/*
 * What: Thread pool with task queue. Workers wait on condition_variable, pop tasks.
 *       submit() returns std::future. Proper shutdown semantics.
 *
 * Why: Creating/destroying threads is expensive (~20-50us). A thread pool amortizes
 *      this cost by reusing a fixed set of worker threads. Essential for servers,
 *      HFT order management, and any high-throughput concurrent system.
 *
 * Interviewers look for:
 *   - Correct shutdown: notify all workers, join all threads
 *   - Use of std::packaged_task + std::future for result retrieval
 *   - Condition variable with predicate (avoiding spurious wakeups)
 *   - Understanding of task queue contention and when lock-free alternatives help
 *
 * Pitfalls:
 *   - Forgetting to notify_all on shutdown => workers hang forever
 *   - Submitting after shutdown => undefined behavior (must handle gracefully)
 *   - Exception in task must not crash the worker thread
 *   - std::function<void()> has allocation overhead; consider alternatives for HFT
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <future>
#include <cassert>
#include <atomic>

class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;

public:
    explicit ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx_);
                        cv_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty()) return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task(); // execute outside the lock
                }
            });
        }
    }

    // Submit a task and get a future for the result
    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {
        using ReturnType = decltype(f(args...));
        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        std::future<ReturnType> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(mtx_);
            if (stop_) throw std::runtime_error("submit on stopped pool");
            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();
        return result;
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            stop_ = true;
        }
        cv_.notify_all();
        for (auto& w : workers_) w.join();
    }
};

int main() {
    constexpr int NUM_TASKS = 20;
    ThreadPool pool(4);

    // 1. Submit tasks that return values
    std::vector<std::future<int>> futures;
    for (int i = 0; i < NUM_TASKS; ++i) {
        futures.push_back(pool.submit([](int x) { return x * x; }, i));
    }

    // Collect results
    int sum = 0;
    for (int i = 0; i < NUM_TASKS; ++i) {
        int result = futures[i].get();
        assert(result == i * i);
        sum += result;
    }
    std::cout << "Sum of squares 0.." << NUM_TASKS - 1 << " = " << sum << "\n";

    // 2. Submit void tasks with shared atomic counter
    std::atomic<int> counter{0};
    std::vector<std::future<void>> void_futures;
    for (int i = 0; i < 100; ++i) {
        void_futures.push_back(pool.submit([&counter]() {
            counter.fetch_add(1, std::memory_order_relaxed);
        }));
    }
    for (auto& f : void_futures) f.get();
    assert(counter.load() == 100);
    std::cout << "Atomic counter after 100 tasks: " << counter.load() << "\n";

    std::cout << "All thread pool tests passed!\n";
    return 0;
}
