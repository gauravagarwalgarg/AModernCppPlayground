// builder.cpp - Builder Pattern (QueryBuilder with Method Chaining)
// Interview Relevance: Demonstrates step-by-step construction of complex objects,
// method chaining (fluent interface), and immutable result. Common in SQL builders,
// HTTP request builders, and configuration objects.
// Compile: g++ -std=c++20 -Wall builder.cpp -o builder

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

class Query {
    std::string sql_;
    friend class QueryBuilder;
    explicit Query(std::string sql) : sql_(std::move(sql)) {}
public:
    const std::string& str() const { return sql_; }
};

class QueryBuilder {
    std::string select_clause_;
    std::string from_clause_;
    std::vector<std::string> where_clauses_;
    std::string order_clause_;
    int limit_ = -1;

public:
    QueryBuilder& select(const std::string& cols) {
        select_clause_ = cols;
        return *this;  // Enable method chaining
    }

    QueryBuilder& from(const std::string& table) {
        from_clause_ = table;
        return *this;
    }

    QueryBuilder& where(const std::string& condition) {
        where_clauses_.push_back(condition);
        return *this;
    }

    QueryBuilder& orderBy(const std::string& col) {
        order_clause_ = col;
        return *this;
    }

    QueryBuilder& limit(int n) {
        limit_ = n;
        return *this;
    }

    // Build produces immutable result - builder can be reused
    Query build() const {
        std::string sql = "SELECT " + select_clause_ + " FROM " + from_clause_;
        for (size_t i = 0; i < where_clauses_.size(); ++i) {
            sql += (i == 0 ? " WHERE " : " AND ") + where_clauses_[i];
        }
        if (!order_clause_.empty()) sql += " ORDER BY " + order_clause_;
        if (limit_ > 0) sql += " LIMIT " + std::to_string(limit_);
        return Query(sql);
    }
};

int main() {
    // Fluent interface - reads like natural language
    auto query = QueryBuilder()
        .select("name, email")
        .from("users")
        .where("age > 18")
        .where("active = true")
        .orderBy("name ASC")
        .limit(10)
        .build();

    std::cout << "Generated SQL:\n  " << query.str() << "\n\n";

    assert(query.str() == "SELECT name, email FROM users WHERE age > 18 "
                          "AND active = true ORDER BY name ASC LIMIT 10");

    // Simple query
    auto simple = QueryBuilder().select("*").from("orders").build();
    std::cout << "Simple query:\n  " << simple.str() << "\n";
    assert(simple.str() == "SELECT * FROM orders");

    std::cout << "\nKey insight: Builder separates construction from representation.\n";
    std::cout << "Same builder process can create different query variations.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
