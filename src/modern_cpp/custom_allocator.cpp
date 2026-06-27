/*
 * Custom Allocator: Arena/Pool Allocator for Containers
 *
 * WHAT: Custom allocators replace the default heap allocator (new/delete) for
 * STL containers. Arena allocators pre-allocate a buffer and hand out chunks
 * without system calls deterministic O(1) allocation.
 *
 * WHY IT MATTERS IN INTERVIEWS: HFT interview must-know. In hot paths, malloc
 * is unpredictable (system calls, fragmentation, locks). Arena allocators give:
 * - Deterministic latency (no syscalls)
 * - Cache-friendly allocation (contiguous memory)
 * - Bulk deallocation (reset entire arena)
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding allocator requirements (allocate, deallocate, value_type)
 * - Monotonic/arena allocator (bump pointer, no individual free)
 * - Pool allocator (fixed-size blocks, free list)
 * - Using custom allocator with std::vector, std::list
 * - Memory alignment considerations
 *
 * COMMON PITFALLS:
 * - Allocator state and container move semantics
 * - Forgetting alignment requirements
 * - Arena exhaustion without fallback
 * - Not making allocator stateless or propagating correctly
 */

#include <iostream>
#include <vector>
#include <list>
#include <cassert>
#include <cstddef>
#include <memory>
#include <chrono>
#include <array>
#include <cstdint>

// === 1. Monotonic Arena Allocator ===
class Arena {
    std::unique_ptr<std::byte[]> buffer_;
    size_t capacity_;
    size_t offset_ = 0;
    size_t alloc_count_ = 0;

public:
    explicit Arena(size_t bytes) : buffer_(std::make_unique<std::byte[]>(bytes)),
                                   capacity_(bytes) {}

    void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t)) {
        size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned + bytes > capacity_) {
            throw std::bad_alloc();
        }
        void* ptr = buffer_.get() + aligned;
        offset_ = aligned + bytes;
        ++alloc_count_;
        return ptr;
    }

    void reset() { offset_ = 0; alloc_count_ = 0; }
    size_t used() const { return offset_; }
    size_t remaining() const { return capacity_ - offset_; }
    size_t allocCount() const { return alloc_count_; }
};

// === 2. STL-compatible Allocator wrapping Arena ===
template <typename T>
class ArenaAllocator {
    Arena* arena_;
public:
    using value_type = T;

    explicit ArenaAllocator(Arena& arena) noexcept : arena_(&arena) {}

    template <typename U>
    ArenaAllocator(const ArenaAllocator<U>& other) noexcept : arena_(other.arena()) {}

    T* allocate(size_t n) {
        return static_cast<T*>(arena_->allocate(n * sizeof(T), alignof(T)));
    }

    void deallocate(T*, size_t) noexcept {
        // Arena allocator: no-op! Memory freed when arena is reset.
    }

    Arena* arena() const noexcept { return arena_; }

    template <typename U>
    bool operator==(const ArenaAllocator<U>& other) const noexcept {
        return arena_ == other.arena();
    }

    template <typename U>
    bool operator!=(const ArenaAllocator<U>& other) const noexcept {
        return !(*this == other);
    }
};

// === 3. Simple Pool Allocator (fixed-size blocks with free list) ===
template <size_t BlockSize, size_t BlockCount>
class PoolAllocator {
    struct Block { Block* next; };
    std::array<std::byte, BlockSize * BlockCount> storage_;
    Block* freeList_ = nullptr;
    size_t allocated_ = 0;

public:
    PoolAllocator() {
        for (size_t i = 0; i < BlockCount; ++i) {
            auto* block = reinterpret_cast<Block*>(&storage_[i * BlockSize]);
            block->next = freeList_;
            freeList_ = block;
        }
    }

    void* allocate() {
        if (!freeList_) throw std::bad_alloc();
        Block* block = freeList_;
        freeList_ = block->next;
        ++allocated_;
        return block;
    }

    void deallocate(void* ptr) {
        auto* block = static_cast<Block*>(ptr);
        block->next = freeList_;
        freeList_ = block;
        --allocated_;
    }

    size_t allocated() const { return allocated_; }
    size_t available() const { return BlockCount - allocated_; }
};

int main() {
    std::cout << "=== 1. Arena Allocator with std::vector ===\n";
    Arena arena(4096);
    {
        using ArenaVec = std::vector<int, ArenaAllocator<int>>;
        ArenaAllocator<int> alloc(arena);
        ArenaVec vec(alloc);

        vec.reserve(100);
        for (int i = 0; i < 100; ++i) vec.push_back(i);

        assert(vec.size() == 100);
        assert(vec[50] == 50);
        std::cout << "  Vector with 100 ints, arena used: " << arena.used() << " bytes\n";
        std::cout << "  Arena allocations: " << arena.allocCount() << "\n";
    }

    std::cout << "\n=== 2. Arena reset (bulk free) ===\n";
    std::cout << "  Before reset: " << arena.used() << " bytes used\n";
    arena.reset();
    std::cout << "  After reset: " << arena.used() << " bytes used (instant free!)\n";
    assert(arena.used() == 0);

    std::cout << "\n=== 3. Pool Allocator (fixed-size blocks) ===\n";
    PoolAllocator<64, 100> pool;
    std::vector<void*> ptrs;
    for (int i = 0; i < 10; ++i) {
        ptrs.push_back(pool.allocate());
    }
    assert(pool.allocated() == 10);
    assert(pool.available() == 90);
    for (void* p : ptrs) pool.deallocate(p);
    assert(pool.allocated() == 0);
    std::cout << "  Allocated and freed 10 blocks from pool\n";

    std::cout << "\n=== 4. Performance: Arena vs Default allocator ===\n";
    constexpr int N = 100000;
    auto t1 = std::chrono::high_resolution_clock::now();
    {
        std::vector<int> defaultVec;
        for (int i = 0; i < N; ++i) defaultVec.push_back(i);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    {
        Arena perfArena(N * sizeof(int) * 2);
        ArenaAllocator<int> alloc(perfArena);
        std::vector<int, ArenaAllocator<int>> arenaVec(alloc);
        arenaVec.reserve(N);
        for (int i = 0; i < N; ++i) arenaVec.push_back(i);
    }
    auto t3 = std::chrono::high_resolution_clock::now();

    auto default_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto arena_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "  Default allocator: " << default_us << " us\n";
    std::cout << "  Arena allocator:   " << arena_us << " us\n";

    std::cout << "\n=== HFT Relevance ===\n";
    std::cout << "  - No malloc/free in hot path (no system calls)\n";
    std::cout << "  - Deterministic latency (bounded allocation time)\n";
    std::cout << "  - Cache-friendly (contiguous memory)\n";
    std::cout << "  - Bulk deallocation between messages/requests\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
