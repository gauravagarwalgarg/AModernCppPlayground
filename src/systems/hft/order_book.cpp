/**
 * @file order_book.cpp
 * @brief Limit Order Book Implementation - HFT Interview Staple
 * 
 * Data Structure: std::map<price, std::list<Order>> for each side (bid/ask)
 * 
 * Why std::map?
 *   - Sorted by price → O(log n) insert/delete, O(1) best price (begin/rbegin)
 *   - Red-black tree guarantees no worst-case degradation
 *   - Bids: reverse-sorted (highest first), Asks: sorted (lowest first)
 * 
 * Why std::list at each price level?
 *   - FIFO within same price (price-time priority)
 *   - O(1) insert at back, O(1) cancel with iterator
 *   - No iterator invalidation on other operations
 * 
 * Complexity: addOrder O(log P), cancelOrder O(1) with saved iterator, getTopOfBook O(1)
 * where P = number of distinct price levels
 * 
 * Compile: g++ -std=c++20 -Wall -pthread order_book.cpp -o order_book
 */

#include <iostream>
#include <map>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <chrono>

enum class Side : uint8_t { Buy, Sell };

struct Order {
    uint64_t id;
    Side side;
    double price;
    uint32_t quantity;
    int64_t timestamp;
};

class OrderBook {
public:
    using OrderList = std::list<Order>;
    using PriceLevel = std::map<double, OrderList>;

    // O(log P) where P = number of price levels
    void addOrder(uint64_t id, Side side, double price, uint32_t qty) {
        Order order{id, side, price, qty, now()};

        auto& book = (side == Side::Buy) ? bids_ : asks_;
        auto& level = book[price];  // creates level if not exists
        level.push_back(order);

        // Store iterator for O(1) cancel
        auto it = std::prev(level.end());
        order_lookup_[id] = {side, price, it};
    }

    // O(1) with stored iterator (amortized, may remove empty level)
    bool cancelOrder(uint64_t id) {
        auto it = order_lookup_.find(id);
        if (it == order_lookup_.end()) return false;

        auto& [side, price, list_it] = it->second;
        auto& book = (side == Side::Buy) ? bids_ : asks_;

        auto level_it = book.find(price);
        if (level_it != book.end()) {
            level_it->second.erase(list_it);
            if (level_it->second.empty()) {
                book.erase(level_it);  // Remove empty price level
            }
        }
        order_lookup_.erase(it);
        return true;
    }

    // O(1) - best bid is rbegin() of sorted map, best ask is begin()
    struct TopOfBook {
        double best_bid = 0.0, best_ask = 0.0;
        uint32_t bid_qty = 0, ask_qty = 0;
    };

    TopOfBook getTopOfBook() const {
        TopOfBook top;
        if (!bids_.empty()) {
            top.best_bid = bids_.rbegin()->first;
            for (const auto& order : bids_.rbegin()->second)
                top.bid_qty += order.quantity;
        }
        if (!asks_.empty()) {
            top.best_ask = asks_.begin()->first;
            for (const auto& order : asks_.begin()->second)
                top.ask_qty += order.quantity;
        }
        return top;
    }

    void printBook(int depth = 3) const {
        std::cout << "\n=== ORDER BOOK ===\n";
        std::cout << "--- ASKS (sell) ---\n";
        int count = 0;
        for (auto it = asks_.begin(); it != asks_.end() && count < depth; ++it, ++count) {
            uint32_t total_qty = 0;
            for (const auto& o : it->second) total_qty += o.quantity;
            std::cout << "  $" << it->first << " x " << total_qty
                      << " (" << it->second.size() << " orders)\n";
        }
        std::cout << "--- BIDS (buy) ---\n";
        count = 0;
        for (auto it = bids_.rbegin(); it != bids_.rend() && count < depth; ++it, ++count) {
            uint32_t total_qty = 0;
            for (const auto& o : it->second) total_qty += o.quantity;
            std::cout << "  $" << it->first << " x " << total_qty
                      << " (" << it->second.size() << " orders)\n";
        }
    }

private:
    PriceLevel bids_;  // Highest price = rbegin() = best bid
    PriceLevel asks_;  // Lowest price = begin() = best ask

    struct OrderLocation {
        Side side;
        double price;
        OrderList::iterator it;
    };
    std::unordered_map<uint64_t, OrderLocation> order_lookup_;

    static int64_t now() {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }
};

int main() {
    std::cout << "=== Limit Order Book Demo ===\n";
    OrderBook book;

    // Simulate market activity
    book.addOrder(1, Side::Buy,  100.00, 500);
    book.addOrder(2, Side::Buy,  100.00, 300);  // Same price, queued behind order 1
    book.addOrder(3, Side::Buy,   99.50, 200);
    book.addOrder(4, Side::Sell, 100.50, 400);
    book.addOrder(5, Side::Sell, 101.00, 600);
    book.addOrder(6, Side::Sell, 100.50, 100);  // Same ask level

    book.printBook();

    auto top = book.getTopOfBook();
    std::cout << "\nTop of Book: Bid $" << top.best_bid << " x " << top.bid_qty
              << " | Ask $" << top.best_ask << " x " << top.ask_qty << "\n";
    std::cout << "Spread: $" << (top.best_ask - top.best_bid) << "\n";

    // Cancel an order
    std::cout << "\n--- Cancel order 2 (buy $100 x 300) ---\n";
    book.cancelOrder(2);
    book.printBook();

    top = book.getTopOfBook();
    std::cout << "\nUpdated Top: Bid $" << top.best_bid << " x " << top.bid_qty << "\n";

    return 0;
}
