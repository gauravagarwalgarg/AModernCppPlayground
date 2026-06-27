/*
 * What: Full doubly linked list: Node<T>, DoublyLinkedList<T> with push_front/back,
 *       pop_front/back, insert_at, erase, size, iterator support. Rule of Five.
 *
 * Why: Demonstrates manual memory management, pointer manipulation, and iterator
 *      design. Interviewers use this to assess understanding of data structure
 *      internals, RAII, and the STL iterator contract.
 *
 * Interviewers look for:
 *   - Correct pointer updates on insert/erase (prev/next consistency)
 *   - Rule of Five: copy ctor, copy assign, move ctor, move assign, destructor
 *   - Iterator implementing operator++, --, *, !=, == (bidirectional iterator)
 *   - Edge cases: operations on empty list, single-element list
 *
 * Pitfalls:
 *   - Forgetting to update both prev AND next pointers on insert/erase
 *   - Memory leaks on destruction if not traversing and deleting all nodes
 *   - Iterator invalidation on erase (dangling pointer)
 *   - Off-by-one errors in insert_at with 0-based indexing
 */

#include <iostream>
#include <cassert>
#include <string>
#include <utility>

template <typename T>
class DoublyLinkedList {
    struct Node {
        T data;
        Node* prev;
        Node* next;
        Node(const T& val) : data(val), prev(nullptr), next(nullptr) {}
        Node(T&& val) : data(std::move(val)), prev(nullptr), next(nullptr) {}
    };

    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_t size_ = 0;

    void clear() {
        while (head_) { Node* tmp = head_; head_ = head_->next; delete tmp; }
        tail_ = nullptr; size_ = 0;
    }

    void copy_from(const DoublyLinkedList& other) {
        for (Node* n = other.head_; n; n = n->next) push_back(n->data);
    }

public:
    DoublyLinkedList() = default;
    ~DoublyLinkedList() { clear(); }

    // Rule of Five
    DoublyLinkedList(const DoublyLinkedList& other) { copy_from(other); }
    DoublyLinkedList& operator=(const DoublyLinkedList& other) {
        if (this != &other) { clear(); copy_from(other); }
        return *this;
    }
    DoublyLinkedList(DoublyLinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_) {
        other.head_ = other.tail_ = nullptr; other.size_ = 0;
    }
    DoublyLinkedList& operator=(DoublyLinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_; tail_ = other.tail_; size_ = other.size_;
            other.head_ = other.tail_ = nullptr; other.size_ = 0;
        }
        return *this;
    }

    void push_front(const T& val) {
        Node* n = new Node(val);
        n->next = head_;
        if (head_) head_->prev = n;
        head_ = n;
        if (!tail_) tail_ = n;
        ++size_;
    }

    void push_back(const T& val) {
        Node* n = new Node(val);
        n->prev = tail_;
        if (tail_) tail_->next = n;
        tail_ = n;
        if (!head_) head_ = n;
        ++size_;
    }

    void pop_front() {
        assert(head_ && "pop_front on empty list");
        Node* tmp = head_;
        head_ = head_->next;
        if (head_) head_->prev = nullptr; else tail_ = nullptr;
        delete tmp; --size_;
    }

    void pop_back() {
        assert(tail_ && "pop_back on empty list");
        Node* tmp = tail_;
        tail_ = tail_->prev;
        if (tail_) tail_->next = nullptr; else head_ = nullptr;
        delete tmp; --size_;
    }

    void insert_at(size_t pos, const T& val) {
        assert(pos <= size_);
        if (pos == 0) { push_front(val); return; }
        if (pos == size_) { push_back(val); return; }
        Node* cur = head_;
        for (size_t i = 0; i < pos; ++i) cur = cur->next;
        Node* n = new Node(val);
        n->prev = cur->prev; n->next = cur;
        cur->prev->next = n; cur->prev = n;
        ++size_;
    }

    void erase(size_t pos) {
        assert(pos < size_);
        if (pos == 0) { pop_front(); return; }
        if (pos == size_ - 1) { pop_back(); return; }
        Node* cur = head_;
        for (size_t i = 0; i < pos; ++i) cur = cur->next;
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete cur; --size_;
    }

    size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    T& front() { assert(head_); return head_->data; }
    T& back() { assert(tail_); return tail_->data; }

    // Bidirectional iterator
    struct Iterator {
        Node* node;
        Iterator(Node* n) : node(n) {}
        T& operator*() { return node->data; }
        Iterator& operator++() { node = node->next; return *this; }
        Iterator& operator--() { node = node->prev; return *this; }
        bool operator!=(const Iterator& o) const { return node != o.node; }
        bool operator==(const Iterator& o) const { return node == o.node; }
    };

    Iterator begin() { return Iterator(head_); }
    Iterator end() { return Iterator(nullptr); }
};

int main() {
    // Test with int
    DoublyLinkedList<int> list;
    for (int i = 1; i <= 5; ++i) list.push_back(i);
    assert(list.size() == 5 && list.front() == 1 && list.back() == 5);

    list.push_front(0);
    assert(list.front() == 0 && list.size() == 6);

    list.insert_at(3, 99);
    assert(list.size() == 7);

    list.erase(3); // remove the 99
    list.pop_front(); list.pop_back();
    assert(list.size() == 4 && list.front() == 1 && list.back() == 4);

    std::cout << "List contents: ";
    for (auto& val : list) std::cout << val << " ";
    std::cout << "\n";

    // Test Rule of Five
    DoublyLinkedList<int> copy = list;       // copy ctor
    DoublyLinkedList<int> moved = std::move(copy); // move ctor
    assert(moved.size() == 4 && copy.size() == 0);

    // Test with string
    DoublyLinkedList<std::string> slist;
    slist.push_back("hello");
    slist.push_back("world");
    assert(slist.size() == 2 && slist.front() == "hello");

    std::cout << "String list: ";
    for (auto& s : slist) std::cout << s << " ";
    std::cout << "\n";

    std::cout << "All doubly linked list tests passed!\n";
    return 0;
}
