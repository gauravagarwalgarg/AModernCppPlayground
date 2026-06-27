/*
 * What: Fixed-size object pool allocator. Pre-allocates N blocks. O(1) allocate
 *       and deallocate via free list. No system calls in steady state.
 *
 * Why: In HFT, malloc/new can take 100ns+ and trigger page faults. A memory pool
 *      pre-allocates memory at startup, then hands out fixed-size blocks in O(1)
 *      using a free list. This eliminates allocator jitter on the hot path.
 *
 * Interviewers look for:
 *   - Free list implementation using intrusive linked list (reuse freed memory)
 *   - O(1) allocate (pop from free list) and deallocate (push to free list)
 *   - Proper alignment handling for the stored type
 *   - Understanding of why this matters: deterministic latency, no fragmentation
 *
 * Pitfalls:
 *   - Double-free: returning same block twice corrupts the free list
 *   - Returning memory not from this pool => undefined behavior
 *   - Alignment: blocks must be aligned for the target type
 *   - Pool exhaustion: must handle gracefully (return nullptr or throw)
 */

#include <iostream>
#include <vector>
#include <cassert>
#include <cstddef>
#include <new>

template <typename T, size_t PoolSize = 1024>
class MemoryPool {
    // Free list node: stored in-place within unused blocks
    union Block {
        Block* next;                           // when free: pointer to next free block
        alignas(T) char storage[sizeof(T)];    // when allocated: storage for T
    };

    Block* pool_ = nullptr;     // raw memory
    Block* free_list_ = nullptr; // head of free list
    size_t allocated_ = 0;

public:
    MemoryPool() {
        // Pre-allocate all blocks upfront (single allocation)
        pool_ = static_cast<Block*>(::operator new(sizeof(Block) * PoolSize));

        // Initialize free list: chain all blocks together
        free_list_ = &pool_[0];
        for (size_t i = 0; i < PoolSize - 1; ++i) {
            pool_[i].next = &pool_[i + 1];
        }
        pool_[PoolSize - 1].next = nullptr;
    }

    ~MemoryPool() {
        ::operator delete(pool_);
    }

    // Non-copyable, non-movable
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool& operator=(const MemoryPool&) = delete;

    // O(1) allocate: pop from free list, construct T in-place
    template <typename... Args>
    T* allocate(Args&&... args) {
        if (!free_list_) return nullptr; // pool exhausted

        Block* block = free_list_;
        free_list_ = free_list_->next;
        ++allocated_;

        // Placement new: construct T in pre-allocated storage
        return new (block->storage) T(std::forward<Args>(args)...);
    }

    // O(1) deallocate: destroy T, push block back to free list
    void deallocate(T* ptr) {
        if (!ptr) return;

        ptr->~T(); // explicit destructor call

        Block* block = reinterpret_cast<Block*>(ptr);
        block->next = free_list_;
        free_list_ = block;
        --allocated_;
    }

    size_t allocated() const { return allocated_; }
    size_t available() const { return PoolSize - allocated_; }
    static constexpr size_t capacity() { return PoolSize; }
};

// Test struct to verify construction/destruction
struct Order {
    int id;
    double price;
    int quantity;
    static int construct_count;
    static int destruct_count;

    Order(int i, double p, int q) : id(i), price(p), quantity(q) { ++construct_count; }
    ~Order() { ++destruct_count; }
};
int Order::construct_count = 0;
int Order::destruct_count = 0;

int main() {
    MemoryPool<Order, 64> pool;

    // Allocate objects
    std::vector<Order*> orders;
    for (int i = 0; i < 50; ++i) {
        Order* o = pool.allocate(i, 100.0 + i, i * 10);
        assert(o != nullptr);
        orders.push_back(o);
    }
    assert(pool.allocated() == 50);
    assert(pool.available() == 14);

    // Verify objects are correctly constructed
    assert(orders[0]->id == 0 && orders[49]->id == 49);
    assert(Order::construct_count == 50);

    // Deallocate some
    for (int i = 0; i < 30; ++i) {
        pool.deallocate(orders[i]);
    }
    assert(pool.allocated() == 20);
    assert(Order::destruct_count == 30);

    // Reallocate: reuses freed blocks (O(1), no syscall)
    for (int i = 0; i < 30; ++i) {
        Order* o = pool.allocate(100 + i, 200.0, 1);
        assert(o != nullptr);
    }
    assert(pool.allocated() == 50);

    std::cout << "Memory pool capacity: " << pool.capacity() << "\n";
    std::cout << "Allocated: " << pool.allocated() << "\n";
    std::cout << "Available: " << pool.available() << "\n";
    std::cout << "Constructions: " << Order::construct_count << "\n";
    std::cout << "Destructions: " << Order::destruct_count << "\n";
    std::cout << "All memory pool tests passed!\n";
    return 0;
}
