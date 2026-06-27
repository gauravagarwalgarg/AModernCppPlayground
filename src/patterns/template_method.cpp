// template_method.cpp - Template Method Pattern
// Interview Relevance: Defines algorithm skeleton in base class, deferring steps to
// subclasses. Tests understanding of the Hollywood Principle ("don't call us, we'll
// call you"), NVI idiom, and when to use virtual vs. final methods.
// Compile: g++ -std=c++20 -Wall template_method.cpp -o template_method

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>

class DataProcessor {
public:
    virtual ~DataProcessor() = default;

    // Template method - defines the algorithm skeleton (final = can't override)
    void process() {
        std::string raw = readData();
        std::cout << "  Read: " << raw.substr(0, 40) << "...\n";
        auto processed = processData(raw);
        std::cout << "  Processed: " << processed.size() << " records\n";
        writeData(processed);
        std::cout << "  Written successfully\n";
    }

protected:
    // Steps to be overridden by subclasses
    virtual std::string readData() = 0;
    virtual std::vector<std::string> processData(const std::string& raw) = 0;
    virtual void writeData(const std::vector<std::string>& data) = 0;
};

class CSVProcessor : public DataProcessor {
    std::vector<std::string> output_;
protected:
    std::string readData() override {
        return "name,age,city\nAlice,30,NYC\nBob,25,LA";  // Simulated file read
    }

    std::vector<std::string> processData(const std::string& raw) override {
        std::vector<std::string> records;
        std::istringstream iss(raw);
        std::string line;
        std::getline(iss, line);  // Skip header
        while (std::getline(iss, line)) {
            records.push_back("[CSV] " + line);
        }
        return records;
    }

    void writeData(const std::vector<std::string>& data) override {
        output_ = data;
        for (const auto& r : data) std::cout << "    → " << r << "\n";
    }
public:
    const std::vector<std::string>& output() const { return output_; }
};

class JSONProcessor : public DataProcessor {
    std::vector<std::string> output_;
protected:
    std::string readData() override {
        return R"([{"name":"Charlie","age":35},{"name":"Diana","age":28}])";
    }

    std::vector<std::string> processData(const std::string& raw) override {
        // Simplified JSON "parsing"
        std::vector<std::string> records;
        size_t pos = 0;
        while ((pos = raw.find("\"name\"", pos)) != std::string::npos) {
            auto end = raw.find('}', pos);
            records.push_back("[JSON] " + raw.substr(pos, end - pos));
            pos = end;
        }
        return records;
    }

    void writeData(const std::vector<std::string>& data) override {
        output_ = data;
        for (const auto& r : data) std::cout << "    → " << r << "\n";
    }
public:
    const std::vector<std::string>& output() const { return output_; }
};

int main() {
    std::cout << "--- CSV Processing ---\n";
    CSVProcessor csv;
    csv.process();
    assert(csv.output().size() == 2);

    std::cout << "\n--- JSON Processing ---\n";
    JSONProcessor json;
    json.process();
    assert(json.output().size() == 2);

    std::cout << "\nKey insight: Base class controls WHEN steps run (the skeleton).\n";
    std::cout << "Subclasses control WHAT each step does. Algorithm flow is fixed.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
