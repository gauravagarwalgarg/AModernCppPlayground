/**************************************************************************************************
 * Topic: Pimpl Idiom (Pointer to Implementation) The Bridge Pattern in C++
 * Source: Adapted from "C++ Software Design" by Klaus Iglberger (O'Reilly 2022)
 *
 * Interview Relevance: ★★★★★ (Any C++ role)
 * What interviewers look for:
 *   - ABI stability: adding private members doesn't break binary compatibility
 *   - Compilation firewall: changes to impl don't require recompiling clients
 *   - Correct Rule of Five with unique_ptr to incomplete type
 *   - Performance trade-off: indirection cost vs compilation speedup
 *
 * Day-to-day application:
 *   - Library interfaces (Qt uses Pimpl extensively)
 *   - Large codebases: reduce #include chains and build times
 *   - Platform abstraction layers
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o pimpl_idiom pimpl_idiom.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

// ============================================================================
// Widget.h The public header (what clients see)
// ============================================================================

class Widget {
public:
    Widget(std::string name, int value);
    ~Widget();  // Must be declared here, defined where Impl is complete

    // Rule of Five: must declare all since unique_ptr<Impl> is non-copyable
    Widget(Widget const& other);
    Widget& operator=(Widget const& other);
    Widget(Widget&& other) noexcept;
    Widget& operator=(Widget&& other) noexcept;

    // Public interface implementation hidden behind the pointer
    std::string const& name() const;
    int value() const;
    void set_value(int v);
    void print() const;

private:
    struct Impl;  // Forward declaration only!
    std::unique_ptr<Impl> pimpl_;
};

// ============================================================================
// Widget.cpp The implementation (only this file knows about Impl)
// ============================================================================

// In a real project, this would be in a separate .cpp file
struct Widget::Impl {
    std::string name;
    int value;
    int internal_counter{0};  // Adding this field doesn't affect ABI!

    Impl(std::string n, int v) : name(std::move(n)), value(v) {}
};

Widget::Widget(std::string name, int value)
    : pimpl_(std::make_unique<Impl>(std::move(name), value)) {}

Widget::~Widget() = default;  // Defined here where Impl is complete

Widget::Widget(Widget const& other)
    : pimpl_(std::make_unique<Impl>(*other.pimpl_)) {}

Widget& Widget::operator=(Widget const& other) {
    *pimpl_ = *other.pimpl_;
    return *this;
}

Widget::Widget(Widget&& other) noexcept = default;
Widget& Widget::operator=(Widget&& other) noexcept = default;

std::string const& Widget::name() const { return pimpl_->name; }
int Widget::value() const { return pimpl_->value; }

void Widget::set_value(int v) {
    pimpl_->value = v;
    pimpl_->internal_counter++;
}

void Widget::print() const {
    std::cout << "Widget{name=\"" << pimpl_->name
              << "\", value=" << pimpl_->value << "}\n";
}

// ============================================================================
// Client code
// ============================================================================

int main() {
    Widget w1("alpha", 42);
    w1.print();

    // Copy
    Widget w2 = w1;
    w2.set_value(100);
    assert(w1.value() == 42);   // Original unchanged
    assert(w2.value() == 100);  // Copy modified independently

    // Move
    Widget w3 = std::move(w1);
    assert(w3.name() == "alpha");
    assert(w3.value() == 42);

    // Assignment
    w2 = w3;
    assert(w2.value() == 42);

    w3.print();

    std::cout << "All assertions passed.\n";
    return EXIT_SUCCESS;
}
