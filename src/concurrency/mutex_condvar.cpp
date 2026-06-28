/*
 * What: std::mutex, lock_guard, unique_lock, scoped_lock (C++17).
 *       std::condition_variable for producer-consumer. Deadlock avoidance.
 *
 * Why: Mutexes protect shared state; condition variables enable threads to wait
 *      for state changes without busy-waiting. scoped_lock prevents deadlocks
 *      when locking multiple mutexes simultaneously.
 *
 * Interviewers look for:
 *   - RAII lock management (never raw lock/unlock)
 *   - Why unique_lock is needed with condition_variable (must be unlockable)
 *   - Spurious wakeup handling (predicate in wait)
 *   - Deadlock avoidance strategies (lock ordering, scoped_lock)
 *
 * Pitfalls:
 *   - condition_variable::wait without predicate => spurious wakeup bugs
 *   - lock_guard can't be used with condition_variable (not unlockable)
 *   - Forgetting to notify under the lock can cause lost wakeups
 *   - Recursive locking on std::mutex => undefined behavior (use recursive_mutex)
 */

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <cassert>
#include <vector>

// --- Producer-Consumer with condition_variable ---
std::mutex mtx;
std::condition_variable cv;
std::queue<int> buffer;
constexpr int NUM_ITEMS = 10;
bool done = false;

void producer() {
    for (int i = 0; i < NUM_ITEMS; ++i) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            buffer.push(i);
            std::cout << "Produced: " << i << "\n";
        }
        cv.notify_one(); // notify AFTER releasing lock for efficiency
    }
    {
        std::lock_guard<std::mutex> lock(mtx);
        done = true;
    }
    cv.notify_all();
}

void consumer(int id, std::vector<int>& consumed) {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx); // unique_lock required for cv
        cv.wait(lock, [] { return !buffer.empty() || done; }); // predicate prevents spurious wakeup

        while (!buffer.empty()) {
            int val = buffer.front();
            buffer.pop();
            consumed.push_back(val);
            std::cout << "Consumer " << id << " got: " << val << "\n";
        }
        if (done && buffer.empty()) break;
    }
}

// --- Deadlock avoidance with scoped_lock ---
struct Account {
    std::mutex mtx;
    int balance;
    Account(int b) : balance(b) {}
};

void transfer(Account& from, Account& to, int amount) {
    // scoped_lock locks both mutexes without deadlock (uses std::lock internally)
    std::scoped_lock lock(from.mtx, to.mtx);
    if (from.balance >= amount) {
        from.balance -= amount;
        to.balance += amount;
    }
}

int main() {
    // 1. Producer-Consumer
    std::vector<int> consumed;
    std::thread prod(producer);
    std::thread cons(consumer, 1, std::ref(consumed));
    prod.join();
    cons.join();
    assert(consumed.size() == NUM_ITEMS);
    std::cout << "Producer-consumer: all " << NUM_ITEMS << " items consumed.\n\n";

    // 2. Deadlock-free transfer with scoped_lock
    Account a(1000), b(500);
    std::thread t1([&]() { for (int i = 0; i < 100; ++i) transfer(a, b, 5); });
    std::thread t2([&]() { for (int i = 0; i < 100; ++i) transfer(b, a, 3); });
    t1.join();
    t2.join();
    // Total money is conserved
    assert(a.balance + b.balance == 1500);
    std::cout << "After transfers: A=" << a.balance << " B=" << b.balance
              << " Total=" << a.balance + b.balance << "\n";

    std::cout << "All mutex/condvar tests passed!\n";
    return 0;
}
