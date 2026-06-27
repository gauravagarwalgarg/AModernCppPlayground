// chain_of_responsibility.cpp - Chain of Responsibility Pattern
// Interview Relevance: Decouples sender from receiver by passing requests along a
// chain. Tests understanding of linked processing, middleware patterns, and when
// to stop propagation. Real use: logging frameworks, HTTP middleware, event bubbling.
// Compile: g++ -std=c++20 -Wall chain_of_responsibility.cpp -o chain_of_responsibility

#include <iostream>
#include <memory>
#include <string>
#include <cassert>

enum class LogLevel { Debug, Info, Warning, Error };

std::string levelToStr(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
    }
    return "UNKNOWN";
}

class LogHandler {
protected:
    LogLevel level_;
    std::unique_ptr<LogHandler> next_;
public:
    explicit LogHandler(LogLevel level) : level_(level) {}
    virtual ~LogHandler() = default;

    void setNext(std::unique_ptr<LogHandler> handler) {
        next_ = std::move(handler);
    }

    void handle(LogLevel msgLevel, const std::string& msg) {
        if (msgLevel >= level_) {
            write(msgLevel, msg);
        }
        // Pass to next handler regardless (each decides independently)
        if (next_) {
            next_->handle(msgLevel, msg);
        }
    }

protected:
    virtual void write(LogLevel level, const std::string& msg) = 0;
};

class ConsoleHandler : public LogHandler {
public:
    explicit ConsoleHandler(LogLevel level) : LogHandler(level) {}
protected:
    void write(LogLevel level, const std::string& msg) override {
        std::cout << "  [Console/" << levelToStr(level) << "] " << msg << "\n";
    }
};

class FileHandler : public LogHandler {
    int write_count_ = 0;
public:
    explicit FileHandler(LogLevel level) : LogHandler(level) {}
    int writeCount() const { return write_count_; }
protected:
    void write(LogLevel level, const std::string& msg) override {
        std::cout << "  [File/" << levelToStr(level) << "] " << msg << "\n";
        ++write_count_;
    }
};

class AlertHandler : public LogHandler {
    int alert_count_ = 0;
public:
    explicit AlertHandler(LogLevel level) : LogHandler(level) {}
    int alertCount() const { return alert_count_; }
protected:
    void write(LogLevel level, const std::string& msg) override {
        std::cout << "  [ALERT/" << levelToStr(level) << "] *** " << msg << " ***\n";
        ++alert_count_;
    }
};

int main() {
    // Build chain: Console(Debug) → File(Info) → Alert(Error)
    auto alert = std::make_unique<AlertHandler>(LogLevel::Error);
    auto* alertPtr = alert.get();

    auto file = std::make_unique<FileHandler>(LogLevel::Info);
    auto* filePtr = file.get();
    file->setNext(std::move(alert));

    auto console = std::make_unique<ConsoleHandler>(LogLevel::Debug);
    console->setNext(std::move(file));

    std::cout << "--- Sending messages through the chain ---\n";
    console->handle(LogLevel::Debug, "Variable x = 42");     // Console only
    console->handle(LogLevel::Info, "User logged in");        // Console + File
    console->handle(LogLevel::Error, "Database connection failed"); // All three

    assert(filePtr->writeCount() == 2);   // Info + Error
    assert(alertPtr->alertCount() == 1);  // Error only

    std::cout << "\nKey insight: Each handler independently decides whether to process.\n";
    std::cout << "Handlers can be reordered/added/removed without changing senders.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
