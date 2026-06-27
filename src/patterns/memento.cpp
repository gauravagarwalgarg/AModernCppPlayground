/**************************************************************************************************
 * Topic: Memento Pattern Capture and Restore State (Undo/Redo)
 *
 * Interview Relevance: ★★★★
 * What interviewers look for:
 *   - Encapsulation preserved: caretaker can't access internal state
 *   - Undo/redo stack implementation
 *   - Memory management of snapshots
 *   - Real-world: text editors, game save states, transaction rollback
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o memento memento.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Memento opaque snapshot (caretaker can't peek inside)
// ============================================================================

class EditorMemento {
public:
    virtual ~EditorMemento() = default;
    virtual std::string description() const = 0;
};

// ============================================================================
// Originator the object whose state we want to save/restore
// ============================================================================

class TextEditor {
public:
    void type(std::string const& text) { content_ += text; }
    void set_cursor(int pos) { cursor_ = pos; }
    void clear() { content_.clear(); cursor_ = 0; }

    std::string const& content() const { return content_; }
    int cursor() const { return cursor_; }

    // Save current state
    std::unique_ptr<EditorMemento> save() const {
        return std::make_unique<ConcreteMemento>(content_, cursor_);
    }

    // Restore from memento
    void restore(EditorMemento const& memento) {
        auto const& m = static_cast<ConcreteMemento const&>(memento);
        content_ = m.content_;
        cursor_ = m.cursor_;
    }

private:
    std::string content_;
    int cursor_{0};

    // Concrete memento nested private class (encapsulation!)
    class ConcreteMemento : public EditorMemento {
    public:
        ConcreteMemento(std::string content, int cursor)
            : content_(std::move(content)), cursor_(cursor) {}

        std::string description() const override {
            auto preview = content_.substr(0, 20);
            if (content_.size() > 20) preview += "...";
            return "\"" + preview + "\" (cursor:" + std::to_string(cursor_) + ")";
        }

    private:
        friend class TextEditor;  // Only TextEditor can access internals
        std::string content_;
        int cursor_;
    };
};

// ============================================================================
// Caretaker manages history (undo/redo stacks)
// ============================================================================

class History {
public:
    void push(std::unique_ptr<EditorMemento> memento) {
        undo_stack_.push_back(std::move(memento));
        redo_stack_.clear();  // New action invalidates redo history
    }

    EditorMemento const* undo() {
        if (undo_stack_.empty()) return nullptr;
        redo_stack_.push_back(std::move(undo_stack_.back()));
        undo_stack_.pop_back();
        return undo_stack_.empty() ? nullptr : undo_stack_.back().get();
    }

    EditorMemento const* redo() {
        if (redo_stack_.empty()) return nullptr;
        auto* result = redo_stack_.back().get();
        undo_stack_.push_back(std::move(redo_stack_.back()));
        redo_stack_.pop_back();
        return result;
    }

    size_t undo_count() const { return undo_stack_.size(); }
    size_t redo_count() const { return redo_stack_.size(); }

private:
    std::vector<std::unique_ptr<EditorMemento>> undo_stack_;
    std::vector<std::unique_ptr<EditorMemento>> redo_stack_;
};

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Memento Pattern (Undo/Redo) ===\n\n";

    TextEditor editor;
    History history;

    // Type some text, saving state at each step
    editor.type("Hello");
    history.push(editor.save());
    std::cout << "State 1: \"" << editor.content() << "\"\n";

    editor.type(", World");
    history.push(editor.save());
    std::cout << "State 2: \"" << editor.content() << "\"\n";

    editor.type("! How are you?");
    history.push(editor.save());
    std::cout << "State 3: \"" << editor.content() << "\"\n";

    assert(editor.content() == "Hello, World! How are you?");

    // Undo
    std::cout << "\n--- Undo ---\n";
    if (auto* m = history.undo()) {
        editor.restore(*m);
        std::cout << "After undo: \"" << editor.content() << "\"\n";
    }
    assert(editor.content() == "Hello, World");

    if (auto* m = history.undo()) {
        editor.restore(*m);
        std::cout << "After undo: \"" << editor.content() << "\"\n";
    }
    assert(editor.content() == "Hello");

    // Redo
    std::cout << "\n--- Redo ---\n";
    if (auto* m = history.redo()) {
        editor.restore(*m);
        std::cout << "After redo: \"" << editor.content() << "\"\n";
    }
    assert(editor.content() == "Hello, World");

    assert(history.undo_count() == 2);
    assert(history.redo_count() == 1);

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
