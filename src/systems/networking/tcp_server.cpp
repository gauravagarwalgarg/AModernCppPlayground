/**
 * @file tcp_server.cpp
 * @brief Simple TCP Echo Server - POSIX Sockets in Modern C++ (Linux Only)
 * 
 * Demonstrates the complete socket lifecycle:
 *   socket() → bind() → listen() → accept() → read()/write() → close()
 * 
 * Modern C++ additions:
 *   - RAII wrapper for file descriptors (no fd leak possible)
 *   - Scoped socket address setup
 *   - Clean error handling without goto
 * 
 * In production: would add non-blocking I/O, connection pooling, TLS.
 * For HFT: kernel bypass (DPDK), busy-polling, CPU pinning.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread tcp_server.cpp -o tcp_server
 * Test with: echo "hello" | nc localhost 8080
 */

#ifdef __linux__
#include <iostream>
#include <cstring>
#include <array>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// RAII wrapper for socket file descriptors - prevents fd leaks
class Socket {
    int fd_ = -1;
public:
    explicit Socket(int fd) noexcept : fd_(fd) {}
    Socket(int domain, int type, int protocol) : fd_(::socket(domain, type, protocol)) {
        if (fd_ < 0) throw std::runtime_error("socket() failed: " + std::string(strerror(errno)));
    }
    ~Socket() { if (fd_ >= 0) ::close(fd_); }

    // Non-copyable, movable
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept : fd_(other.fd_) { other.fd_ = -1; }
    Socket& operator=(Socket&& other) noexcept {
        if (this != &other) { if (fd_ >= 0) ::close(fd_); fd_ = other.fd_; other.fd_ = -1; }
        return *this;
    }

    int fd() const noexcept { return fd_; }
    explicit operator bool() const noexcept { return fd_ >= 0; }
};

int main() {
    std::cout << "=== TCP Echo Server Demo ===\n";
    constexpr int PORT = 8080;

    // Create socket
    Socket server_sock(AF_INET, SOCK_STREAM, 0);

    // Allow address reuse (avoid "address already in use" on restart)
    int opt = 1;
    setsockopt(server_sock.fd(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind to address
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_sock.fd(), reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "bind() failed: " << strerror(errno) << "\n";
        return 1;
    }

    // Listen for connections
    if (listen(server_sock.fd(), 5) < 0) {
        std::cerr << "listen() failed: " << strerror(errno) << "\n";
        return 1;
    }
    std::cout << "Listening on port " << PORT << "...\n";
    std::cout << "(Demo: accepting one connection then exiting)\n";
    std::cout << "Test with: echo 'hello' | nc localhost " << PORT << "\n\n";

    // Accept one connection (blocking)
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    Socket client_sock(accept(server_sock.fd(),
                              reinterpret_cast<sockaddr*>(&client_addr), &client_len));

    if (!client_sock) {
        std::cerr << "accept() failed: " << strerror(errno) << "\n";
        return 1;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "Client connected: " << client_ip << ":" << ntohs(client_addr.sin_port) << "\n";

    // Echo loop
    std::array<char, 1024> buffer{};
    ssize_t bytes_read = read(client_sock.fd(), buffer.data(), buffer.size() - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        std::cout << "Received: " << buffer.data();
        // Echo back
        write(client_sock.fd(), buffer.data(), bytes_read);
        std::cout << "Echoed " << bytes_read << " bytes back to client\n";
    }

    std::cout << "Connection closed. Server shutting down.\n";
    return 0;
    // RAII: both sockets automatically closed here
}

#else
#include <iostream>
int main() {
    std::cout << "This example requires Linux (POSIX sockets).\n";
    std::cout << "Compile and run on Linux/WSL.\n";
    return 0;
}
#endif
