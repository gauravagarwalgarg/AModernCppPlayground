/*
 * Smart Pointers in Modern C++
 *
 * WHAT: RAII wrappers for dynamic memory unique_ptr (exclusive ownership),
 * shared_ptr (shared ownership with ref counting), weak_ptr (non-owning observer).
 *
 * WHY IT MATTERS IN INTERVIEWS: Smart pointers replace raw new/delete entirely.
 * Interviewers test understanding of ownership semantics, reference cycles, and
 * when to choose which pointer type.
 *
 * WHAT INTERVIEWERS LOOK FOR:
 * - unique_ptr for single ownership (zero overhead, move-only)
 * - shared_ptr for shared ownership (ref counting overhead, thread-safe count)
 * - weak_ptr to break circular references
 * - Factory pattern returning unique_ptr
 * - Custom deleters for non-memory resources
 *
 * COMMON PITFALLS:
 * - Circular references with shared_ptr → memory leak
 * - Creating shared_ptr from raw pointer multiple times (double delete)
 * - Storing raw pointer from get() and outliving the smart pointer
 * - shared_ptr overhead in hot paths (atomic ref count)
 */

#include <iostream>
#include <memory>
#include <cassert>
#include <vector>
#include <functional>

// === unique_ptr: exclusive ownership, zero overhead ===
class Sensor {
    std::string id_;
public:
    explicit Sensor(std::string id) : id_(std::move(id)) {
        std::cout << "  Sensor [" << id_ << "] created\n";
    }
    ~Sensor() { std::cout << "  Sensor [" << id_ << "] destroyed\n"; }
    const std::string& id() const { return id_; }
};

// Factory pattern the idiomatic way to create objects
std::unique_ptr<Sensor> createSensor(const std::string& id) {
    return std::make_unique<Sensor>(id); // No raw new
}

// === shared_ptr: circular reference demo ===
struct Node {
    std::string name;
    std::shared_ptr<Node> next;   // Strong reference
    std::weak_ptr<Node> parent;   // Weak reference breaks cycle
    Node(std::string n) : name(std::move(n)) {
        std::cout << "  Node [" << name << "] created\n";
    }
    ~Node() { std::cout << "  Node [" << name << "] destroyed\n"; }
};

// === Custom deleter for non-memory resources ===
struct FileDeleter {
    void operator()(FILE* fp) const {
        if (fp) {
            std::cout << "  Custom deleter: closing file\n";
            fclose(fp);
        }
    }
};

int main() {
    std::cout << "=== 1. unique_ptr exclusive ownership ===\n";
    {
        auto sensor = createSensor("TEMP_01");
        assert(sensor != nullptr);
        assert(sensor->id() == "TEMP_01");

        // Transfer ownership move only, no copy
        auto sensor2 = std::move(sensor);
        assert(sensor == nullptr); // Original is null after move
        assert(sensor2->id() == "TEMP_01");
    } // sensor2 destroyed here automatically

    std::cout << "\n=== 2. shared_ptr reference counting ===\n";
    {
        auto shared1 = std::make_shared<Sensor>("PRES_01");
        assert(shared1.use_count() == 1);
        {
            auto shared2 = shared1; // Copy ref count increases
            assert(shared1.use_count() == 2);
            assert(shared2.use_count() == 2);
        } // shared2 destroyed, count drops to 1
        assert(shared1.use_count() == 1);
    } // shared1 destroyed, Sensor cleaned up

    std::cout << "\n=== 3. weak_ptr breaking cycles ===\n";
    {
        auto parent = std::make_shared<Node>("Parent");
        auto child = std::make_shared<Node>("Child");
        parent->next = child;        // parent owns child
        child->parent = parent;      // weak_ptr: child observes parent, no cycle

        assert(parent.use_count() == 1); // Only one strong ref to parent
        assert(child.use_count() == 2);  // parent->next + child

        // Accessing weak_ptr safely
        if (auto locked = child->parent.lock()) {
            assert(locked->name == "Parent");
            std::cout << "  Child's parent: " << locked->name << "\n";
        }
    } // Both destroyed properly no leak!

    std::cout << "\n=== 4. Custom deleter ===\n";
    {
        std::unique_ptr<FILE, FileDeleter> file(fopen("/dev/null", "w"));
        assert(file != nullptr);
        // File auto-closed when unique_ptr goes out of scope
    }

    // Lambda deleter with shared_ptr
    {
        auto deleter = [](int* p) {
            std::cout << "  Lambda deleter: freeing array\n";
            delete[] p;
        };
        std::shared_ptr<int> arr(new int[10], deleter);
    }

    std::cout << "\n=== 5. When to use each ===\n";
    std::cout << "  unique_ptr: default choice, single owner (factories, members)\n";
    std::cout << "  shared_ptr: genuine shared ownership (caches, observer patterns)\n";
    std::cout << "  weak_ptr:   observing without owning (break cycles, caches)\n";
    std::cout << "  raw ptr:    non-owning reference (function params, views)\n";

    std::cout << "\nAll assertions passed!\n";
    return 0;
}
