# Concurrency Cheat Sheet

## Mutex vs Spinlock

| Aspect | Mutex | Spinlock |
|--------|-------|----------|
| Blocking | Sleeps (context switch) | Busy-waits (burns CPU) |
| Best for | Long critical sections | Very short critical sections (<1μs) |
| Fairness | OS-scheduled | Unfair by default |
| Use case | General-purpose | HFT hot path, interrupt handlers |

## Condition Variable Protocol

```cpp
// Producer
{
    std::lock_guard<std::mutex> lk(mtx);
    queue.push(item);
}
cv.notify_one();  // notify AFTER releasing lock (optional but good practice)

// Consumer
std::unique_lock<std::mutex> lk(mtx);
cv.wait(lk, [&]{ return !queue.empty(); });  // ALWAYS use predicate
auto item = queue.front();
queue.pop();
```

**Rules:** Always use a predicate (spurious wakeups). Always hold the mutex when checking condition.

→ `src/concurrency/mutex_condvar.cpp`, `producer_consumer.cpp`

## Atomic Operations & Memory Orderings

| Ordering | Guarantee | Use Case |
|----------|-----------|----------|
| `relaxed` | Atomicity only, no ordering | Counters, statistics |
| `acquire` | Reads after this see writes before matching release | Consumer side of queue |
| `release` | Writes before this are visible after matching acquire | Producer side of queue |
| `acq_rel` | Both acquire and release | Read-modify-write in both directions |
| `seq_cst` | Total global order (default) | When in doubt; simple correctness |

```cpp
// Producer-consumer with acquire-release
data = 42;
ready.store(true, std::memory_order_release);  // producer

if (ready.load(std::memory_order_acquire))     // consumer
    assert(data == 42);                        // guaranteed
```

→ `src/concurrency/atomics.cpp`, `memory_model.cpp`

## ABA Problem

**What:** Thread reads A, gets preempted, value changes A→B→A, CAS succeeds incorrectly.
**Fix:** Tagged pointers (pack version counter with pointer), hazard pointers, epoch-based reclamation.

## False Sharing

**What:** Two threads write to different variables on the same cache line → constant invalidation.
**Fix:**
```cpp
struct alignas(64) PaddedCounter {
    std::atomic<int> value;
};
// Each counter on its own cache line
```

→ Related: `src/concurrency/lock_free_queue.cpp`

## Thread Pool Design

Key decisions:
- Work-stealing vs shared queue (work-stealing scales better)
- Fixed vs dynamic thread count (fixed = predictable latency)
- Task type: `std::function` (general) vs template (zero-overhead)

→ `src/concurrency/thread_pool.cpp`

## Common Interview Questions

| Question | Key Answer |
|----------|------------|
| Difference between `mutex` and `atomic`? | Mutex protects regions; atomic protects single variables with hardware support |
| What causes a deadlock? | Circular wait + hold-and-wait. Fix: lock ordering, `std::scoped_lock` |
| Explain happens-before | If A happens-before B, effects of A are visible when B executes |
| Lock-free vs wait-free? | Lock-free: some thread always progresses. Wait-free: ALL threads progress in bounded steps |
| Why `compare_exchange_weak` in a loop? | Can fail spuriously on some architectures (LL/SC) |
| Reader-writer lock vs mutex? | RW lock allows concurrent reads; use when reads >> writes |
| What is a memory fence? | Forces ordering of loads/stores across the fence |
