/*
 * RAII Resource Acquisition Is Initialization
 *
 * WHAT: The core C++ idiom where resource lifetime is tied to object lifetime.
 * Constructor acquires, destructor releases. Guarantees cleanup even with
 * exceptions the foundation of C++ safety.
 *
 * WHY IT MATTERS IN INTERVIEWS: RAII is THE design pattern of C++. Every
 * resource (memory, files, locks, sockets, handles) should be managed this way.
 * It's what makes C++ memory-safe without a garbage collector.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding that destructors guarantee cleanup (stack unwinding)
 * - Ability to write RAII wrappers for arbitrary resources
 * - Knowledge of standard RAII types (unique_ptr, lock_guard, fstream)
 * - Exception safety through RAII (no resource leaks on throw)
 *
 * COMMON PITFALLS:
 * - Manual resource management (forgetting to close/release on all paths)
 * - Exceptions between acquire and release in non-RAII code
 * - Not making RAII classes non-copyable when resource isn't copyable
 * - Destructor throwing exceptions (std::terminate)
 */

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <mutex>
#include <fstream>
#include <cstdio>
#include <string>
#include <memory>

// === 1. File Handle RAII Wrapper ===
class FileHandle {
    FILE* fp_ = nullptr;
    std::string path_;
public:
    explicit FileHandle(const std::string& path, const char* mode = "r")
        : path_(path) {
        fp_ = fopen(path.c_str(), mode);
        if (!fp_) throw std::runtime_error("Failed to open: " + path);
        std::cout << "  [FileHandle] Opened: " << path_ << "\n";
    }

    ~FileHandle() {
        if (fp_) {
            fclose(fp_);
            std::cout << "  [FileHandle] Closed: " << path_ << "\n";
        }
    }

    // Non-copyable (file handle is unique resource)
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;

    // Movable
    FileHandle(FileHandle&& other) noexcept
        : fp_(other.fp_), path_(std::move(other.path_)) {
        other.fp_ = nullptr;
    }

    FILE* get() const { return fp_; }
    bool isOpen() const { return fp_ != nullptr; }
};

// === 2. Scoped Lock (simplified lock_guard) ===
class ScopedLock {
    std::mutex& mtx_;
public:
    explicit ScopedLock(std::mutex& m) : mtx_(m) {
        mtx_.lock();
        std::cout << "  [ScopedLock] Acquired\n";
    }
    ~ScopedLock() {
        mtx_.unlock();
        std::cout << "  [ScopedLock] Released\n";
    }
    ScopedLock(const ScopedLock&) = delete;
    ScopedLock& operator=(const ScopedLock&) = delete;
};

// === 3. Generic ScopeGuard (execute on exit) ===
template <typename Func>
class ScopeGuard {
    Func cleanup_;
    bool active_;
public:
    explicit ScopeGuard(Func f) : cleanup_(std::move(f)), active_(true) {}
    ~ScopeGuard() { if (active_) cleanup_(); }
    void dismiss() { active_ = false; }
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

template <typename Func>
ScopeGuard<Func> makeScopeGuard(Func f) { return ScopeGuard<Func>(std::move(f)); }

// === 4. Demonstrating exception safety ===
void riskyOperation(bool shouldThrow) {
    FileHandle file("/dev/null", "w");
    std::cout << "  Doing work with file...\n";
    if (shouldThrow) {
        throw std::runtime_error("Something went wrong!");
        // File is STILL closed destructor runs during stack unwinding
    }
    std::cout << "  Work completed successfully\n";
}

int main() {
    std::cout << "=== 1. FileHandle RAII ===\n";
    {
        FileHandle f("/dev/null", "w");
        assert(f.isOpen());
    } // Automatically closed here

    std::cout << "\n=== 2. Exception safety with RAII ===\n";
    try {
        riskyOperation(true); // Throws!
    } catch (const std::exception& e) {
        std::cout << "  Caught: " << e.what() << "\n";
        std::cout << "  File was still properly closed (RAII guarantee)!\n";
    }

    std::cout << "\n=== 3. ScopedLock ===\n";
    {
        std::mutex mtx;
        ScopedLock lock(mtx);
        std::cout << "  Critical section lock held\n";
        // Even if exception here, lock is released
    }

    std::cout << "\n=== 4. ScopeGuard (generic cleanup) ===\n";
    {
        int resource_id = 42;
        auto guard = makeScopeGuard([&]() {
            std::cout << "  [ScopeGuard] Cleaning up resource " << resource_id << "\n";
        });
        std::cout << "  Using resource " << resource_id << "...\n";
        // guard.dismiss(); // Call this to skip cleanup if committed
    }

    std::cout << "\n=== 5. Standard RAII types ===\n";
    std::cout << "  std::unique_ptr heap memory\n";
    std::cout << "  std::lock_guard mutex locking\n";
    std::cout << "  std::fstream file I/O\n";
    std::cout << "  std::unique_lock flexible mutex\n";
    std::cout << "  std::jthread (C++20) thread with auto-join\n";

    std::cout << "\nAll assertions passed! RAII = no leaks, guaranteed.\n";
    return 0;
}
