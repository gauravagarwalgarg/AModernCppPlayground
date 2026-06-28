/**************************************************************************************************
 * Topic: Proxy Pattern Control Access to an Object
 *
 * Interview Relevance: ★★★★
 * What interviewers look for:
 *   - Same interface as the real subject (Liskov substitution)
 *   - Types: virtual proxy (lazy), protection proxy (auth), logging proxy, caching proxy
 *   - Modern C++: smart pointers as built-in proxy (shared_ptr = reference-counting proxy)
 *   - Real-world: lazy DB connections, API rate limiting, smart references
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o proxy proxy.cpp
 **************************************************************************************************/

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

// ============================================================================
// Subject interface
// ============================================================================

class DataService {
public:
    virtual ~DataService() = default;
    virtual std::string fetch(std::string const& key) = 0;
    virtual int request_count() const = 0;
};

// ============================================================================
// Real Subject expensive operation (simulates DB/network call)
// ============================================================================

class RemoteDataService : public DataService {
public:
    std::string fetch(std::string const& key) override {
        ++requests_;
        // Simulate expensive operation
        return "data_for_" + key;
    }

    int request_count() const override { return requests_; }

private:
    int requests_{0};
};

// ============================================================================
// Caching Proxy adds caching without modifying the real service
// ============================================================================

class CachingProxy : public DataService {
public:
    explicit CachingProxy(std::unique_ptr<DataService> real_service)
        : real_(std::move(real_service)) {}

    std::string fetch(std::string const& key) override {
        auto it = cache_.find(key);
        if (it != cache_.end()) {
            ++cache_hits_;
            return it->second;
        }
        auto result = real_->fetch(key);
        cache_[key] = result;
        return result;
    }

    int request_count() const override { return real_->request_count(); }
    int cache_hit_count() const { return cache_hits_; }

private:
    std::unique_ptr<DataService> real_;
    std::unordered_map<std::string, std::string> cache_;
    int cache_hits_{0};
};

// ============================================================================
// Logging Proxy adds logging around calls
// ============================================================================

class LoggingProxy : public DataService {
public:
    explicit LoggingProxy(std::unique_ptr<DataService> real_service)
        : real_(std::move(real_service)) {}

    std::string fetch(std::string const& key) override {
        std::cout << "  [LOG] fetch(\"" << key << "\") called\n";
        auto result = real_->fetch(key);
        std::cout << "  [LOG] fetch(\"" << key << "\") returned: " << result << '\n';
        return result;
    }

    int request_count() const override { return real_->request_count(); }

private:
    std::unique_ptr<DataService> real_;
};

// ============================================================================
// Client code works with any DataService (real or proxy)
// ============================================================================

void process(DataService& service, std::string const& key) {
    auto data = service.fetch(key);
    std::cout << "  Got: " << data << '\n';
}

int main() {
    std::cout << "=== Proxy Pattern ===\n\n";

    // Stack proxies: Logging → Caching → Real
    auto real = std::make_unique<RemoteDataService>();
    auto cached = std::make_unique<CachingProxy>(std::move(real));
    auto* cache_ptr = cached.get();  // Keep handle for assertions
    LoggingProxy service(std::move(cached));

    std::cout << "First request (cache miss → hits real service):\n";
    process(service, "user_123");

    std::cout << "\nSecond request same key (cache hit → no real service call):\n";
    process(service, "user_123");

    std::cout << "\nThird request different key (cache miss):\n";
    process(service, "order_456");

    // Assertions
    assert(service.request_count() == 2);     // Only 2 real requests
    assert(cache_ptr->cache_hit_count() == 1); // 1 cache hit

    std::cout << "\nReal service calls: " << service.request_count()
              << " | Cache hits: " << cache_ptr->cache_hit_count() << '\n';
    std::cout << "All assertions passed.\n";
    return EXIT_SUCCESS;
}
