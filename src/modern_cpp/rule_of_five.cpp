/*
 * Rule of Five (and Rule of Zero / Rule of Three)
 *
 * WHAT: If a class manages a resource, you must define or delete: destructor,
 * copy constructor, copy assignment, move constructor, move assignment.
 * Rule of Zero: prefer classes that need none (use smart pointers/RAII members).
 *
 * WHY IT MATTERS IN INTERVIEWS: Classic interview question: "When do you need
 * a custom destructor?" If you define one, you likely need all five. Shows
 * understanding of object lifecycle and ownership semantics.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Rule of Zero as default preference (compiler-generated are correct)
 * - When you MUST implement (raw resource ownership)
 * - Proper implementation of all 5 special members
 * - Self-assignment protection
 * - noexcept on move operations
 * - = default and = delete usage
 *
 * COMMON PITFALLS:
 * - Defining destructor but forgetting copy/move (Rule of 3/5 violation)
 * - Missing self-assignment check in copy assignment
 * - Not marking move operations noexcept
 * - Shallow copy of raw pointers (double-free)
 */

#include <iostream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include <memory>
#include <string>

// === Rule of Five: Resource-managing class ===
class Buffer {
    size_t size_;
    int* data_;  // Raw resource requires Rule of Five

public:
    // Constructor
    explicit Buffer(size_t size) : size_(size), data_(new int[size]) {
        std::fill(data_, data_ + size_, 0);
        std::cout << "  [Ctor] Buffer(" << size_ << ") allocated\n";
    }

    // 1. Destructor
    ~Buffer() {
        delete[] data_;
        std::cout << "  [Dtor] Buffer(" << size_ << ") freed\n";
    }

    // 2. Copy Constructor (deep copy)
    Buffer(const Buffer& other) : size_(other.size_), data_(new int[other.size_]) {
        std::copy(other.data_, other.data_ + size_, data_);
        std::cout << "  [Copy Ctor] Buffer(" << size_ << ") deep copied\n";
    }

    // 3. Copy Assignment (copy-and-swap idiom)
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {  // Self-assignment protection
            Buffer tmp(other);  // Copy
            std::swap(size_, tmp.size_);
            std::swap(data_, tmp.data_);  // Swap (old data freed in tmp's dtor)
        }
        std::cout << "  [Copy=] Buffer(" << size_ << ") copy assigned\n";
        return *this;
    }

    // 4. Move Constructor (steal resources)
    Buffer(Buffer&& other) noexcept : size_(other.size_), data_(other.data_) {
        other.size_ = 0;
        other.data_ = nullptr;  // Leave in valid, destructible state
        std::cout << "  [Move Ctor] Buffer(" << size_ << ") moved\n";
    }

    // 5. Move Assignment
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;     // Release current resource
            size_ = other.size_;
            data_ = other.data_;
            other.size_ = 0;
            other.data_ = nullptr;
        }
        std::cout << "  [Move=] Buffer(" << size_ << ") move assigned\n";
        return *this;
    }

    // Accessors
    size_t size() const { return size_; }
    int& operator[](size_t idx) { return data_[idx]; }
    const int& operator[](size_t idx) const { return data_[idx]; }
};

// === Rule of Zero: let RAII members handle everything ===
class ModernBuffer {
    std::unique_ptr<int[]> data_;
    size_t size_;
public:
    explicit ModernBuffer(size_t size)
        : data_(std::make_unique<int[]>(size)), size_(size) {
        std::fill(data_.get(), data_.get() + size, 0);
    }
    // Compiler generates correct move ctor/assign (unique_ptr is move-only)
    // Copy is deleted (unique_ptr is non-copyable) correct semantics!
    size_t size() const { return size_; }
    int& operator[](size_t idx) { return data_[idx]; }
};

// === Deleted operations ===
class Singleton {
public:
    static Singleton& instance() {
        static Singleton s;
        return s;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    void doWork() { std::cout << "  Singleton working\n"; }
private:
    Singleton() = default;
};

int main() {
    std::cout << "=== 1. Rule of Five: Buffer class ===\n";
    {
        Buffer b1(10);
        b1[0] = 42;
        b1[9] = 99;

        Buffer b2 = b1;         // Copy constructor
        assert(b2[0] == 42);
        assert(b2.size() == 10);

        Buffer b3(5);
        b3 = b1;               // Copy assignment
        assert(b3[9] == 99);

        Buffer b4 = std::move(b1); // Move constructor
        assert(b4[0] == 42);
        assert(b1.size() == 0);    // Moved-from state

        Buffer b5(3);
        b5 = std::move(b4);   // Move assignment
        assert(b5[9] == 99);
        assert(b4.size() == 0);
    }

    std::cout << "\n=== 2. Rule of Zero: ModernBuffer ===\n";
    {
        ModernBuffer mb(100);
        mb[0] = 7;
        ModernBuffer mb2 = std::move(mb); // Move works automatically
        assert(mb2[0] == 7);
        // ModernBuffer mb3 = mb2; // Won't compile copy deleted (correct!)
        std::cout << "  ModernBuffer: no special members needed!\n";
    }

    std::cout << "\n=== 3. Deleted operations (Singleton) ===\n";
    Singleton::instance().doWork();
    // Singleton s2 = Singleton::instance(); // Compile error: deleted

    std::cout << "\n=== Summary ===\n";
    std::cout << "  Rule of 0: Use RAII members, define nothing (preferred)\n";
    std::cout << "  Rule of 3: dtor + copy ctor + copy= (pre-C++11)\n";
    std::cout << "  Rule of 5: + move ctor + move= (modern C++)\n";
    std::cout << "  When custom dtor needed? Raw resource ownership.\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
