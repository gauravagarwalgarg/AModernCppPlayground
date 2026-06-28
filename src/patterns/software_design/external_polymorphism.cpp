/**************************************************************************************************
 * Topic: External Polymorphism Non-Intrusive Runtime Polymorphism
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★★ (HFT, FAANG, Systems)
 * What interviewers look for:
 *   - Adding polymorphic behavior WITHOUT modifying existing classes
 *   - Separation of concerns: shape data vs drawing behavior
 *   - Template-based model stores both type and strategy
 *   - When to prefer this over classical inheritance hierarchies
 *
 * Day-to-day application:
 *   - Plugin systems where you can't modify third-party types
 *   - Serialization frameworks (serialize any type without inheritance)
 *   - Logging/formatting external types
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o external_polymorphism external_polymorphism.cpp
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
// Concrete types completely unaware of any polymorphic interface
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

// A third-party type you can't modify
class Triangle {
public:
    Triangle(double base, double height) : base_(base), height_(height) {}
    double base() const { return base_; }
    double height() const { return height_; }
private:
    double base_;
    double height_;
};

// ============================================================================
// External Polymorphism: Concept + Model
// ============================================================================

// The concept defines what operations are polymorphic
class ShapeConcept {
public:
    virtual ~ShapeConcept() = default;
    virtual void draw(std::ostream& os) const = 0;
    virtual double area() const = 0;
};

// The model adapts any (ShapeT + Strategy) pair to the concept
template <typename ShapeT, typename DrawStrategy, typename AreaStrategy>
class ShapeModel : public ShapeConcept {
public:
    ShapeModel(ShapeT shape, DrawStrategy drawer, AreaStrategy area_calc)
        : shape_{std::move(shape)}
        , drawer_{std::move(drawer)}
        , area_calc_{std::move(area_calc)} {}

    void draw(std::ostream& os) const override { drawer_(shape_, os); }
    double area() const override { return area_calc_(shape_); }

private:
    ShapeT shape_;
    DrawStrategy drawer_;
    AreaStrategy area_calc_;
};

// ============================================================================
// Convenience factory type-deduced make function
// ============================================================================

template <typename ShapeT, typename DrawStrategy, typename AreaStrategy>
auto make_shape(ShapeT shape, DrawStrategy drawer, AreaStrategy area) {
    using Model = ShapeModel<ShapeT, DrawStrategy, AreaStrategy>;
    return std::make_unique<Model>(std::move(shape), std::move(drawer), std::move(area));
}

// ============================================================================
// Usage
// ============================================================================

int main() {
    using Shapes = std::vector<std::unique_ptr<ShapeConcept>>;

    Shapes shapes;

    // Circle with its own draw and area strategies
    shapes.push_back(make_shape(
        Circle{5.0},
        [](Circle const& c, std::ostream& os) {
            os << "Circle(r=" << c.radius() << ")";
        },
        [](Circle const& c) { return 3.14159265 * c.radius() * c.radius(); }
    ));

    // Square
    shapes.push_back(make_shape(
        Square{4.0},
        [](Square const& s, std::ostream& os) {
            os << "Square(side=" << s.side() << ")";
        },
        [](Square const& s) { return s.side() * s.side(); }
    ));

    // Third-party Triangle no modification needed!
    shapes.push_back(make_shape(
        Triangle{6.0, 3.0},
        [](Triangle const& t, std::ostream& os) {
            os << "Triangle(base=" << t.base() << ", h=" << t.height() << ")";
        },
        [](Triangle const& t) { return 0.5 * t.base() * t.height(); }
    ));

    // Polymorphic operations
    std::cout << "=== Shapes via External Polymorphism ===\n";
    for (auto const& shape : shapes) {
        std::ostringstream oss;
        shape->draw(oss);
        std::cout << oss.str() << " | area = " << shape->area() << '\n';
    }

    // Assertions
    assert(shapes[0]->area() > 78.0 && shapes[0]->area() < 79.0);  // pi*25
    assert(shapes[1]->area() == 16.0);                               // 4^2
    assert(shapes[2]->area() == 9.0);                                // 0.5*6*3

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
