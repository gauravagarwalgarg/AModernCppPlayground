// solid_principles.cpp - All 5 SOLID Principles: Violation then Fix
// Interview Relevance: SOLID is THE most frequently asked OOP design topic.
// Each principle shown with a commented violation followed by the correct approach.
// Demonstrates real trade-offs and when each principle applies.
// Compile: g++ -std=c++20 -Wall solid_principles.cpp -o solid_principles

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <cmath>
#include <numbers>

// ============================================================================
// 1. SRP - Single Responsibility Principle
//    "A class should have only one reason to change"
// ============================================================================

namespace SRP {

// VIOLATION: UserManager does authentication AND persistence
// struct UserManager {
//     bool authenticate(const std::string& user, const std::string& pass) { ... }
//     void saveToDatabase(const User& u) { ... }  // Two reasons to change!
//     void loadFromDatabase(int id) { ... }
// };

// FIX: Split into focused classes
struct User {
    std::string name;
    std::string passwordHash;
};

class UserAuth {
public:
    bool authenticate(const std::string& user, const std::string& pass) {
        // Single responsibility: authentication logic only
        return user == "admin" && pass == "secret";
    }

    std::string hashPassword(const std::string& pass) {
        // Simple hash for demo
        size_t h = std::hash<std::string>{}(pass);
        return std::to_string(h);
    }
};

class UserRepository {
    std::vector<User> store_;
public:
    void save(const User& u) {
        store_.push_back(u);  // Single responsibility: persistence only
    }

    const User* findByName(const std::string& name) const {
        for (const auto& u : store_)
            if (u.name == name) return &u;
        return nullptr;
    }

    size_t count() const { return store_.size(); }
};

void demonstrate() {
    std::cout << "=== SRP: Single Responsibility ===\n";
    UserAuth auth;
    UserRepository repo;

    assert(auth.authenticate("admin", "secret"));
    assert(!auth.authenticate("admin", "wrong"));

    repo.save({"Alice", auth.hashPassword("pass123")});
    repo.save({"Bob", auth.hashPassword("pass456")});
    assert(repo.count() == 2);
    assert(repo.findByName("Alice") != nullptr);

    std::cout << "  Auth and persistence are separate concerns ✓\n";
    std::cout << "  Changing DB schema won't affect auth logic ✓\n\n";
}

} // namespace SRP

// ============================================================================
// 2. OCP - Open/Closed Principle
//    "Open for extension, closed for modification"
// ============================================================================

namespace OCP {

// VIOLATION: Adding new shape requires modifying the function
// double area(const std::string& type, double a, double b) {
//     if (type == "circle") return 3.14 * a * a;
//     else if (type == "rect") return a * b;
//     // Adding triangle means MODIFYING this function!
// }

// FIX: Use polymorphism - extend by adding classes, not modifying code
class Shape {
public:
    virtual ~Shape() = default;
    virtual double area() const = 0;
    virtual std::string name() const = 0;
};

class Circle : public Shape {
    double r_;
public:
    explicit Circle(double r) : r_(r) {}
    double area() const override { return std::numbers::pi * r_ * r_; }
    std::string name() const override { return "Circle"; }
};

class Rectangle : public Shape {
    double w_, h_;
public:
    Rectangle(double w, double h) : w_(w), h_(h) {}
    double area() const override { return w_ * h_; }
    std::string name() const override { return "Rectangle"; }
};

// Adding Triangle requires ZERO changes to existing code
class Triangle : public Shape {
    double b_, h_;
public:
    Triangle(double b, double h) : b_(b), h_(h) {}
    double area() const override { return 0.5 * b_ * h_; }
    std::string name() const override { return "Triangle"; }
};

// This function works with ANY shape - never needs modification
double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) {
    double sum = 0;
    for (const auto& s : shapes) sum += s->area();
    return sum;
}

void demonstrate() {
    std::cout << "=== OCP: Open/Closed ===\n";
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(3.0, 4.0));
    shapes.push_back(std::make_unique<Triangle>(6.0, 3.0));

    double total = totalArea(shapes);
    std::cout << "  Total area: " << total << "\n";
    assert(total > 90.0);  // pi*25 + 12 + 9 ≈ 99.5

    std::cout << "  Adding Pentagon = new class, zero existing code changes ✓\n\n";
}

} // namespace OCP

// ============================================================================
// 3. LSP - Liskov Substitution Principle
//    "Subtypes must be substitutable for their base types"
// ============================================================================

namespace LSP {

// VIOLATION: Penguin inherits fly() but can't fly - breaks substitution
// class Bird {
// public:
//     virtual void fly() { std::cout << "Flying!\n"; }
// };
// class Penguin : public Bird {
//     void fly() override { throw std::runtime_error("Can't fly!"); } // BREAKS LSP!
// };

// FIX: Separate flying ability from being a bird
class Bird {
public:
    virtual ~Bird() = default;
    virtual std::string name() const = 0;
    virtual std::string move() const = 0;
};

class FlyingBird : public Bird {
public:
    std::string move() const override { return "flying"; }
};

class Sparrow : public FlyingBird {
public:
    std::string name() const override { return "Sparrow"; }
};

class Eagle : public FlyingBird {
public:
    std::string name() const override { return "Eagle"; }
};

// Penguin IS-A Bird but NOT a FlyingBird
class Penguin : public Bird {
public:
    std::string name() const override { return "Penguin"; }
    std::string move() const override { return "swimming"; }  // No violation!
};

// This function works correctly with ALL birds
void describeBird(const Bird& bird) {
    std::cout << "  " << bird.name() << " moves by " << bird.move() << "\n";
}

void demonstrate() {
    std::cout << "=== LSP: Liskov Substitution ===\n";
    Sparrow sparrow;
    Eagle eagle;
    Penguin penguin;

    describeBird(sparrow);   // Works
    describeBird(eagle);     // Works
    describeBird(penguin);   // Works - no exception, no surprise!

    assert(sparrow.move() == "flying");
    assert(penguin.move() == "swimming");

    std::cout << "  No surprises when substituting subtypes ✓\n";
    std::cout << "  Penguin doesn't pretend to fly ✓\n\n";
}

} // namespace LSP

// ============================================================================
// 4. ISP - Interface Segregation Principle
//    "Clients shouldn't be forced to depend on methods they don't use"
// ============================================================================

namespace ISP {

// VIOLATION: Robot is forced to implement eat() which makes no sense
// class IWorker {
// public:
//     virtual void work() = 0;
//     virtual void eat() = 0;   // Robot doesn't eat!
//     virtual void sleep() = 0; // Robot doesn't sleep!
// };
// class Robot : public IWorker {
//     void work() override { ... }
//     void eat() override { /* nonsensical */ }   // Forced empty implementation!
//     void sleep() override { /* nonsensical */ }
// };

// FIX: Segregate into focused interfaces
class IWorkable {
public:
    virtual ~IWorkable() = default;
    virtual std::string work() = 0;
};

class IEatable {
public:
    virtual ~IEatable() = default;
    virtual std::string eat() = 0;
};

class ISleepable {
public:
    virtual ~ISleepable() = default;
    virtual std::string sleep() = 0;
};

// Human implements all interfaces it needs
class Human : public IWorkable, public IEatable, public ISleepable {
    std::string name_;
public:
    explicit Human(std::string name) : name_(std::move(name)) {}
    std::string work() override { return name_ + " writes code"; }
    std::string eat() override { return name_ + " eats lunch"; }
    std::string sleep() override { return name_ + " sleeps 8h"; }
};

// Robot only implements what makes sense - no forced empty methods
class Robot : public IWorkable {
    std::string id_;
public:
    explicit Robot(std::string id) : id_(std::move(id)) {}
    std::string work() override { return id_ + " assembles parts 24/7"; }
    // No eat() or sleep() - not forced to implement nonsensical methods
};

// Functions depend only on the interface they need
void assignTask(IWorkable& worker) {
    std::cout << "  Task: " << worker.work() << "\n";
}

void scheduleLunch(IEatable& eater) {
    std::cout << "  Lunch: " << eater.eat() << "\n";
}

void demonstrate() {
    std::cout << "=== ISP: Interface Segregation ===\n";
    Human alice("Alice");
    Robot r2d2("R2D2");

    assignTask(alice);   // Human can work
    assignTask(r2d2);    // Robot can work
    scheduleLunch(alice); // Human can eat
    // scheduleLunch(r2d2); // Won't compile - Robot isn't IEatable ✓

    assert(alice.work().find("code") != std::string::npos);
    assert(r2d2.work().find("24/7") != std::string::npos);

    std::cout << "  Robot doesn't implement eat() - compile-time safety ✓\n\n";
}

} // namespace ISP

// ============================================================================
// 5. DIP - Dependency Inversion Principle
//    "Depend on abstractions, not concretions"
// ============================================================================

namespace DIP {

// VIOLATION: High-level module directly depends on low-level implementation
// class SmtpSender { void send(std::string msg) { /* SMTP logic */ } };
// class NotificationService {
//     SmtpSender smtp;  // TIGHT COUPLING to concrete class!
//     void notify(std::string msg) { smtp.send(msg); }
// };

// FIX: Both high and low-level modules depend on abstraction
class IMessageSender {
public:
    virtual ~IMessageSender() = default;
    virtual bool send(const std::string& to, const std::string& msg) = 0;
    virtual std::string type() const = 0;
};

class SmtpSender : public IMessageSender {
public:
    bool send(const std::string& to, const std::string& msg) override {
        std::cout << "  [SMTP] To: " << to << " - " << msg << "\n";
        return true;
    }
    std::string type() const override { return "SMTP"; }
};

class SmsSender : public IMessageSender {
public:
    bool send(const std::string& to, const std::string& msg) override {
        std::cout << "  [SMS] To: " << to << " - " << msg << "\n";
        return true;
    }
    std::string type() const override { return "SMS"; }
};

class PushSender : public IMessageSender {
public:
    bool send(const std::string& to, const std::string& msg) override {
        std::cout << "  [Push] To: " << to << " - " << msg << "\n";
        return true;
    }
    std::string type() const override { return "Push"; }
};

// High-level module depends on ABSTRACTION (IMessageSender), not concrete class
class NotificationService {
    std::vector<std::unique_ptr<IMessageSender>> senders_;
public:
    void addSender(std::unique_ptr<IMessageSender> sender) {
        senders_.push_back(std::move(sender));
    }

    int notifyAll(const std::string& to, const std::string& msg) {
        int sent = 0;
        for (auto& sender : senders_) {
            if (sender->send(to, msg)) ++sent;
        }
        return sent;
    }
};

void demonstrate() {
    std::cout << "=== DIP: Dependency Inversion ===\n";
    NotificationService service;

    // Inject dependencies - easy to swap, mock, or extend
    service.addSender(std::make_unique<SmtpSender>());
    service.addSender(std::make_unique<SmsSender>());
    service.addSender(std::make_unique<PushSender>());

    int count = service.notifyAll("user@example.com", "Order shipped!");
    assert(count == 3);

    std::cout << "  Service doesn't know about SMTP/SMS/Push directly ✓\n";
    std::cout << "  Can inject mock sender for testing ✓\n";
    std::cout << "  Adding new channel = new class, zero service changes ✓\n\n";
}

} // namespace DIP

// ============================================================================

int main() {
    SRP::demonstrate();
    OCP::demonstrate();
    LSP::demonstrate();
    ISP::demonstrate();
    DIP::demonstrate();

    std::cout << "=== Summary ===\n";
    std::cout << "S - Single Responsibility: One class, one reason to change\n";
    std::cout << "O - Open/Closed: Extend via new classes, not modification\n";
    std::cout << "L - Liskov Substitution: Subtypes honor base contracts\n";
    std::cout << "I - Interface Segregation: Small, focused interfaces\n";
    std::cout << "D - Dependency Inversion: Depend on abstractions\n";
    std::cout << "\nAll SOLID assertions passed!\n";
    return 0;
}
