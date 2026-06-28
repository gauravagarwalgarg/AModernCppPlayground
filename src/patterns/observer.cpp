// observer.cpp - Observer / Event Bus Pattern
// Interview Relevance: Core pattern in event-driven systems. Tests understanding of
// decoupled communication, std::function for type-safe callbacks, and lifetime management.
// Real use: market data distribution, GUI event loops, pub/sub messaging.
// Compile: g++ -std=c++20 -Wall observer.cpp -o observer

#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <cassert>

class EventBus {
public:
    using Callback = std::function<void(const std::string&)>;

    // Subscribe returns an ID for unsubscription
    int subscribe(const std::string& event, Callback cb) {
        int id = next_id_++;
        listeners_[event].push_back({id, std::move(cb)});
        return id;
    }

    void unsubscribe(const std::string& event, int id) {
        auto& vec = listeners_[event];
        vec.erase(std::remove_if(vec.begin(), vec.end(),
            [id](const Listener& l) { return l.id == id; }), vec.end());
    }

    void emit(const std::string& event, const std::string& data) {
        if (auto it = listeners_.find(event); it != listeners_.end()) {
            for (const auto& listener : it->second) {
                listener.callback(data);
            }
        }
    }

    size_t listenerCount(const std::string& event) const {
        auto it = listeners_.find(event);
        return it != listeners_.end() ? it->second.size() : 0;
    }

private:
    struct Listener {
        int id;
        Callback callback;
    };
    std::unordered_map<std::string, std::vector<Listener>> listeners_;
    int next_id_ = 0;
};

int main() {
    EventBus bus;
    int trade_count = 0;
    std::string last_price;

    // Subscriber 1: Trade logger
    bus.subscribe("trade", [&trade_count](const std::string& data) {
        ++trade_count;
        std::cout << "  [TradeLogger] Trade executed: " << data << "\n";
    });

    // Subscriber 2: Price tracker
    int price_sub = bus.subscribe("price_update", [&last_price](const std::string& data) {
        last_price = data;
        std::cout << "  [PriceTracker] New price: " << data << "\n";
    });

    // Subscriber 3: Alert system
    bus.subscribe("price_update", [](const std::string& data) {
        if (std::stod(data) > 150.0)
            std::cout << "  [ALERT] Price exceeds threshold: " << data << "\n";
    });

    std::cout << "--- Emitting events ---\n";
    bus.emit("trade", "AAPL 100@155.50");
    bus.emit("price_update", "155.50");
    bus.emit("price_update", "148.20");

    assert(trade_count == 1);
    assert(last_price == "148.20");
    assert(bus.listenerCount("price_update") == 2);

    // Unsubscribe price tracker
    bus.unsubscribe("price_update", price_sub);
    assert(bus.listenerCount("price_update") == 1);

    std::cout << "\n--- After unsubscribing PriceTracker ---\n";
    bus.emit("price_update", "160.00");  // Only alert fires

    std::cout << "\nKey insight: Publishers and subscribers are fully decoupled.\n";
    std::cout << "Neither knows about the other - only the event bus connects them.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
