// singleton.cpp - Meyer's Singleton Pattern
// Interview Relevance: One of the most frequently asked design pattern questions.
// Demonstrates thread-safe lazy initialization (guaranteed by C++11 §6.7),
// deleted copy/move semantics, and when NOT to use singletons (testing, tight coupling).
// Compile: g++ -std=c++20 -Wall singleton.cpp -o singleton

#include <iostream>
#include <cassert>
#include <string>

class Logger {
public:
    static Logger& instance() {
        static Logger inst;  // Thread-safe since C++11 (magic statics)
        return inst;
    }

    // Delete copy and move to prevent multiple instances
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void log(const std::string& msg) {
        std::cout << "[LOG #" << ++count_ << "] " << msg << "\n";
    }

    int getCount() const { return count_; }

private:
    Logger() : count_(0) {
        std::cout << "Logger constructed (once only)\n";
    }
    ~Logger() { std::cout << "Logger destroyed\n"; }

    int count_;
};

int main() {
    // Both references point to the same instance
    Logger& a = Logger::instance();
    Logger& b = Logger::instance();
    assert(&a == &b);  // Same address guaranteed

    a.log("First message");
    b.log("Second message");
    assert(a.getCount() == 2);

    std::cout << "\n--- Why Meyer's Singleton is preferred ---\n";
    std::cout << "1. Thread-safe without explicit locks (C++11 guarantee)\n";
    std::cout << "2. Lazy initialization - created on first use\n";
    std::cout << "3. Destruction order is well-defined (reverse of construction)\n";

    std::cout << "\n--- When NOT to use Singleton ---\n";
    std::cout << "1. Makes unit testing hard (global state, can't mock easily)\n";
    std::cout << "2. Hides dependencies (not explicit in constructor)\n";
    std::cout << "3. Violates SRP (manages own lifetime + business logic)\n";
    std::cout << "4. Prefer dependency injection for testable code\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
