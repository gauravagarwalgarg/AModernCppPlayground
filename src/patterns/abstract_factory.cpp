// abstract_factory.cpp - Abstract Factory Pattern
// Interview Relevance: Tests understanding of creating families of related objects
// without specifying concrete classes. Real-world use: cross-platform UI toolkits
// (Qt, wxWidgets), database driver families, theme systems.
// Compile: g++ -std=c++20 -Wall abstract_factory.cpp -o abstract_factory

#include <iostream>
#include <memory>
#include <string>
#include <cassert>

// Abstract products
class Button {
public:
    virtual ~Button() = default;
    virtual std::string render() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual std::string render() const = 0;
};

// Linux family
class LinuxButton : public Button {
public:
    std::string render() const override { return "[GTK Button]"; }
};

class LinuxCheckbox : public Checkbox {
public:
    std::string render() const override { return "[GTK Checkbox]"; }
};

// Windows family
class WindowsButton : public Button {
public:
    std::string render() const override { return "[Win32 Button]"; }
};

class WindowsCheckbox : public Checkbox {
public:
    std::string render() const override { return "[Win32 Checkbox]"; }
};

// Abstract Factory
class UIFactory {
public:
    virtual ~UIFactory() = default;
    virtual std::unique_ptr<Button> createButton() const = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() const = 0;
};

class LinuxUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<LinuxButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<LinuxCheckbox>();
    }
};

class WindowsUIFactory : public UIFactory {
public:
    std::unique_ptr<Button> createButton() const override {
        return std::make_unique<WindowsButton>();
    }
    std::unique_ptr<Checkbox> createCheckbox() const override {
        return std::make_unique<WindowsCheckbox>();
    }
};

// Client code - works with ANY factory, doesn't know concrete types
void buildUI(const UIFactory& factory) {
    auto btn = factory.createButton();
    auto chk = factory.createCheckbox();
    std::cout << "  Button:   " << btn->render() << "\n";
    std::cout << "  Checkbox: " << chk->render() << "\n";
}

int main() {
    std::cout << "--- Linux UI ---\n";
    LinuxUIFactory linuxFactory;
    buildUI(linuxFactory);

    std::cout << "--- Windows UI ---\n";
    WindowsUIFactory winFactory;
    buildUI(winFactory);

    // Verify family consistency
    auto btn = linuxFactory.createButton();
    assert(btn->render().find("GTK") != std::string::npos);

    auto winBtn = winFactory.createButton();
    assert(winBtn->render().find("Win32") != std::string::npos);

    std::cout << "\nKey insight: Ensures products from same family are compatible.\n";
    std::cout << "Adding macOS support = new factory + new products, zero client changes.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
