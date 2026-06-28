/**
 * @file market_data_handler.cpp
 * @brief Simulated Market Data Processor - Low-Latency Tick Handler
 * 
 * Demonstrates HFT-critical techniques:
 *   1. Struct alignment with #pragma pack - minimize cache line waste
 *   2. [[likely]]/[[unlikely]] branch hints - help branch predictor
 *   3. Hot-path optimization - zero allocations, pre-allocated buffers
 *   4. OHLCV bar aggregation from raw ticks
 *   5. Exponential moving average (EMA) without division in hot path
 * 
 * In production: this would read from kernel-bypass NIC (DPDK/ef_vi),
 * use lock-free queues, and pin to isolated CPU cores.
 * 
 * Compile: g++ -std=c++20 -Wall -O2 -pthread market_data_handler.cpp -o market_data_handler
 */

#include <iostream>
#include <array>
#include <chrono>
#include <cstring>
#include <cmath>

// Packed tick - matches wire format, minimize padding
#pragma pack(push, 1)
struct alignas(1) MarketTick {
    char symbol[8];       // 8 bytes - fixed size, no std::string allocation
    double price;         // 8 bytes
    uint32_t quantity;    // 4 bytes
    int64_t timestamp_ns; // 8 bytes - nanosecond precision
    char side;            // 1 byte: 'B' or 'S'
};                        // Total: 29 bytes packed (vs 40+ with padding)
#pragma pack(pop)

struct OHLCVBar {
    double open = 0, high = 0, low = 0, close = 0;
    uint64_t volume = 0;
    int64_t bar_start_ns = 0;
    int count = 0;

    void reset(double price, int64_t ts) {
        open = high = low = close = price;
        volume = 0;
        bar_start_ns = ts;
        count = 0;
    }
};

class MarketDataHandler {
    static constexpr int64_t BAR_DURATION_NS = 1'000'000'000; // 1-second bars
    static constexpr double EMA_ALPHA = 0.1;  // EMA smoothing factor

    OHLCVBar current_bar_{};
    double ema_price_ = 0.0;
    bool ema_initialized_ = false;
    uint64_t ticks_processed_ = 0;

public:
    // HOT PATH - called per tick, must be ultra-fast
    void onTick(const MarketTick& tick) noexcept {
        ++ticks_processed_;

        // New bar period?
        if (__builtin_expect(current_bar_.count == 0, 0)) [[unlikely]] {
            current_bar_.reset(tick.price, tick.timestamp_ns);
        }

        // Check if current tick belongs to new bar
        int64_t elapsed = tick.timestamp_ns - current_bar_.bar_start_ns;
        if (elapsed >= BAR_DURATION_NS) [[unlikely]] {
            emitBar();
            current_bar_.reset(tick.price, tick.timestamp_ns);
        }

        // Update OHLCV - no branches for high/low (branchless would use cmov)
        if (tick.price > current_bar_.high) [[unlikely]] current_bar_.high = tick.price;
        if (tick.price < current_bar_.low)  [[unlikely]] current_bar_.low = tick.price;
        current_bar_.close = tick.price;
        current_bar_.volume += tick.quantity;
        current_bar_.count++;

        // EMA update: EMA = alpha * price + (1 - alpha) * prev_EMA
        if (__builtin_expect(!ema_initialized_, 0)) [[unlikely]] {
            ema_price_ = tick.price;
            ema_initialized_ = true;
        } else {
            ema_price_ += EMA_ALPHA * (tick.price - ema_price_);
        }
    }

    double getEMA() const noexcept { return ema_price_; }
    uint64_t tickCount() const noexcept { return ticks_processed_; }

private:
    void emitBar() const {
        std::cout << "BAR | O:" << current_bar_.open << " H:" << current_bar_.high
                  << " L:" << current_bar_.low << " C:" << current_bar_.close
                  << " V:" << current_bar_.volume << " ticks:" << current_bar_.count << "\n";
    }
};

int main() {
    std::cout << "=== Market Data Handler Demo ===\n";
    std::cout << "MarketTick size: " << sizeof(MarketTick) << " bytes (packed)\n\n";

    MarketDataHandler handler;

    // Simulate tick stream - pre-allocated array (no heap allocation)
    std::array<MarketTick, 10> ticks{};
    const char* sym = "AAPL";
    double prices[] = {150.0, 150.5, 149.8, 151.0, 150.2, 150.8, 149.5, 151.5, 150.0, 150.3};
    int64_t base_ts = 1'000'000'000;

    for (int i = 0; i < 10; ++i) {
        std::memcpy(ticks[i].symbol, sym, 4);
        ticks[i].price = prices[i];
        ticks[i].quantity = 100 * (i + 1);
        ticks[i].timestamp_ns = base_ts + i * 200'000'000; // 200ms apart
        ticks[i].side = (i % 2 == 0) ? 'B' : 'S';
    }

    // Measure processing latency
    auto start = std::chrono::high_resolution_clock::now();
    for (const auto& tick : ticks) {
        handler.onTick(tick);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << "\nProcessed " << handler.tickCount() << " ticks in " << ns << " ns"
              << " (" << ns / handler.tickCount() << " ns/tick)\n";
    std::cout << "EMA Price: " << handler.getEMA() << "\n";

    return 0;
}
