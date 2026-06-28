// factory.cpp - Factory Method Pattern
// Interview Relevance: Tests understanding of polymorphic object creation, ownership
// semantics with unique_ptr, and Open/Closed principle. Common in systems where new
// types are added frequently without modifying existing code.
// Compile: g++ -std=c++20 -Wall factory.cpp -o factory

#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <cmath>
#include <numbers>

class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
    double radius_;
public:
    explicit Circle(double r) : radius_(r) {}
    double area() const override { return std::numbers::pi * radius_ * radius_; }
    std::string name() const override { return "Circle(r=" + std::to_string(radius_) + ")"; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rectangle(" + std::to_string(w_) + "x" + std::to_string(h_) + ")"; }
};

class Triangle : public Shape {
    double base_, height_;
public:
    Triangle(double b, double h) : base_(b), height_(h) {}
    double area() const override { return 0.5 * base_ * height_; }
    std::string name() const override { return "Triangle(b=" + std::to_string(base_) + ")"; }
};

// Factory Method - client doesn't know concrete types
class ShapeFactory {
public:
    enum class Type { Circle, Rectangle, Triangle };

    static std::unique_ptr<Shape> create(Type type, double a = 1.0, double b = 1.0) {
        switch (type) {
            case Type::Circle:    return std::make_unique<Circle>(a);
            case Type::Rectangle: return std::make_unique<Rectangle>(a, b);
            case Type::Triangle:  return std::make_unique<Triangle>(a, b);
        }
        return nullptr;  // Unreachable with enum class
    }
};

int main() {
    // Client code works with Shape* - doesn't know concrete types
    auto circle = ShapeFactory::create(ShapeFactory::Type::Circle, 5.0);
    auto rect   = ShapeFactory::create(ShapeFactory::Type::Rectangle, 3.0, 4.0);
    auto tri    = ShapeFactory::create(ShapeFactory::Type::Triangle, 6.0, 3.0);

    std::cout << circle->name() << " area = " << circle->area() << "\n";
    std::cout << rect->name()   << " area = " << rect->area() << "\n";
    std::cout << tri->name()    << " area = " << tri->area() << "\n";

    assert(std::abs(rect->area() - 12.0) < 1e-9);
    assert(std::abs(tri->area() - 9.0) < 1e-9);

    std::cout << "\nKey insight: Adding a new shape (Pentagon) requires NO changes\n";
    std::cout << "to client code - only extend factory and add new class.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
