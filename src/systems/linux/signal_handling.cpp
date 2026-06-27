/**
 * @file signal_handling.cpp
 * @brief Signal Handling - Graceful Shutdown Pattern
 * 
 * Key concepts:
 *   - sigaction() vs signal(): sigaction is portable, doesn't reset handler, blocks signals
 *     during handler execution. Always prefer sigaction() in production.
 *   - Signal-safe functions: only async-signal-safe functions in handlers
 *     (write() ok, printf()/malloc()/cout NOT ok)
 *   - Volatile sig_atomic_t: the only safe way to communicate from handler to main loop
 *   - Graceful shutdown: set flag in handler, check in main loop, cleanup, exit
 * 
 * Common signals: SIGINT (Ctrl+C), SIGTERM (kill), SIGUSR1/2 (app-defined)
 * 
 * Compile: g++ -std=c++20 -Wall -pthread signal_handling.cpp -o signal_handling
 */

#ifdef __linux__
#include <iostream>
#include <csignal>
#include <unistd.h>
#include <cstring>
#include <atomic>

// volatile sig_atomic_t is the ONLY safe type for signal handlers
// (std::atomic also works on most platforms but isn't guaranteed signal-safe)
volatile sig_atomic_t g_shutdown_requested = 0;
volatile sig_atomic_t g_reload_requested = 0;

// Signal handlers - MUST only use async-signal-safe functions
void handleSIGINT(int /*sig*/) {
    // write() is signal-safe, cout/printf are NOT
    const char msg[] = "\n[SIGNAL] SIGINT received - requesting shutdown\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    g_shutdown_requested = 1;
}

void handleSIGTERM(int /*sig*/) {
    const char msg[] = "[SIGNAL] SIGTERM received - requesting shutdown\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    g_shutdown_requested = 1;
}

void handleSIGUSR1(int /*sig*/) {
    const char msg[] = "[SIGNAL] SIGUSR1 received - config reload requested\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    g_reload_requested = 1;
}

void installSignalHandlers() {
    struct sigaction sa{};

    // SIGINT handler
    sa.sa_handler = handleSIGINT;
    sigemptyset(&sa.sa_mask);         // Don't block other signals during handler
    sa.sa_flags = 0;                  // No special flags (SA_RESTART if you want syscall restart)
    sigaction(SIGINT, &sa, nullptr);

    // SIGTERM handler
    sa.sa_handler = handleSIGTERM;
    sigaction(SIGTERM, &sa, nullptr);

    // SIGUSR1 handler
    sa.sa_handler = handleSIGUSR1;
    sigaction(SIGUSR1, &sa, nullptr);

    // Ignore SIGPIPE (common in network servers)
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, nullptr);
}

int main() {
    std::cout << "=== Signal Handling Demo ===\n";
    std::cout << "PID: " << getpid() << "\n";
    std::cout << "Press Ctrl+C or send: kill -SIGUSR1 " << getpid() << "\n";
    std::cout << "Running for 3 iterations to demonstrate...\n\n";

    installSignalHandlers();

    // Simulate: send ourselves SIGUSR1 for demo
    kill(getpid(), SIGUSR1);

    int iteration = 0;
    while (!g_shutdown_requested && iteration < 3) {
        // Check for reload request
        if (g_reload_requested) {
            std::cout << "[MAIN] Reloading configuration...\n";
            g_reload_requested = 0;
        }

        std::cout << "[MAIN] Working... iteration " << ++iteration << "\n";
        sleep(1);
    }

    // Graceful shutdown
    std::cout << "\n[MAIN] Performing graceful shutdown...\n";
    std::cout << "[MAIN] Flushing buffers...\n";
    std::cout << "[MAIN] Closing connections...\n";
    std::cout << "[MAIN] Shutdown complete.\n";

    return 0;
}

#else
#include <iostream>
int main() {
    std::cout << "Signal handling demo requires Linux/POSIX.\n";
    return 0;
}
#endif
