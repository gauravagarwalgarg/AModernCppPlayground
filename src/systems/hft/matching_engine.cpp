/**
 * @file matching_engine.cpp
 * @brief Simple Matching Engine - Core of an Exchange
 * 
 * Price-Time Priority matching:
 *   1. Best price gets priority (highest bid matches first, lowest ask matches first)
 *   2. At same price, earlier order (FIFO) gets priority
 * 
 * Flow: Incoming order → Check opposite book for match → Execute trades → Rest in book
 * 
 * This is the heart of every electronic exchange (NYSE, CME, NASDAQ).
 * Interview focus: Understand the matching algorithm, not just the data structure.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread matching_engine.cpp -o matching_engine
 */

#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <cstdint>
#include <string>

enum class Side : uint8_t { Buy, Sell };

struct Order {
    uint64_t id;
    Side side;
    double price;
    uint32_t quantity;
    uint64_t timestamp;
};

struct Trade {
    uint64_t buy_order_id;
    uint64_t sell_order_id;
    double price;
    uint32_t quantity;
};

class MatchingEngine {
    using OrderList = std::list<Order>;
    // Bids: highest price first (reverse order), Asks: lowest price first
    std::map<double, OrderList, std::greater<>> bids_;  // descending
    std::map<double, OrderList, std::less<>> asks_;     // ascending
    std::vector<Trade> trades_;
    uint64_t next_order_id_ = 1;
    uint64_t timestamp_ = 0;

public:
    uint64_t submitOrder(Side side, double price, uint32_t qty) {
        Order order{next_order_id_++, side, price, qty, timestamp_++};
        
        if (side == Side::Buy) {
            matchBuy(order);
        } else {
            matchSell(order);
        }
        return order.id;
    }

    const std::vector<Trade>& getTrades() const { return trades_; }

    void printBook() const {
        std::cout << "\n--- ASKS ---\n";
        for (auto it = asks_.rbegin(); it != asks_.rend(); ++it) {
            uint32_t qty = 0;
            for (const auto& o : it->second) qty += o.quantity;
            std::cout << "  $" << it->first << " x " << qty << "\n";
        }
        std::cout << "--- BIDS ---\n";
        for (const auto& [price, orders] : bids_) {
            uint32_t qty = 0;
            for (const auto& o : orders) qty += o.quantity;
            std::cout << "  $" << price << " x " << qty << "\n";
        }
    }

private:
    void matchBuy(Order& order) {
        // Buy matches against asks: match if buy_price >= ask_price
        while (order.quantity > 0 && !asks_.empty()) {
            auto best_ask = asks_.begin();
            if (order.price < best_ask->first) break;  // No match possible

            auto& ask_orders = best_ask->second;
            auto& resting = ask_orders.front();

            uint32_t fill_qty = std::min(order.quantity, resting.quantity);
            trades_.push_back({order.id, resting.id, resting.price, fill_qty});

            order.quantity -= fill_qty;
            resting.quantity -= fill_qty;

            if (resting.quantity == 0) {
                ask_orders.pop_front();
                if (ask_orders.empty()) asks_.erase(best_ask);
            }
        }
        // Rest unfilled portion in book
        if (order.quantity > 0) {
            bids_[order.price].push_back(order);
        }
    }

    void matchSell(Order& order) {
        // Sell matches against bids: match if sell_price <= bid_price
        while (order.quantity > 0 && !bids_.empty()) {
            auto best_bid = bids_.begin();  // highest bid (std::greater comparator)
            if (order.price > best_bid->first) break;

            auto& bid_orders = best_bid->second;
            auto& resting = bid_orders.front();

            uint32_t fill_qty = std::min(order.quantity, resting.quantity);
            trades_.push_back({resting.id, order.id, resting.price, fill_qty});

            order.quantity -= fill_qty;
            resting.quantity -= fill_qty;

            if (resting.quantity == 0) {
                bid_orders.pop_front();
                if (bid_orders.empty()) bids_.erase(best_bid);
            }
        }
        if (order.quantity > 0) {
            asks_[order.price].push_back(order);
        }
    }
};

int main() {
    std::cout << "=== Matching Engine Demo ===\n";
    MatchingEngine engine;

    // Build initial book
    engine.submitOrder(Side::Buy,  100.0, 500);
    engine.submitOrder(Side::Buy,   99.5, 300);
    engine.submitOrder(Side::Sell, 101.0, 400);
    engine.submitOrder(Side::Sell, 101.5, 200);
    std::cout << "Initial book (no crosses):";
    engine.printBook();

    // Aggressive buy that crosses the spread
    std::cout << "\n>>> Aggressive BUY $101.0 x 250\n";
    engine.submitOrder(Side::Buy, 101.0, 250);

    std::cout << "After aggressive buy:";
    engine.printBook();

    // Aggressive sell that sweeps multiple levels
    std::cout << "\n>>> Aggressive SELL $99.0 x 700 (sweeps all bids)\n";
    engine.submitOrder(Side::Sell, 99.0, 700);

    std::cout << "After aggressive sell:";
    engine.printBook();

    // Print all trades
    std::cout << "\n=== TRADE LOG ===\n";
    for (const auto& t : engine.getTrades()) {
        std::cout << "Trade: Buy#" << t.buy_order_id << " x Sell#" << t.sell_order_id
                  << " @ $" << t.price << " qty=" << t.quantity << "\n";
    }

    return 0;
}
