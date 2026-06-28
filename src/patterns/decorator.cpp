// decorator.cpp - Decorator Pattern (Stream Wrappers)
// Interview Relevance: Shows how to add behavior at runtime without subclass explosion.
// Tests understanding of composition over inheritance, transparent wrapping, and
// the decorator's recursive structure. Real use: Java I/O streams, middleware chains.
// Compile: g++ -std=c++20 -Wall decorator.cpp -o decorator

#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <cassert>

class DataStream {
public:
    virtual ~DataStream() = default;
    virtual std::string write(const std::string& data) = 0;
    virtual std::string read(const std::string& data) = 0;
};

class RawStream : public DataStream {
public:
    std::string write(const std::string& data) override { return data; }
    std::string read(const std::string& data) override { return data; }
};

// Base decorator - holds a wrapped stream
class StreamDecorator : public DataStream {
protected:
    std::unique_ptr<DataStream> wrapped_;
public:
    explicit StreamDecorator(std::unique_ptr<DataStream> s) : wrapped_(std::move(s)) {}
};

class EncryptedStream : public StreamDecorator {
public:
    using StreamDecorator::StreamDecorator;

    std::string write(const std::string& data) override {
        // Simple XOR "encryption" for demonstration
        std::string encrypted = data;
        for (char& c : encrypted) c ^= 0x42;
        std::cout << "  [Encrypt] Applied encryption\n";
        return wrapped_->write(encrypted);
    }

    std::string read(const std::string& data) override {
        std::string decrypted = wrapped_->read(data);
        for (char& c : decrypted) c ^= 0x42;
        std::cout << "  [Decrypt] Applied decryption\n";
        return decrypted;
    }
};

class CompressedStream : public StreamDecorator {
public:
    using StreamDecorator::StreamDecorator;

    std::string write(const std::string& data) override {
        // Simulated RLE compression
        std::string compressed = "[compressed:" + std::to_string(data.size()) + "]" + data;
        std::cout << "  [Compress] Compressed data\n";
        return wrapped_->write(compressed);
    }

    std::string read(const std::string& data) override {
        std::string raw = wrapped_->read(data);
        // Strip our compression header
        auto pos = raw.find(']');
        std::string decompressed = (pos != std::string::npos) ? raw.substr(pos + 1) : raw;
        std::cout << "  [Decompress] Decompressed data\n";
        return decompressed;
    }
};

int main() {
    // Stack decorators: Raw → Encrypted → Compressed
    std::cout << "--- Writing with Compressed(Encrypted(Raw)) ---\n";
    auto stream = std::make_unique<CompressedStream>(
                      std::make_unique<EncryptedStream>(
                          std::make_unique<RawStream>()));

    std::string original = "Hello, Decorator Pattern!";
    std::string written = stream->write(original);
    std::cout << "  Written bytes: " << written.size() << "\n\n";

    std::cout << "--- Reading back ---\n";
    std::string result = stream->read(written);
    assert(result == original);
    std::cout << "  Result: \"" << result << "\"\n";

    std::cout << "\nKey insight: Each decorator is transparent - you can stack\n";
    std::cout << "any combination without modifying existing classes.\n";
    std::cout << "N decorators = N classes, not 2^N subclass combinations.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
