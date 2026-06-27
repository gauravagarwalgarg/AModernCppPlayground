// command.cpp - Command Pattern (TextEditor with Undo/Redo)
// Interview Relevance: Encapsulates operations as objects enabling undo/redo,
// macro recording, and operation queuing. Tests understanding of operation history,
// stack-based undo, and decoupling invoker from receiver.
// Compile: g++ -std=c++20 -Wall command.cpp -o command

#include <iostream>
#include <string>
#include <memory>
#include <stack>
#include <cassert>

// Receiver
class TextDocument {
    std::string text_;
public:
    void insert(size_t pos, const std::string& s) { text_.insert(pos, s); }
    void erase(size_t pos, size_t len) { text_.erase(pos, len); }
    const std::string& text() const { return text_; }
};

// Command interface
class Command {
public:
    virtual ~Command() = default;
    virtual void execute() = 0;
    virtual void undo() = 0;
};

class InsertCommand : public Command {
    TextDocument& doc_;
    size_t pos_;
    std::string text_;
public:
    InsertCommand(TextDocument& doc, size_t pos, std::string text)
        : doc_(doc), pos_(pos), text_(std::move(text)) {}
    void execute() override { doc_.insert(pos_, text_); }
    void undo() override { doc_.erase(pos_, text_.size()); }
};

class DeleteCommand : public Command {
    TextDocument& doc_;
    size_t pos_, len_;
    std::string deleted_;  // Store for undo
public:
    DeleteCommand(TextDocument& doc, size_t pos, size_t len)
        : doc_(doc), pos_(pos), len_(len) {}
    void execute() override {
        deleted_ = doc_.text().substr(pos_, len_);
        doc_.erase(pos_, len_);
    }
    void undo() override { doc_.insert(pos_, deleted_); }
};

// Invoker - manages command history
class TextEditor {
    TextDocument doc_;
    std::stack<std::unique_ptr<Command>> undo_stack_;
    std::stack<std::unique_ptr<Command>> redo_stack_;

    void executeCmd(std::unique_ptr<Command> cmd) {
        cmd->execute();
        undo_stack_.push(std::move(cmd));
        while (!redo_stack_.empty()) redo_stack_.pop();  // Clear redo on new action
    }
public:
    void insert(size_t pos, const std::string& text) {
        executeCmd(std::make_unique<InsertCommand>(doc_, pos, text));
    }
    void erase(size_t pos, size_t len) {
        executeCmd(std::make_unique<DeleteCommand>(doc_, pos, len));
    }
    void undo() {
        if (undo_stack_.empty()) return;
        auto cmd = std::move(undo_stack_.top()); undo_stack_.pop();
        cmd->undo();
        redo_stack_.push(std::move(cmd));
    }
    void redo() {
        if (redo_stack_.empty()) return;
        auto cmd = std::move(redo_stack_.top()); redo_stack_.pop();
        cmd->execute();
        undo_stack_.push(std::move(cmd));
    }
    const std::string& text() const { return doc_.text(); }
};

int main() {
    TextEditor editor;

    editor.insert(0, "Hello World");
    std::cout << "After insert: \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello World");

    editor.insert(5, ", Beautiful");
    std::cout << "After insert: \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello, Beautiful World");

    editor.erase(5, 11);  // Remove ", Beautiful"
    std::cout << "After delete: \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello World");

    editor.undo();
    std::cout << "After undo:   \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello, Beautiful World");

    editor.undo();
    std::cout << "After undo:   \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello World");

    editor.redo();
    std::cout << "After redo:   \"" << editor.text() << "\"\n";
    assert(editor.text() == "Hello, Beautiful World");

    std::cout << "\nKey insight: Commands are first-class objects. Each operation\n";
    std::cout << "knows how to execute AND undo itself. History is just a stack.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
