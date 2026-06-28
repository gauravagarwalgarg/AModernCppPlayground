/*
 * What: LRU Cache with O(1) get/put using doubly linked list + unordered_map.
 *       Template<K,V>. Production-quality implementation (LeetCode 146 extended).
 *
 * Why: LRU caches are ubiquitous: CPU caches, database buffer pools, web caches.
 *      The doubly-linked-list + hashmap combination achieves O(1) for all operations.
 *      Thread-safe versions are used in HFT for caching instrument metadata.
 *
 * Interviewers look for:
 *   - O(1) get AND put (not O(n) - must use hashmap + linked list combo)
 *   - Correct eviction of least-recently-used on capacity overflow
 *   - Moving accessed nodes to front (mark as most-recently-used)
 *   - Clean separation of list manipulation from cache logic
 *
 * Pitfalls:
 *   - Forgetting to update the map when evicting => dangling pointers
 *   - Not moving node to front on get() => incorrect LRU ordering
 *   - Memory leaks if nodes aren't properly deleted on eviction
 *   - Thread-safety: naive mutex around everything destroys performance
 */

#include <iostream>
#include <unordered_map>
#include <cassert>
#include <string>

template <typename K, typename V>
class LRUCache {
    struct Node {
        K key;
        V value;
        Node* prev;
        Node* next;
        Node(const K& k, const V& v) : key(k), value(v), prev(nullptr), next(nullptr) {}
    };

    size_t capacity_;
    size_t size_ = 0;
    Node* head_ = nullptr; // most recently used
    Node* tail_ = nullptr; // least recently used
    std::unordered_map<K, Node*> map_;

    void move_to_front(Node* node) {
        if (node == head_) return;
        // Detach
        if (node->prev) node->prev->next = node->next;
        if (node->next) node->next->prev = node->prev;
        if (node == tail_) tail_ = node->prev;
        // Attach to front
        node->prev = nullptr;
        node->next = head_;
        if (head_) head_->prev = node;
        head_ = node;
        if (!tail_) tail_ = node;
    }

    void evict() {
        if (!tail_) return;
        Node* victim = tail_;
        tail_ = tail_->prev;
        if (tail_) tail_->next = nullptr;
        else head_ = nullptr;
        map_.erase(victim->key);
        delete victim;
        --size_;
    }

public:
    explicit LRUCache(size_t capacity) : capacity_(capacity) {
        assert(capacity > 0);
    }

    ~LRUCache() {
        while (head_) { Node* tmp = head_; head_ = head_->next; delete tmp; }
    }

    // Returns pointer to value or nullptr if not found
    V* get(const K& key) {
        auto it = map_.find(key);
        if (it == map_.end()) return nullptr;
        move_to_front(it->second);
        return &(it->second->value);
    }

    void put(const K& key, const V& value) {
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->value = value;
            move_to_front(it->second);
            return;
        }
        if (size_ == capacity_) evict();
        Node* node = new Node(key, value);
        node->next = head_;
        if (head_) head_->prev = node;
        head_ = node;
        if (!tail_) tail_ = node;
        map_[key] = node;
        ++size_;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
};

// Note: For thread-safe version, wrap get/put with std::shared_mutex
// (read-lock for get, write-lock for put) or use concurrent hash map + lock-free list.

int main() {
    LRUCache<int, std::string> cache(3);

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");
    assert(cache.size() == 3);

    // Access key 1 -> moves to front (most recent)
    auto* val = cache.get(1);
    assert(val && *val == "one");

    // Insert key 4 -> evicts LRU (key 2, since 1 was just accessed)
    cache.put(4, "four");
    assert(cache.get(2) == nullptr); // evicted
    assert(cache.get(3) != nullptr); // still present
    assert(cache.get(4) != nullptr); // newly added

    // Update existing key
    cache.put(1, "ONE");
    assert(*cache.get(1) == "ONE");

    // Capacity test
    LRUCache<int, int> numcache(100);
    for (int i = 0; i < 200; ++i) numcache.put(i, i * i);
    assert(numcache.size() == 100);
    // First 100 should be evicted
    assert(numcache.get(0) == nullptr);
    assert(numcache.get(99) == nullptr);
    // Last 100 should be present
    assert(numcache.get(100) != nullptr && *numcache.get(100) == 10000);
    assert(numcache.get(199) != nullptr && *numcache.get(199) == 199 * 199);

    std::cout << "LRU Cache: all operations O(1)\n";
    std::cout << "Eviction policy: least-recently-used\n";
    std::cout << "All LRU cache tests passed!\n";
    return 0;
}
