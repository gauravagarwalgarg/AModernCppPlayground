/*
 * What: std::thread creation, join, detach. std::jthread (C++20). Thread arguments
 *       (copy, ref, move). Thread-local storage. Lambda threads.
 *
 * Why: Threads are the fundamental unit of concurrency. Understanding ownership,
 *      argument passing semantics, and RAII-based jthread is essential for writing
 *      correct concurrent code in modern C++.
 *
 * Interviewers look for:
 *   - Knowledge of join vs detach semantics and when each is appropriate
 *   - Understanding that arguments are copied by default (std::ref for references)
 *   - Awareness of std::jthread's cooperative cancellation and auto-join
 *   - Proper use of thread_local for per-thread state
 *
 * Pitfalls:
 *   - Forgetting to join/detach causes std::terminate (jthread fixes this)
 *   - Passing references without std::ref leads to copies, not actual references
 *   - Detached threads accessing destroyed stack variables = undefined behavior
 *   - thread_local has construction/destruction overhead per thread
 */

#include <iostream>
#include <thread>
#include <string>
#include <cassert>
#include <stop_token>

// Thread-local storage: each thread gets its own copy
thread_local int tls_counter = 0;

void increment_tls(int id, int times) {
    for (int i = 0; i < times; ++i) ++tls_counter;
    std::cout << "Thread " << id << " tls_counter = " << tls_counter << "\n";
}

// Demonstrate argument passing
void by_value(int x) { ++x; std::cout << "by_value: x=" << x << "\n"; }
void by_ref(int& x) { ++x; std::cout << "by_ref: x=" << x << "\n"; }
void by_move(std::string s) { std::cout << "by_move: s=" << s << "\n"; }

// jthread with stop_token (C++20 cooperative cancellation)
void cancellable_work(std::stop_token stoken, int id) {
    int count = 0;
    while (!stoken.stop_requested() && count < 5) {
        ++count;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "jthread " << id << " did " << count << " iterations\n";
}

int main() {
    // 1. Basic thread with lambda
    int result = 0;
    std::thread t1([&result]() { result = 42; });
    t1.join();
    assert(result == 42);
    std::cout << "Lambda thread set result = " << result << "\n";

    // 2. Thread arguments: copy vs ref vs move
    int val = 10;
    std::thread t2(by_value, val);
    t2.join();
    assert(val == 10); // unchanged - was copied

    std::thread t3(by_ref, std::ref(val)); // must use std::ref!
    t3.join();
    assert(val == 11); // modified through reference

    std::string msg = "hello";
    std::thread t4(by_move, std::move(msg));
    t4.join();
    assert(msg.empty()); // moved-from state

    // 3. Thread-local storage
    std::thread t5(increment_tls, 1, 3);
    std::thread t6(increment_tls, 2, 5);
    t5.join();
    t6.join();
    assert(tls_counter == 0); // main thread's copy is untouched

    // 4. std::jthread - auto-joins on destruction, supports stop_token
    {
        std::jthread jt(cancellable_work, 1);
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        jt.request_stop(); // cooperative cancellation
    } // auto-joins here, no explicit join needed

    std::cout << "All thread tests passed!\n";
    return 0;
}
