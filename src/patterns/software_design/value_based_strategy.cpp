/**************************************************************************************************
 * Topic: Value-Based Strategy Pattern std::function for Runtime Polymorphism
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★★
 * What interviewers look for:
 *   - std::function vs virtual interface for strategy injection
 *   - Value semantics: strategies are copyable, no raw pointer lifetime issues
 *   - Trade-off: flexibility (any callable) vs overhead (heap allocation in std::function)
 *   - Real-world: sort comparators, event handlers, middleware pipelines
 *
 * Day-to-day application:
 *   - Configurable logging (swap log strategy per environment)
 *   - Pluggable validators in form pipelines
 *   - HTTP middleware stacks
 *   - Payment processing with swappable gateways
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o value_based_strategy value_based_strategy.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// ============================================================================
// Shape hierarchy with value-based strategy (std::function)
// ============================================================================

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw(std::ostream& os) const = 0;
};

class Circle : public Shape {
public:
    using DrawStrategy = std::function<void(Circle const&, std::ostream&)>;

    Circle(double radius, DrawStrategy drawer)
        : radius_(radius), drawer_(std::move(drawer)) {
        if (!drawer_) throw std::invalid_argument("drawer must not be empty");
    }

    void draw(std::ostream& os) const override { drawer_(*this, os); }
    double radius() const { return radius_; }

private:
    double radius_;
    DrawStrategy drawer_;
};

class Square : public Shape {
public:
    using DrawStrategy = std::function<void(Square const&, std::ostream&)>;

    Square(double side, DrawStrategy drawer)
        : side_(side), drawer_(std::move(drawer)) {
        if (!drawer_) throw std::invalid_argument("drawer must not be empty");
    }

    void draw(std::ostream& os) const override { drawer_(*this, os); }
    double side() const { return side_; }

private:
    double side_;
    DrawStrategy drawer_;
};

// ============================================================================
// Draw strategies any callable works (lambdas, function objects, free functions)
// ============================================================================

// Function object strategy
struct SVGCircleDrawer {
    void operator()(Circle const& c, std::ostream& os) const {
        os << "<circle r=\"" << c.radius() << "\" />";
    }
};

// Lambda factory for text drawing
auto make_text_drawer() {
    return [](auto const& shape, std::ostream& os) {
        if constexpr (std::is_same_v<std::decay_t<decltype(shape)>, Circle>) {
            os << "Drawing circle with radius " << shape.radius();
        } else {
            os << "Drawing square with side " << shape.side();
        }
    };
}

// ============================================================================
// Generic draw-all works with any collection of shapes
// ============================================================================

void draw_all(std::vector<std::unique_ptr<Shape>> const& shapes, std::ostream& os) {
    for (auto const& shape : shapes) {
        shape->draw(os);
        os << '\n';
    }
}

// ============================================================================
// main
// ============================================================================

int main() {
    using Shapes = std::vector<std::unique_ptr<Shape>>;
    Shapes shapes;

    // Lambda strategy
    shapes.push_back(std::make_unique<Circle>(
        3.0,
        [](Circle const& c, std::ostream& os) {
            os << "Circle(r=" << c.radius() << ")";
        }));

    // Function object strategy
    shapes.push_back(std::make_unique<Circle>(5.0, SVGCircleDrawer{}));

    // Generic text drawer
    shapes.push_back(std::make_unique<Square>(4.0, make_text_drawer()));

    std::cout << "=== Value-Based Strategy Pattern ===\n";
    draw_all(shapes, std::cout);

    // Verify SVG output
    std::ostringstream oss;
    shapes[1]->draw(oss);
    assert(oss.str() == "<circle r=\"5\" />");

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
