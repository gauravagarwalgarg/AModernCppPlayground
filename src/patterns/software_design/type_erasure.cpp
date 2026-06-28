/**************************************************************************************************
 * Topic: Type Erasure The Core Pattern of Modern C++ Software Design
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 * 
 * Interview Relevance: ★★★★★ (HFT, FAANG, Systems)
 * What interviewers look for:
 *   - Understanding of value semantics vs reference semantics
 *   - Combining External Polymorphism + Bridge + Prototype patterns
 *   - How type erasure eliminates inheritance hierarchies
 *   - Real-world usage: std::function, std::any, std::move_only_function
 *
 * Key Concepts:
 *   1. External Polymorphism: Concept/Model separation (virtual interface hidden inside)
 *   2. Bridge Pattern: unique_ptr<Concept> provides ABI stability and decoupling
 *   3. Prototype Pattern: clone() enables value semantics (copyable polymorphism)
 *   4. Open for extension: New shapes added WITHOUT modifying existing code
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o type_erasure type_erasure.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// ============================================================================
// Concrete types NO inheritance, NO virtual functions, plain value types
// ============================================================================

class Circle {
public:
    explicit Circle(double radius) : radius_(radius) {}
    double radius() const { return radius_; }

private:
    double radius_;
};

class Square {
public:
    explicit Square(double side) : side_(side) {}
    double side() const { return side_; }

private:
    double side_;
};

// ============================================================================
// Type-Erased Shape Polymorphism WITHOUT requiring shapes to inherit anything
// ============================================================================

namespace detail {

// The "Concept" hidden virtual interface (External Polymorphism)
class ShapeConcept {
public:
    virtual ~ShapeConcept() = default;
    virtual void draw(std::ostream& os) const = 0;
    virtual std::unique_ptr<ShapeConcept> clone() const = 0;  // Prototype
};

// The "Model" bridges any shape type + draw strategy into the concept
template <typename ShapeT, typename DrawStrategy>
class OwningShapeModel : public ShapeConcept {
public:
    OwningShapeModel(ShapeT shape, DrawStrategy drawer)
        : shape_{std::move(shape)}, drawer_{std::move(drawer)} {}

    void draw(std::ostream& os) const override { drawer_(shape_, os); }

    std::unique_ptr<ShapeConcept> clone() const override {
        return std::make_unique<OwningShapeModel>(*this);
    }

private:
    ShapeT shape_;
    DrawStrategy drawer_;
};

}  // namespace detail

// The value-semantic wrapper copyable, movable, polymorphic
class Shape {
public:
    // Constructor: accepts ANY shape + ANY drawing strategy (no inheritance needed)
    template <typename ShapeT, typename DrawStrategy>
    Shape(ShapeT shape, DrawStrategy drawer) {
        using Model = detail::OwningShapeModel<ShapeT, DrawStrategy>;
        pimpl_ = std::make_unique<Model>(std::move(shape), std::move(drawer));
    }

    // Copy via Prototype (deep copy)
    Shape(Shape const& other) : pimpl_(other.pimpl_->clone()) {}
    Shape& operator=(Shape const& other) {
        Shape copy(other);
        pimpl_.swap(copy.pimpl_);
        return *this;
    }

    // Move = default (unique_ptr handles it)
    Shape(Shape&&) = default;
    Shape& operator=(Shape&&) = default;
    ~Shape() = default;

private:
    // Hidden friend the public interface
    friend void draw(Shape const& shape, std::ostream& os) {
        shape.pimpl_->draw(os);
    }

    std::unique_ptr<detail::ShapeConcept> pimpl_;  // Bridge
};

// ============================================================================
// Usage shapes are value types, stored in vector by value, fully copyable
// ============================================================================

int main() {
    std::vector<Shape> shapes;

    // Add shapes with their draw strategies (lambdas)
    shapes.emplace_back(
        Circle{3.14},
        [](Circle const& c, std::ostream& os) {
            os << "Circle(r=" << c.radius() << ")";
        });

    shapes.emplace_back(
        Square{2.0},
        [](Square const& s, std::ostream& os) {
            os << "Square(side=" << s.side() << ")";
        });

    // Draw all shapes
    std::cout << "=== Drawing shapes ===\n";
    for (auto const& shape : shapes) {
        draw(shape, std::cout);
        std::cout << '\n';
    }

    // Copy the entire collection (deep copy via Prototype)
    auto shapes_copy = shapes;
    std::cout << "\n=== Drawing copied shapes ===\n";
    for (auto const& shape : shapes_copy) {
        draw(shape, std::cout);
        std::cout << '\n';
    }

    // Verify via string
    std::ostringstream oss;
    draw(shapes[0], oss);
    assert(oss.str() == "Circle(r=3.14)");

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
