// adapter.cpp - Adapter Pattern (Object Adapter via Composition)
// Interview Relevance: Converts incompatible interface to expected one. Tests
// understanding of composition over inheritance and interface compatibility.
// Real use: wrapping third-party libraries, legacy system integration.
// Compile: g++ -std=c++20 -Wall adapter.cpp -o adapter

#include <iostream>
#include <cassert>
#include <cmath>

// Modern interface that our system expects
class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual double x() const = 0;
    virtual double y() const = 0;
    virtual double width() const = 0;
    virtual double height() const = 0;
};

// Legacy class we can't modify (third-party library)
class LegacyRectangle {
    double x1_, y1_, x2_, y2_;
public:
    LegacyRectangle(double x1, double y1, double x2, double y2)
        : x1_(x1), y1_(y1), x2_(x2), y2_(y2) {}
    void oldDraw() const {
        std::cout << "LegacyRectangle::oldDraw(" << x1_ << "," << y1_
                  << " -> " << x2_ << "," << y2_ << ")\n";
    }
    double getX1() const { return x1_; }
    double getY1() const { return y1_; }
    double getX2() const { return x2_; }
    double getY2() const { return y2_; }
};

// Object Adapter - uses composition (preferred over class adapter/multiple inheritance)
class RectangleAdapter : public Shape {
    LegacyRectangle legacy_;
public:
    RectangleAdapter(double x, double y, double w, double h)
        : legacy_(x, y, x + w, y + h) {}

    void draw() const override { legacy_.oldDraw(); }
    double x() const override { return legacy_.getX1(); }
    double y() const override { return legacy_.getY1(); }
    double width() const override { return legacy_.getX2() - legacy_.getX1(); }
    double height() const override { return legacy_.getY2() - legacy_.getY1(); }
};

int main() {
    // Client code uses modern Shape interface
    RectangleAdapter rect(10, 20, 100, 50);

    std::cout << "Using adapted interface:\n";
    rect.draw();
    std::cout << "  Position: (" << rect.x() << ", " << rect.y() << ")\n";
    std::cout << "  Size: " << rect.width() << " x " << rect.height() << "\n";

    assert(std::abs(rect.x() - 10.0) < 1e-9);
    assert(std::abs(rect.y() - 20.0) < 1e-9);
    assert(std::abs(rect.width() - 100.0) < 1e-9);
    assert(std::abs(rect.height() - 50.0) < 1e-9);

    std::cout << "\nKey insight: Adapter wraps the legacy object via composition.\n";
    std::cout << "Client code never sees LegacyRectangle - it only knows Shape.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
