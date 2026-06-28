/*
 * Copy Elision: RVO, NRVO, and Guaranteed Copy Elision (C++17)
 *
 * WHAT: Copy elision allows the compiler to skip copy/move constructors by
 * constructing objects directly in the target location. C++17 guarantees
 * elision for prvalues (temporary materialization optimization).
 *
 * WHY IT MATTERS IN INTERVIEWS: Understanding copy elision explains why
 * returning by value is efficient in modern C++. "Return big objects by value"
 * is the correct advice copy elision makes it zero-cost.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - RVO (Return Value Optimization): unnamed temporaries
 * - NRVO (Named RVO): named local variables
 * - Guaranteed copy elision (C++17): prvalue semantics
 * - When elision CANNOT happen (conditions, multiple returns)
 * - How to write functions that benefit from elision
 *
 * COMMON PITFALLS:
 * - NRVO is not guaranteed (compiler may choose not to)
 * - Multiple return paths can prevent NRVO
 * - std::move on return statement PREVENTS elision!
 * - Relying on side effects in copy/move constructors
 */

#include <iostream>
#include <vector>
#include <string>
#include <cassert>

class Widget {
    std::string name_;
    std::vector<int> data_;
    static int constructions_;
    static int copies_;
    static int moves_;

public:
    explicit Widget(std::string name, size_t size = 100)
        : name_(std::move(name)), data_(size, 42) {
        ++constructions_;
        std::cout << "  [Ctor] " << name_ << "\n";
    }

    Widget(const Widget& other) : name_(other.name_ + "_copy"), data_(other.data_) {
        ++copies_;
        std::cout << "  [Copy] " << name_ << "\n";
    }

    Widget(Widget&& other) noexcept
        : name_(std::move(other.name_)), data_(std::move(other.data_)) {
        ++moves_;
        std::cout << "  [Move] " << name_ << "\n";
    }

    Widget& operator=(const Widget&) = default;
    Widget& operator=(Widget&&) noexcept = default;

    const std::string& name() const { return name_; }
    size_t size() const { return data_.size(); }

    static void resetCounters() { constructions_ = copies_ = moves_ = 0; }
    static void report(const char* label) {
        std::cout << "  -> " << label << ": ctors=" << constructions_
                  << " copies=" << copies_ << " moves=" << moves_ << "\n";
        resetCounters();
    }
};
int Widget::constructions_ = 0;
int Widget::copies_ = 0;
int Widget::moves_ = 0;

// === 1. RVO: Return unnamed temporary (guaranteed in C++17) ===
Widget makeWidgetRVO() {
    return Widget("rvo_widget");
}

// === 2. NRVO: Return named local variable (not guaranteed but usually done) ===
Widget makeWidgetNRVO() {
    Widget w("nrvo_widget");
    return w;
}

// === 3. Elision PREVENTED: multiple return paths ===
Widget makeWidgetConditional(bool flag) {
    Widget a("path_a");
    Widget b("path_b");
    if (flag) return a;
    return b;
}

// === 4. PITFALL: std::move PREVENTS copy elision ===
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpessimizing-move"
Widget makeWidgetBad() {
    Widget w("dont_move_me");
    return std::move(w); // BAD! Prevents NRVO, forces move
}
#pragma GCC diagnostic pop

Widget makeWidgetGood() {
    Widget w("let_me_elide");
    return w; // GOOD! Compiler can apply NRVO
}

// === 5. Guaranteed copy elision (C++17) ===
Widget createAndReturn() {
    return Widget("guaranteed");
}

int main() {
    std::cout << "=== 1. RVO (Return Value Optimization) ===\n";
    Widget::resetCounters();
    Widget w1 = makeWidgetRVO();
    Widget::report("RVO");
    assert(w1.size() == 100);

    std::cout << "\n=== 2. NRVO (Named Return Value Optimization) ===\n";
    Widget::resetCounters();
    Widget w2 = makeWidgetNRVO();
    Widget::report("NRVO");

    std::cout << "\n=== 3. Conditional return (elision may be prevented) ===\n";
    Widget::resetCounters();
    Widget w3 = makeWidgetConditional(true);
    Widget::report("Conditional");

    std::cout << "\n=== 4. PITFALL: std::move prevents elision ===\n";
    Widget::resetCounters();
    Widget w4 = makeWidgetBad();
    Widget::report("Bad (std::move)");

    Widget::resetCounters();
    Widget w5 = makeWidgetGood();
    Widget::report("Good (no move)");

    std::cout << "\n=== 5. Guaranteed copy elision (C++17) ===\n";
    Widget::resetCounters();
    Widget w6 = createAndReturn();
    Widget::report("Guaranteed");

    Widget::resetCounters();
    Widget w7 = Widget("direct_init");
    Widget::report("Direct prvalue");

    std::cout << "\n=== Guidelines ===\n";
    std::cout << "  1. Return by value (not pointer/reference)\n";
    std::cout << "  2. Don't use std::move on return statements\n";
    std::cout << "  3. Single return path helps NRVO\n";
    std::cout << "  4. Return unnamed temporaries for guaranteed elision\n";
    std::cout << "  5. Trust the compiler value semantics are cheap!\n";

    // Suppress unused warnings
    (void)w1; (void)w2; (void)w3; (void)w4; (void)w5; (void)w6; (void)w7;

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
