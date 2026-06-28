/**************************************************************************************************
 * Topic: Modern Observer Pattern Value Semantics with std::function
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★★
 * What interviewers look for:
 *   - Classic Observer vs Modern Observer (std::function callbacks)
 *   - Connection management: how to safely disconnect
 *   - Avoiding dangling pointers via token-based subscription
 *   - Thread safety considerations
 *
 * Day-to-day application:
 *   - Event systems in any application
 *   - Reactive UIs (property changed notifications)
 *   - Pub/sub in distributed systems
 *   - Market data distribution in trading systems
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o modern_observer modern_observer.cpp
 **************************************************************************************************/

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

// ============================================================================
// Connection token RAII-based automatic unsubscription
// ============================================================================

class Connection {
public:
    using DisconnectFn = std::function<void()>;

    Connection() = default;
    explicit Connection(DisconnectFn fn) : disconnect_(std::move(fn)) {}

    ~Connection() { disconnect(); }

    // Non-copyable, movable
    Connection(Connection const&) = delete;
    Connection& operator=(Connection const&) = delete;
    Connection(Connection&& other) noexcept : disconnect_(std::exchange(other.disconnect_, nullptr)) {}
    Connection& operator=(Connection&& other) noexcept {
        disconnect();
        disconnect_ = std::exchange(other.disconnect_, nullptr);
        return *this;
    }

    void disconnect() {
        if (disconnect_) {
            disconnect_();
            disconnect_ = nullptr;
        }
    }

private:
    DisconnectFn disconnect_;
};

// ============================================================================
// Observable Subject that notifies subscribers
// ============================================================================

template <typename... Args>
class Signal {
public:
    using Slot = std::function<void(Args...)>;

    Signal() = default;

    // Subscribe returns a Connection for automatic cleanup
    [[nodiscard]] Connection connect(Slot slot) {
        auto id = next_id_++;
        slots_.emplace_back(id, std::move(slot));

        return Connection{[this, id]() {
            slots_.erase(
                std::remove_if(slots_.begin(), slots_.end(),
                               [id](auto const& p) { return p.first == id; }),
                slots_.end());
        }};
    }

    // Fire the signal
    void emit(Args... args) const {
        for (auto const& [id, slot] : slots_) {
            slot(args...);
        }
    }

    std::size_t subscriber_count() const { return slots_.size(); }

private:
    std::vector<std::pair<uint64_t, Slot>> slots_;
    uint64_t next_id_{0};
};

// ============================================================================
// Example: Stock price observable
// ============================================================================

class Stock {
public:
    explicit Stock(std::string symbol, double price)
        : symbol_(std::move(symbol)), price_(price) {}

    std::string const& symbol() const { return symbol_; }
    double price() const { return price_; }

    void set_price(double new_price) {
        double old_price = price_;
        price_ = new_price;
        price_changed_.emit(symbol_, old_price, new_price);
    }

    Signal<std::string const&, double, double>& on_price_changed() {
        return price_changed_;
    }

private:
    std::string symbol_;
    double price_;
    Signal<std::string const&, double, double> price_changed_;
};

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    Stock aapl("AAPL", 150.0);
    int notification_count = 0;

    // Subscribe with lambda
    auto conn1 = aapl.on_price_changed().connect(
        [&](std::string const& sym, double old_p, double new_p) {
            std::cout << sym << ": " << old_p << " -> " << new_p << '\n';
            notification_count++;
        });

    // Subscribe with another observer
    auto conn2 = aapl.on_price_changed().connect(
        [](std::string const& sym, double, double new_p) {
            if (new_p > 200.0)
                std::cout << "ALERT: " << sym << " above $200!\n";
        });

    assert(aapl.on_price_changed().subscriber_count() == 2);

    aapl.set_price(155.0);
    aapl.set_price(210.0);

    // Disconnect first observer via RAII
    conn1.disconnect();
    assert(aapl.on_price_changed().subscriber_count() == 1);

    aapl.set_price(220.0);  // Only conn2 fires

    assert(notification_count == 2);  // conn1 was called twice before disconnect

    // conn2 auto-disconnects when it goes out of scope
    {
        auto conn3 = aapl.on_price_changed().connect(
            [](auto const&, auto, auto) {});
        assert(aapl.on_price_changed().subscriber_count() == 2);
    }  // conn3 destroyed here
    assert(aapl.on_price_changed().subscriber_count() == 1);

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
