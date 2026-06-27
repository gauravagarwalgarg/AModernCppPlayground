/**
 * @file virtual_memory.cpp
 * @brief Page Table & TLB Simulation - Virtual Memory Translation
 * 
 * Concepts demonstrated:
 *   - Virtual → Physical address translation via page table
 *   - TLB (Translation Lookaside Buffer): small cache of recent translations
 *   - Page faults: access to unmapped pages triggers fault handler
 *   - Page size: typically 4KB (12-bit offset), configurable here
 * 
 * Address structure: [Virtual Page Number | Offset within page]
 * Translation: VPN → Page Table → Physical Frame Number → Physical Address
 * 
 * Real hardware: multi-level page tables (x86-64 has 4 levels), hardware TLB.
 * This is a single-level educational implementation.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread virtual_memory.cpp -o virtual_memory
 */

#include <iostream>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <string>

class VirtualMemory {
    static constexpr uint32_t PAGE_SIZE = 4096;          // 4KB pages
    static constexpr uint32_t OFFSET_BITS = 12;          // log2(4096)
    static constexpr uint32_t OFFSET_MASK = PAGE_SIZE - 1;
    static constexpr size_t TLB_SIZE = 4;                // Tiny TLB for demo

    // Page table: virtual page → physical frame
    std::unordered_map<uint32_t, uint32_t> page_table_;
    
    // TLB cache: most recent translations (associative cache)
    struct TLBEntry { uint32_t vpn; uint32_t pfn; bool valid = false; };
    std::array<TLBEntry, TLB_SIZE> tlb_{};
    size_t tlb_next_ = 0;  // Round-robin replacement

    // Statistics
    uint64_t tlb_hits_ = 0, tlb_misses_ = 0, page_faults_ = 0;

public:
    // Map a virtual page to a physical frame
    void mapPage(uint32_t virtual_page, uint32_t physical_frame) {
        page_table_[virtual_page] = physical_frame;
        std::cout << "  Mapped: VP " << virtual_page << " → PF " << physical_frame << "\n";
    }

    // Translate virtual address to physical address
    struct TranslationResult {
        bool success;
        uint32_t physical_addr;
        std::string source;  // "TLB" or "PageTable" or "Fault"
    };

    TranslationResult translate(uint32_t virtual_addr) {
        uint32_t vpn = virtual_addr >> OFFSET_BITS;
        uint32_t offset = virtual_addr & OFFSET_MASK;

        // Step 1: Check TLB (fast path)
        for (auto& entry : tlb_) {
            if (entry.valid && entry.vpn == vpn) {
                ++tlb_hits_;
                uint32_t phys = (entry.pfn << OFFSET_BITS) | offset;
                return {true, phys, "TLB Hit"};
            }
        }
        ++tlb_misses_;

        // Step 2: Walk page table (slow path)
        auto it = page_table_.find(vpn);
        if (it == page_table_.end()) {
            // PAGE FAULT - page not mapped
            ++page_faults_;
            return {false, 0, "Page Fault"};
        }

        // Found in page table - update TLB (round-robin eviction)
        uint32_t pfn = it->second;
        tlb_[tlb_next_] = {vpn, pfn, true};
        tlb_next_ = (tlb_next_ + 1) % TLB_SIZE;

        uint32_t phys = (pfn << OFFSET_BITS) | offset;
        return {true, phys, "Page Table"};
    }

    void printStats() const {
        std::cout << "\n--- MMU Statistics ---\n";
        std::cout << "TLB Hits:    " << tlb_hits_ << "\n";
        std::cout << "TLB Misses:  " << tlb_misses_ << "\n";
        std::cout << "Page Faults: " << page_faults_ << "\n";
        uint64_t total = tlb_hits_ + tlb_misses_;
        if (total > 0) {
            std::cout << "TLB Hit Rate: " << (100.0 * tlb_hits_ / total) << "%\n";
        }
    }

    void flushTLB() {
        for (auto& entry : tlb_) entry.valid = false;
        std::cout << "TLB flushed (e.g., context switch)\n";
    }
};

int main() {
    std::cout << "=== Virtual Memory Simulation ===\n\n";
    VirtualMemory vm;

    // Setup page mappings (like OS loading a process)
    std::cout << "Setting up page table:\n";
    vm.mapPage(0, 5);    // Code segment: VP0 → PF5
    vm.mapPage(1, 8);    // Data segment: VP1 → PF8
    vm.mapPage(2, 12);   // Heap: VP2 → PF12
    vm.mapPage(10, 20);  // Stack: VP10 → PF20

    std::cout << "\n--- Address Translations ---\n";
    // Translate various addresses
    uint32_t test_addrs[] = {
        0x00000100,  // VP0, offset 256 → should go to PF5
        0x00001200,  // VP1, offset 512 → should go to PF8
        0x00000100,  // VP0 again → TLB hit
        0x00002FFF,  // VP2, offset 4095 → PF12
        0x0000A004,  // VP10, offset 4 → PF20
        0x00005000,  // VP5 - UNMAPPED → page fault!
        0x00000100,  // VP0 again → TLB hit
    };

    for (uint32_t addr : test_addrs) {
        auto result = vm.translate(addr);
        std::cout << "  VA 0x" << std::hex << addr << " → ";
        if (result.success) {
            std::cout << "PA 0x" << result.physical_addr;
        } else {
            std::cout << "FAULT";
        }
        std::cout << std::dec << " [" << result.source << "]\n";
    }

    vm.printStats();

    // Demonstrate TLB flush (happens on context switch)
    std::cout << "\n";
    vm.flushTLB();
    auto result = vm.translate(0x00000100);  // Will be TLB miss now
    std::cout << "After flush: VA 0x100 → [" << result.source << "]\n";

    return 0;
}
