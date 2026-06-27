/**************************************************************************************************
 * Topic: Mediator Pattern Reduce Coupling Between Components
 *
 * Interview Relevance: ★★★
 * What interviewers look for:
 *   - Centralizing complex communication logic
 *   - Components only know the mediator, not each other
 *   - Real-world: GUI event dispatchers, chat rooms, air traffic control
 *   - Trade-off: mediator can become a god object
 *
 * Compile: g++ -std=c++20 -Wall -Wextra -o mediator mediator.cpp
 **************************************************************************************************/

#include <cassert>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// ============================================================================
// Modern Mediator: Event Bus (type-safe, decoupled)
// ============================================================================

class EventBus {
public:
    using Handler = std::function<void(std::string const&)>;
    using SubscriptionId = uint64_t;

    SubscriptionId subscribe(std::string const& event, Handler handler) {
        auto id = next_id_++;
        handlers_[event].push_back({id, std::move(handler)});
        return id;
    }

    void unsubscribe(SubscriptionId id) {
        for (auto& [event, handlers] : handlers_) {
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                               [id](auto const& h) { return h.id == id; }),
                handlers.end());
        }
    }

    void publish(std::string const& event, std::string const& data = "") {
        if (auto it = handlers_.find(event); it != handlers_.end()) {
            for (auto const& [id, handler] : it->second) {
                handler(data);
            }
        }
    }

private:
    struct Subscription {
        SubscriptionId id;
        Handler handler;
    };
    std::unordered_map<std::string, std::vector<Subscription>> handlers_;
    SubscriptionId next_id_{0};
};

// ============================================================================
// Components only know the EventBus (mediator), not each other
// ============================================================================

class AuthService {
public:
    explicit AuthService(EventBus& bus) : bus_(bus) {
        bus_.subscribe("user:login_request", [this](std::string const& data) {
            authenticate(data);
        });
    }

    void authenticate(std::string const& username) {
        std::cout << "  [Auth] Authenticating: " << username << '\n';
        // Simulate auth check
        if (username == "admin" || username == "alice") {
            bus_.publish("user:authenticated", username);
            ++auth_count_;
        } else {
            bus_.publish("user:auth_failed", username);
        }
    }

    int auth_count() const { return auth_count_; }

private:
    EventBus& bus_;
    int auth_count_{0};
};

class LoggingService {
public:
    explicit LoggingService(EventBus& bus) {
        bus.subscribe("user:authenticated", [this](std::string const& user) {
            std::cout << "  [Log] User logged in: " << user << '\n';
            ++log_count_;
        });
        bus.subscribe("user:auth_failed", [this](std::string const& user) {
            std::cout << "  [Log] FAILED login attempt: " << user << '\n';
            ++log_count_;
        });
    }

    int log_count() const { return log_count_; }

private:
    int log_count_{0};
};

class NotificationService {
public:
    explicit NotificationService(EventBus& bus) {
        bus.subscribe("user:authenticated", [this](std::string const& user) {
            std::cout << "  [Notify] Welcome back, " << user << "!\n";
            ++notify_count_;
        });
    }

    int notify_count() const { return notify_count_; }

private:
    int notify_count_{0};
};

// ============================================================================
// Demonstration
// ============================================================================

int main() {
    std::cout << "=== Mediator Pattern (Event Bus) ===\n\n";

    EventBus bus;

    AuthService auth(bus);
    LoggingService logger(bus);
    NotificationService notifier(bus);

    // Simulate login attempts
    std::cout << "Login attempt: alice\n";
    bus.publish("user:login_request", "alice");

    std::cout << "\nLogin attempt: hacker\n";
    bus.publish("user:login_request", "hacker");

    std::cout << "\nLogin attempt: admin\n";
    bus.publish("user:login_request", "admin");

    // Assertions
    assert(auth.auth_count() == 2);       // alice + admin succeeded
    assert(logger.log_count() == 3);      // all 3 attempts logged
    assert(notifier.notify_count() == 2); // only successful logins notified

    std::cout << "\nAll assertions passed.\n";
    return EXIT_SUCCESS;
}
