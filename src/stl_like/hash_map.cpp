/*
 * What: Separate-chaining hash map. Template<K,V>. insert, find, erase,
 *       operator[]. Load factor tracking and automatic rehashing.
 *
 * Why: Hash maps are the most-asked data structure in interviews. Understanding
 *      the internals (hashing, collision resolution, rehashing) is essential.
 *      Demonstrates template design, memory management, and amortized O(1) ops.
 *
 * Interviewers look for:
 *   - Correct hash function usage and bucket index calculation
 *   - Collision handling strategy (chaining vs open addressing tradeoffs)
 *   - Rehashing: when to trigger, how to redistribute elements
 *   - operator[] inserting default value if key not found (like std::unordered_map)
 *
 * Pitfalls:
 *   - Forgetting to rehash leads to O(n) lookup as load factor grows
 *   - Hash collisions: all keys in one bucket => linked list traversal
 *   - Iterator invalidation on rehash
 *   - Non-trivial key types need proper hash and equality
 */

#include <iostream>
#include <vector>
#include <list>
#include <functional>
#include <cassert>
#include <string>
#include <optional>

template <typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
    struct Entry {
        K key;
        V value;
        Entry(const K& k, const V& v) : key(k), value(v) {}
    };

    std::vector<std::list<Entry>> buckets_;
    size_t size_ = 0;
    float max_load_factor_ = 0.75f;
    Hash hasher_;

    size_t bucket_index(const K& key) const {
        return hasher_(key) % buckets_.size();
    }

    void rehash(size_t new_bucket_count) {
        std::vector<std::list<Entry>> new_buckets(new_bucket_count);
        for (auto& bucket : buckets_) {
            for (auto& entry : bucket) {
                size_t idx = hasher_(entry.key) % new_bucket_count;
                new_buckets[idx].emplace_back(entry.key, entry.value);
            }
        }
        buckets_ = std::move(new_buckets);
    }

public:
    explicit HashMap(size_t initial_buckets = 16)
        : buckets_(initial_buckets) {}

    void insert(const K& key, const V& value) {
        size_t idx = bucket_index(key);
        for (auto& entry : buckets_[idx]) {
            if (entry.key == key) { entry.value = value; return; } // update
        }
        buckets_[idx].emplace_back(key, value);
        ++size_;
        if (load_factor() > max_load_factor_) rehash(buckets_.size() * 2);
    }

    std::optional<V> find(const K& key) const {
        size_t idx = bucket_index(key);
        for (const auto& entry : buckets_[idx]) {
            if (entry.key == key) return entry.value;
        }
        return std::nullopt;
    }

    bool erase(const K& key) {
        size_t idx = bucket_index(key);
        auto& bucket = buckets_[idx];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) {
            if (it->key == key) {
                bucket.erase(it);
                --size_;
                return true;
            }
        }
        return false;
    }

    V& operator[](const K& key) {
        size_t idx = bucket_index(key);
        for (auto& entry : buckets_[idx]) {
            if (entry.key == key) return entry.value;
        }
        // Insert default value
        buckets_[idx].emplace_back(key, V{});
        ++size_;
        if (load_factor() > max_load_factor_) {
            rehash(buckets_.size() * 2);
            idx = bucket_index(key);
            for (auto& entry : buckets_[idx]) {
                if (entry.key == key) return entry.value;
            }
        }
        return buckets_[idx].back().value;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    float load_factor() const { return static_cast<float>(size_) / buckets_.size(); }
    size_t bucket_count() const { return buckets_.size(); }
};

int main() {
    HashMap<std::string, int> map;

    // Insert
    map.insert("one", 1);
    map.insert("two", 2);
    map.insert("three", 3);
    assert(map.size() == 3);

    // Find
    auto val = map.find("two");
    assert(val.has_value() && val.value() == 2);
    assert(!map.find("four").has_value());

    // operator[] - access and insert
    map["four"] = 4;
    assert(map.find("four").value() == 4);
    map["one"] = 100; // update via operator[]
    assert(map.find("one").value() == 100);

    // Erase
    assert(map.erase("three"));
    assert(!map.find("three").has_value());
    assert(map.size() == 3);

    // Rehashing test: insert many elements
    HashMap<int, int> intmap;
    for (int i = 0; i < 1000; ++i) intmap.insert(i, i * i);
    assert(intmap.size() == 1000);
    for (int i = 0; i < 1000; ++i) {
        assert(intmap.find(i).value() == i * i);
    }
    std::cout << "After 1000 inserts: buckets=" << intmap.bucket_count()
              << " load_factor=" << intmap.load_factor() << "\n";

    std::cout << "All hash map tests passed!\n";
    return 0;
}
