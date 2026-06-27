// visitor.cpp - Visitor Pattern (AST with Double Dispatch)
// Interview Relevance: Demonstrates double dispatch to select method based on BOTH
// visitor type and element type. Essential for compilers (AST traversal), serializers,
// and adding operations without modifying element classes.
// Compile: g++ -std=c++20 -Wall visitor.cpp -o visitor

#include <iostream>
#include <memory>
#include <string>
#include <cassert>
#include <cmath>

// Forward declarations for visitor interface
class NumberExpr;
class AddExpr;
class MultiplyExpr;

class ExprVisitor {
public:
    virtual ~ExprVisitor() = default;
    virtual void visit(const NumberExpr& expr) = 0;
    virtual void visit(const AddExpr& expr) = 0;
    virtual void visit(const MultiplyExpr& expr) = 0;
};

// Expression hierarchy
class Expression {
public:
    virtual ~Expression() = default;
    virtual void accept(ExprVisitor& visitor) const = 0;  // Double dispatch entry
};

class NumberExpr : public Expression {
    double value_;
public:
    explicit NumberExpr(double v) : value_(v) {}
    double value() const { return value_; }
    void accept(ExprVisitor& v) const override { v.visit(*this); }
};

class AddExpr : public Expression {
    std::unique_ptr<Expression> left_, right_;
public:
    AddExpr(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left_(std::move(l)), right_(std::move(r)) {}
    const Expression& left() const { return *left_; }
    const Expression& right() const { return *right_; }
    void accept(ExprVisitor& v) const override { v.visit(*this); }
};

class MultiplyExpr : public Expression {
    std::unique_ptr<Expression> left_, right_;
public:
    MultiplyExpr(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r)
        : left_(std::move(l)), right_(std::move(r)) {}
    const Expression& left() const { return *left_; }
    const Expression& right() const { return *right_; }
    void accept(ExprVisitor& v) const override { v.visit(*this); }
};

// Visitor 1: Evaluates the expression tree
class EvalVisitor : public ExprVisitor {
    double result_ = 0;
public:
    void visit(const NumberExpr& expr) override { result_ = expr.value(); }
    void visit(const AddExpr& expr) override {
        expr.left().accept(*this); double l = result_;
        expr.right().accept(*this); double r = result_;
        result_ = l + r;
    }
    void visit(const MultiplyExpr& expr) override {
        expr.left().accept(*this); double l = result_;
        expr.right().accept(*this); double r = result_;
        result_ = l * r;
    }
    double result() const { return result_; }
};

// Visitor 2: Prints the expression as a string
class PrintVisitor : public ExprVisitor {
    std::string output_;
public:
    void visit(const NumberExpr& expr) override {
        output_ += std::to_string(static_cast<int>(expr.value()));
    }
    void visit(const AddExpr& expr) override {
        output_ += "(";
        expr.left().accept(*this);
        output_ += " + ";
        expr.right().accept(*this);
        output_ += ")";
    }
    void visit(const MultiplyExpr& expr) override {
        output_ += "(";
        expr.left().accept(*this);
        output_ += " * ";
        expr.right().accept(*this);
        output_ += ")";
    }
    const std::string& str() const { return output_; }
};

int main() {
    // Build AST: (3 + 4) * 2
    auto expr = std::make_unique<MultiplyExpr>(
        std::make_unique<AddExpr>(
            std::make_unique<NumberExpr>(3),
            std::make_unique<NumberExpr>(4)),
        std::make_unique<NumberExpr>(2));

    // Apply Eval visitor
    EvalVisitor eval;
    expr->accept(eval);
    std::cout << "Result: " << eval.result() << "\n";
    assert(std::abs(eval.result() - 14.0) < 1e-9);

    // Apply Print visitor - same tree, different operation
    PrintVisitor printer;
    expr->accept(printer);
    std::cout << "Expression: " << printer.str() << "\n";
    assert(printer.str() == "((3 + 4) * 2)");

    // Another expression: 5 + (2 * 3)
    auto expr2 = std::make_unique<AddExpr>(
        std::make_unique<NumberExpr>(5),
        std::make_unique<MultiplyExpr>(
            std::make_unique<NumberExpr>(2),
            std::make_unique<NumberExpr>(3)));

    EvalVisitor eval2;
    expr2->accept(eval2);
    std::cout << "5 + (2*3) = " << eval2.result() << "\n";
    assert(std::abs(eval2.result() - 11.0) < 1e-9);

    std::cout << "\nKey insight: Double dispatch - accept() dispatches on element type,\n";
    std::cout << "then visit() dispatches on visitor type. New operations (visitors)\n";
    std::cout << "don't require modifying the expression classes.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
