/**************************************************************************************************
 * Topic: Runtime Decorator with Type Erasure Composable Without Inheritance
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★ (Architecture, FAANG)
 * What interviewers look for:
 *   - Decorator pattern without deep inheritance hierarchies
 *   - Type erasure as the runtime composition mechanism
 *   - Composing behaviors at runtime (not compile time)
 *   - Clean separation between what a thing IS and what behaviors are ADDED
 *
 * Day-to-day application:
 *   - Pricing with dynamic discounts (time-limited, coupon-based)
 *   - I/O stream decoration (compression → encryption → buffering)
 *   - Logging middleware that wraps any handler
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o runtime_decorator runtime_decorator.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <utility>

// ============================================================================
// Type-erased Item any type with .price() can be wrapped
// ============================================================================

class Item {
public:
    template <typename T>
    Item(T item)
        : pimpl_(std::make_unique<Model<T>>(std::move(item))) {}

    Item(Item const& other) : pimpl_(other.pimpl_->clone()) {}
    Item& operator=(Item const& other) {
        pimpl_ = other.pimpl_->clone();
        return *this;
    }
    Item(Item&&) = default;
    Item& operator=(Item&&) = default;
    ~Item() = default;

    int price_cents() const { return pimpl_->price_cents(); }
    std::string name() const { return pimpl_->name(); }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual int price_cents() const = 0;
        virtual std::string name() const = 0;
        virtual std::unique_ptr<Concept> clone() const = 0;
    };

    template <typename T>
    struct Model : Concept {
        explicit Model(T item) : item_(std::move(item)) {}
        int price_cents() const override { return item_.price_cents(); }
        std::string name() const override { return item_.name(); }
        std::unique_ptr<Concept> clone() const override {
            return std::make_unique<Model>(*this);
        }
        T item_;
    };

    std::unique_ptr<Concept> pimpl_;
};

// ============================================================================
// Concrete items
// ============================================================================

class Coffee {
public:
    Coffee(std::string name, int cents) : name_(std::move(name)), cents_(cents) {}
    int price_cents() const { return cents_; }
    std::string name() const { return name_; }

private:
    std::string name_;
    int cents_;
};

// ============================================================================
// Decorators regular classes that wrap an Item (not inheritance!)
// ============================================================================

class WithMilk {
public:
    explicit WithMilk(Item item) : item_(std::move(item)) {}
    int price_cents() const { return item_.price_cents() + 50; }  // +$0.50
    std::string name() const { return item_.name() + " + Milk"; }

private:
    Item item_;
};

class WithSugar {
public:
    explicit WithSugar(Item item) : item_(std::move(item)) {}
    int price_cents() const { return item_.price_cents() + 25; }  // +$0.25
    std::string name() const { return item_.name() + " + Sugar"; }

private:
    Item item_;
};

class WithDiscount {
public:
    WithDiscount(int percent, Item item) : item_(std::move(item)), percent_(percent) {}
    int price_cents() const {
        return item_.price_cents() * (100 - percent_) / 100;
    }
    std::string name() const {
        return item_.name() + " (-" + std::to_string(percent_) + "%)";
    }

private:
    Item item_;
    int percent_;
};

// ============================================================================
// Usage Runtime composition, any order of decorations
// ============================================================================

int main() {
    std::cout << "=== Runtime Decorator with Type Erasure ===\n\n";

    // Basic coffee
    Item espresso = Coffee{"Espresso", 300};
    std::cout << espresso.name() << ": $" << espresso.price_cents() / 100.0 << '\n';

    // Decorate at runtime
    Item latte = WithMilk{Coffee{"Latte", 400}};
    std::cout << latte.name() << ": $" << latte.price_cents() / 100.0 << '\n';

    // Stack decorators: Sugar → Milk → Coffee
    Item fancy = WithSugar{WithMilk{Coffee{"Mocha", 450}}};
    std::cout << fancy.name() << ": $" << fancy.price_cents() / 100.0 << '\n';

    // With discount applied last
    Item deal = WithDiscount{10, WithMilk{Coffee{"Americano", 350}}};
    std::cout << deal.name() << ": $" << deal.price_cents() / 100.0 << '\n';

    // Assertions
    assert(espresso.price_cents() == 300);
    assert(latte.price_cents() == 450);        // 400 + 50
    assert(fancy.price_cents() == 525);        // 450 + 50 + 25
    assert(deal.price_cents() == 360);         // (350 + 50) * 0.9 = 360

    // Copy works (deep copy via clone)
    Item copy = fancy;
    assert(copy.price_cents() == fancy.price_cents());
    assert(copy.name() == fancy.name());

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
