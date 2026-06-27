/**
 * @file mmap_shared_memory.cpp
 * @brief Memory-Mapped File / Shared Memory - Zero-Copy IPC
 * 
 * mmap() maps a file (or anonymous memory) directly into process address space.
 * Benefits:
 *   - Zero-copy: no read()/write() syscalls, kernel handles page faults
 *   - Shared between processes: multiple processes map same file
 *   - Persistent: file-backed mmap survives process restart
 * 
 * Use cases in HFT/systems:
 *   - Shared memory IPC between processes (fastest IPC after shared cache)
 *   - Memory-mapped log files
 *   - Database storage engines (LMDB, SQLite WAL)
 * 
 * This demo simulates writer/reader within one process using mmap'd file.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread mmap_shared_memory.cpp -o mmap_shared_memory
 */

#ifdef __linux__
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

// RAII wrapper for mmap'd regions
class MappedRegion {
    void* addr_ = MAP_FAILED;
    size_t size_ = 0;

public:
    MappedRegion(int fd, size_t size, int prot, int flags)
        : size_(size) {
        addr_ = mmap(nullptr, size, prot, flags, fd, 0);
        if (addr_ == MAP_FAILED) {
            throw std::runtime_error("mmap failed: " + std::string(strerror(errno)));
        }
    }

    ~MappedRegion() {
        if (addr_ != MAP_FAILED) {
            msync(addr_, size_, MS_SYNC);  // Flush to disk before unmap
            munmap(addr_, size_);
        }
    }

    MappedRegion(const MappedRegion&) = delete;
    MappedRegion& operator=(const MappedRegion&) = delete;

    void* data() noexcept { return addr_; }
    const void* data() const noexcept { return addr_; }
    size_t size() const noexcept { return size_; }

    template<typename T>
    T* as() noexcept { return static_cast<T*>(addr_); }
};

// Shared data structure (must be POD for shared memory)
struct SharedData {
    int64_t sequence;
    double price;
    int32_t quantity;
    char symbol[16];
    bool valid;
};

int main() {
    std::cout << "=== Memory-Mapped Shared Memory Demo ===\n";
    const char* filepath = "/tmp/mmap_demo.dat";
    constexpr size_t FILE_SIZE = sizeof(SharedData) * 4;

    // Create and size the backing file
    int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) { perror("open"); return 1; }
    ftruncate(fd, FILE_SIZE);  // Set file size

    // Map the file into memory (read/write, shared with other processes)
    MappedRegion region(fd, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED);
    close(fd);  // fd can be closed after mmap

    auto* data = region.as<SharedData>();
    std::cout << "Mapped " << FILE_SIZE << " bytes at " << region.data() << "\n\n";

    // === WRITER (simulated process 1) ===
    std::cout << "--- Writer: Publishing market data ---\n";
    const char* symbols[] = {"AAPL", "GOOGL", "MSFT", "TSLA"};
    double prices[] = {150.25, 2800.50, 340.75, 250.00};

    for (int i = 0; i < 4; ++i) {
        data[i].sequence = i + 1;
        data[i].price = prices[i];
        data[i].quantity = (i + 1) * 100;
        strncpy(data[i].symbol, symbols[i], sizeof(data[i].symbol) - 1);
        data[i].valid = true;
        std::cout << "  Written: seq=" << data[i].sequence << " " << data[i].symbol
                  << " $" << data[i].price << "\n";
    }

    // Memory fence - ensure writes are visible (for multi-process use atomic_thread_fence)
    __sync_synchronize();

    // === READER (simulated process 2) ===
    std::cout << "\n--- Reader: Reading shared memory ---\n";
    // Re-map read-only (simulating a different process)
    fd = open(filepath, O_RDONLY);
    MappedRegion reader_region(fd, FILE_SIZE, PROT_READ, MAP_SHARED);
    close(fd);

    auto* read_data = reader_region.as<const SharedData>();
    for (int i = 0; i < 4; ++i) {
        if (read_data[i].valid) {
            std::cout << "  Read: seq=" << read_data[i].sequence << " "
                      << read_data[i].symbol << " $" << read_data[i].price
                      << " qty=" << read_data[i].quantity << "\n";
        }
    }

    // Cleanup
    unlink(filepath);
    std::cout << "\nDemo complete. File removed.\n";
    return 0;
}

#else
#include <iostream>
int main() {
    std::cout << "mmap demo requires Linux/POSIX.\n";
    return 0;
}
#endif
