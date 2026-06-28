// state.cpp - State Pattern (TCP Connection)
// Interview Relevance: Replaces complex state-based conditionals with polymorphism.
// Tests understanding of state transitions, encapsulation of state-specific behavior,
// and the difference between State and Strategy patterns.
// Compile: g++ -std=c++20 -Wall state.cpp -o state

#include <iostream>
#include <memory>
#include <string>
#include <cassert>

class TCPConnection;  // Forward declaration

class TCPState {
public:
    virtual ~TCPState() = default;
    virtual void open(TCPConnection& conn) = 0;
    virtual void close(TCPConnection& conn) = 0;
    virtual void send(TCPConnection& conn, const std::string& data) = 0;
    virtual std::string name() const = 0;
};

class TCPConnection {
    std::unique_ptr<TCPState> state_;
public:
    TCPConnection(std::unique_ptr<TCPState> initial) : state_(std::move(initial)) {}

    void setState(std::unique_ptr<TCPState> s) { state_ = std::move(s); }
    std::string stateName() const { return state_->name(); }

    void open() { state_->open(*this); }
    void close() { state_->close(*this); }
    void send(const std::string& data) { state_->send(*this, data); }
};

// Concrete states
class ClosedState : public TCPState {
public:
    void open(TCPConnection& conn) override;
    void close(TCPConnection&) override {
        std::cout << "  [Closed] Already closed\n";
    }
    void send(TCPConnection&, const std::string&) override {
        std::cout << "  [Closed] Error: Cannot send, connection closed\n";
    }
    std::string name() const override { return "CLOSED"; }
};

class ListenState : public TCPState {
public:
    void open(TCPConnection& conn) override;
    void close(TCPConnection& conn) override;
    void send(TCPConnection&, const std::string&) override {
        std::cout << "  [Listen] Error: Cannot send, still listening\n";
    }
    std::string name() const override { return "LISTEN"; }
};

class EstablishedState : public TCPState {
public:
    void open(TCPConnection&) override {
        std::cout << "  [Established] Already connected\n";
    }
    void close(TCPConnection& conn) override;
    void send(TCPConnection&, const std::string& data) override {
        std::cout << "  [Established] Sending: \"" << data << "\"\n";
    }
    std::string name() const override { return "ESTABLISHED"; }
};

// State transition implementations
void ClosedState::open(TCPConnection& conn) {
    std::cout << "  [Closed → Listen] Opening connection...\n";
    conn.setState(std::make_unique<ListenState>());
}

void ListenState::open(TCPConnection& conn) {
    std::cout << "  [Listen → Established] Connection established!\n";
    conn.setState(std::make_unique<EstablishedState>());
}

void ListenState::close(TCPConnection& conn) {
    std::cout << "  [Listen → Closed] Closing listener\n";
    conn.setState(std::make_unique<ClosedState>());
}

void EstablishedState::close(TCPConnection& conn) {
    std::cout << "  [Established → Closed] Closing connection\n";
    conn.setState(std::make_unique<ClosedState>());
}

int main() {
    TCPConnection conn(std::make_unique<ClosedState>());
    assert(conn.stateName() == "CLOSED");

    std::cout << "--- TCP State Transitions ---\n";
    conn.send("hello");    // Error: closed
    conn.open();           // Closed → Listen
    assert(conn.stateName() == "LISTEN");

    conn.send("hello");    // Error: listening
    conn.open();           // Listen → Established
    assert(conn.stateName() == "ESTABLISHED");

    conn.send("Hello, Server!");  // Success
    conn.open();                  // Already connected

    conn.close();          // Established → Closed
    assert(conn.stateName() == "CLOSED");

    conn.close();          // Already closed
    conn.send("data");     // Error: closed

    std::cout << "\nKey insight: Each state encapsulates its own behavior.\n";
    std::cout << "No switch/case on state - adding new states doesn't touch existing code.\n";
    std::cout << "State pattern = Strategy where transitions happen internally.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
