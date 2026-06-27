/**************************************************************************************************
 * Topic: Small Buffer Optimization (SBO) in Type Erasure
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★★ (HFT, Performance-Critical Systems)
 * What interviewers look for:
 *   - Heap allocation avoidance for small objects
 *   - Placement new / std::construct_at usage
 *   - Template capacity/alignment parameters
 *   - Same technique used in std::function, std::any, folly::Function
 *
 * Day-to-day application:
 *   - Custom std::function implementations (avoid malloc in hot paths)
 *   - Any type-erased container in latency-sensitive code
 *   - Game engines: polymorphic components without heap allocation
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o small_buffer_optimization small_buffer_optimization.cpp
 **************************************************************************************************/

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

// ============================================================================
// Concrete shape types (plain value types, no inheritance)
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
// SBO-based Type Erasure: Shape lives entirely on the stack
// ============================================================================

template <std::size_t Capacity = 64U, std::size_t Alignment = alignof(void*)>
class Shape {
public:
    // Constructor: stores any shape + strategy in the internal buffer
    template <typename ShapeT, typename DrawStrategy>
    Shape(ShapeT shape, DrawStrategy drawer) {
        using Model = OwningModel<ShapeT, DrawStrategy>;
        static_assert(sizeof(Model) <= Capacity,
                      "Type too large for SBO buffer. Increase Capacity.");
        static_assert(alignof(Model) <= Alignment,
                      "Type has stricter alignment than buffer.");

        std::construct_at(static_cast<Model*>(pimpl()),
                          std::move(shape), std::move(drawer));
    }

    // Copy delegates to virtual clone-in-place
    Shape(Shape const& other) { other.pimpl()->clone(pimpl()); }
    Shape& operator=(Shape const& other) {
        Shape copy(other);
        buffer_.swap(copy.buffer_);
        return *this;
    }

    // Move delegates to virtual move-in-place
    Shape(Shape&& other) noexcept { other.pimpl()->move(pimpl()); }
    Shape& operator=(Shape&& other) noexcept {
        Shape copy(std::move(other));
        buffer_.swap(copy.buffer_);
        return *this;
    }

    ~Shape() { std::destroy_at(pimpl()); }

private:
    // Hidden friend for the public draw interface
    friend void draw(Shape const& shape, std::ostream& os) {
        shape.pimpl()->draw(os);
    }

    // Internal concept (virtual interface)
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw(std::ostream& os) const = 0;
        virtual void clone(Concept* memory) const = 0;  // Prototype in-place
        virtual void move(Concept* memory) = 0;
    };

    // Internal model (stores concrete shape + strategy)
    template <typename ShapeT, typename DrawStrategy>
    struct OwningModel : Concept {
        OwningModel(ShapeT shape, DrawStrategy drawer)
            : shape_(std::move(shape)), drawer_(std::move(drawer)) {}

        void draw(std::ostream& os) const override { drawer_(shape_, os); }

        void clone(Concept* memory) const override {
            std::construct_at(static_cast<OwningModel*>(memory), *this);
        }
        void move(Concept* memory) override {
            std::construct_at(static_cast<OwningModel*>(memory), std::move(*this));
        }

        ShapeT shape_;
        DrawStrategy drawer_;
    };

    Concept* pimpl() { return reinterpret_cast<Concept*>(buffer_.data()); }
    Concept const* pimpl() const {
        return reinterpret_cast<Concept const*>(buffer_.data());
    }

    alignas(Alignment) std::array<std::byte, Capacity> buffer_;
};

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    // Zero heap allocations!
    Shape shape1(
        Circle{2.5},
        [](Circle const& c, std::ostream& os) {
            os << "Circle(r=" << c.radius() << ")";
        });

    Shape shape2(
        Square{4.0},
        [](Square const& s, std::ostream& os) {
            os << "Square(side=" << s.side() << ")";
        });

    std::cout << "=== Small Buffer Optimization Type Erasure ===\n";
    draw(shape1, std::cout);
    std::cout << '\n';
    draw(shape2, std::cout);
    std::cout << '\n';

    // Test copy (deep copy, also on stack)
    Shape shape3 = shape1;
    std::ostringstream oss;
    draw(shape3, oss);
    assert(oss.str() == "Circle(r=2.5)");

    // Test move
    Shape shape4 = std::move(shape2);
    oss.str("");
    draw(shape4, oss);
    assert(oss.str() == "Square(side=4)");

    std::cout << "\nAll assertions passed. Zero heap allocations!\n";
    return EXIT_SUCCESS;
}
