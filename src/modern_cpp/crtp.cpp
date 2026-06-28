/*
 * CRTP Curiously Recurring Template Pattern
 *
 * WHAT: A class derives from a template instantiated with itself:
 * class Derived : public Base<Derived>. Enables static polymorphism,
 * mixin functionality, and eliminates virtual call overhead.
 *
 * WHY IT MATTERS IN INTERVIEWS: HFT firms LOVE this pattern. It provides
 * polymorphism without vtable overhead no indirect branch, no cache miss.
 * Used in quantitative libraries, Eigen, std::enable_shared_from_this.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - Understanding static vs dynamic polymorphism tradeoffs
 * - Implementing mixin patterns (Comparable, Clonable, Printable)
 * - Performance implications (inlined calls vs vtable indirection)
 * - Real-world examples (enable_shared_from_this, Eigen)
 * - Knowing when CRTP is preferred (compile-time known types, hot paths)
 *
 * COMMON PITFALLS:
 * - Accidentally using wrong derived class in template parameter
 * - CRTP doesn't support runtime polymorphism (can't store in base container)
 * - Constructor ordering issues with CRTP bases
 * - Code bloat from template instantiations
 */

#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include <vector>
#include <memory>

// === 1. CRTP for static polymorphism (zero-overhead) ===
template <typename Derived>
class Shape {
public:
    double area() const {
        return static_cast<const Derived*>(this)->areaImpl();
    }
    void draw() const {
        static_cast<const Derived*>(this)->drawImpl();
    }
};

class Circle : public Shape<Circle> {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    double areaImpl() const { return 3.14159265 * radius_ * radius_; }
    void drawImpl() const { std::cout << "  Drawing Circle(r=" << radius_ << ")\n"; }
};

class Square : public Shape<Square> {
    double side_;
public:
    explicit Square(double s) : side_(s) {}
    double areaImpl() const { return side_ * side_; }
    void drawImpl() const { std::cout << "  Drawing Square(s=" << side_ << ")\n"; }
};

template <typename T>
void printArea(const Shape<T>& shape) {
    std::cout << "  Area = " << shape.area() << "\n";
}

// === 2. CRTP Mixin: Comparable (auto-generates !=, >, <=, >=) ===
template <typename Derived>
class Comparable {
public:
    bool operator!=(const Derived& other) const {
        return !(self() == other);
    }
    bool operator>(const Derived& other) const {
        return other < self();
    }
    bool operator<=(const Derived& other) const {
        return !(self() > other);
    }
    bool operator>=(const Derived& other) const {
        return !(self() < other);
    }
private:
    const Derived& self() const { return static_cast<const Derived&>(*this); }
};

class Price : public Comparable<Price> {
    double value_;
public:
    explicit Price(double v) : value_(v) {}
    bool operator==(const Price& other) const { return value_ == other.value_; }
    bool operator<(const Price& other) const { return value_ < other.value_; }
    double value() const { return value_; }
};

// === 3. CRTP Mixin: Counter (counts instances) ===
template <typename Derived>
class InstanceCounter {
    static int count_;
public:
    InstanceCounter() { ++count_; }
    InstanceCounter(const InstanceCounter&) { ++count_; }
    ~InstanceCounter() { --count_; }
    static int instanceCount() { return count_; }
};
template <typename T> int InstanceCounter<T>::count_ = 0;

class Order : public InstanceCounter<Order> {
    std::string symbol_;
public:
    explicit Order(std::string sym) : symbol_(std::move(sym)) {}
};

class Trade : public InstanceCounter<Trade> {
    double price_;
public:
    explicit Trade(double p) : price_(p) {}
};

// === 4. Performance comparison: CRTP vs virtual ===
class VirtualShape {
public:
    virtual double area() const = 0;
    virtual ~VirtualShape() = default;
};

class VirtualCircle : public VirtualShape {
    double r_;
public:
    explicit VirtualCircle(double r) : r_(r) {}
    double area() const override { return 3.14159265 * r_ * r_; }
};

template <typename ShapeT>
double sumAreasCRTP(const std::vector<ShapeT>& shapes) {
    double total = 0;
    for (const auto& s : shapes) total += s.area();
    return total;
}

int main() {
    std::cout << "=== 1. Static polymorphism ===\n";
    Circle c(5.0);
    Square s(4.0);
    c.draw();
    s.draw();
    printArea(c);
    printArea(s);
    assert(c.area() > 78.0 && c.area() < 79.0);
    assert(s.area() == 16.0);

    std::cout << "\n=== 2. Comparable mixin ===\n";
    Price p1(100.50), p2(101.25), p3(100.50);
    assert(p1 == p3);
    assert(p1 != p2);
    assert(p1 < p2);
    assert(p2 > p1);
    assert(p1 <= p3);
    assert(p2 >= p1);
    std::cout << "  All comparison operators from just == and <\n";

    std::cout << "\n=== 3. Instance counter mixin ===\n";
    {
        Order o1("AAPL"), o2("GOOGL"), o3("MSFT");
        Trade t1(150.0), t2(140.0);
        assert(Order::instanceCount() == 3);
        assert(Trade::instanceCount() == 2);
        std::cout << "  Orders: " << Order::instanceCount() << "\n";
        std::cout << "  Trades: " << Trade::instanceCount() << "\n";
    }
    assert(Order::instanceCount() == 0);
    assert(Trade::instanceCount() == 0);
    std::cout << "  After scope: Orders=" << Order::instanceCount()
              << " Trades=" << Trade::instanceCount() << "\n";

    std::cout << "\n=== 4. Performance: CRTP vs virtual ===\n";
    constexpr int N = 1000000;
    std::vector<Circle> circles(N, Circle(3.0));
    std::vector<std::unique_ptr<VirtualShape>> vshapes;
    vshapes.reserve(N);
    for (int i = 0; i < N; ++i) vshapes.push_back(std::make_unique<VirtualCircle>(3.0));

    auto t1 = std::chrono::high_resolution_clock::now();
    double crtp_sum = sumAreasCRTP(circles);
    auto t2 = std::chrono::high_resolution_clock::now();
    double virt_sum = 0;
    for (const auto& sh : vshapes) virt_sum += sh->area();
    auto t3 = std::chrono::high_resolution_clock::now();

    auto crtp_us = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto virt_us = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2).count();
    std::cout << "  CRTP sum: " << crtp_sum << " in " << crtp_us << " us\n";
    std::cout << "  Virtual sum: " << virt_sum << " in " << virt_us << " us\n";
    std::cout << "  (CRTP enables inlining -> vectorization -> faster)\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
