/**
 * @file epoll_server.cpp
 * @brief Event-Driven Server with epoll - Scalable I/O for HFT
 * 
 * epoll vs select/poll:
 *   - select: O(n) scan of all fds, limited to FD_SETSIZE (1024)
 *   - poll: O(n) scan, no fd limit
 *   - epoll: O(1) for events, kernel maintains interest list, scales to millions of fds
 * 
 * Key concepts:
 *   - Edge-triggered (EPOLLET): notify once per state change (must drain fully)
 *   - Level-triggered (default): notify while data available (simpler but more syscalls)
 *   - Non-blocking sockets: never block the event loop
 * 
 * This is the foundation of nginx, Redis, and HFT network stacks.
 * 
 * Compile: g++ -std=c++20 -Wall -pthread epoll_server.cpp -o epoll_server
 * Test: multiple `nc localhost 9090` in separate terminals
 */

#ifdef __linux__
#include <iostream>
#include <cstring>
#include <array>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

// RAII file descriptor
class FileDescriptor {
    int fd_ = -1;
public:
    explicit FileDescriptor(int fd = -1) noexcept : fd_(fd) {}
    ~FileDescriptor() { if (fd_ >= 0) ::close(fd_); }
    FileDescriptor(FileDescriptor&& o) noexcept : fd_(o.fd_) { o.fd_ = -1; }
    FileDescriptor& operator=(FileDescriptor&& o) noexcept {
        if (fd_ >= 0) ::close(fd_);
        fd_ = o.fd_;
        o.fd_ = -1;
        return *this;
    }
    FileDescriptor(const FileDescriptor&) = delete;
    FileDescriptor& operator=(const FileDescriptor&) = delete;
    int fd() const noexcept { return fd_; }
    int release() noexcept { int f = fd_; fd_ = -1; return f; }
};

void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    std::cout << "=== Epoll Event-Driven Server Demo ===\n";
    constexpr int PORT = 9090;
    constexpr int MAX_EVENTS = 64;

    // Create listening socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }
    FileDescriptor server_guard(server_fd);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setNonBlocking(server_fd);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }
    if (listen(server_fd, SOMAXCONN) < 0) { perror("listen"); return 1; }

    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) { perror("epoll_create1"); return 1; }
    FileDescriptor epoll_guard(epoll_fd);

    // Register server socket for incoming connections
    epoll_event ev{};
    ev.events = EPOLLIN;  // Level-triggered read
    ev.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);

    std::cout << "Listening on port " << PORT << " with epoll...\n";
    std::cout << "Test: nc localhost " << PORT << " (multiple terminals)\n";
    std::cout << "(Will handle 5 events then exit for demo)\n\n";

    std::array<epoll_event, MAX_EVENTS> events{};
    int handled = 0;

    while (handled < 5) {
        int n = epoll_wait(epoll_fd, events.data(), MAX_EVENTS, 5000); // 5s timeout
        if (n < 0) { perror("epoll_wait"); break; }
        if (n == 0) { std::cout << "Timeout, exiting demo.\n"; break; }

        for (int i = 0; i < n; ++i) {
            if (events[i].data.fd == server_fd) {
                // New connection
                sockaddr_in client_addr{};
                socklen_t len = sizeof(client_addr);
                int client_fd = accept(server_fd,
                    reinterpret_cast<sockaddr*>(&client_addr), &len);
                if (client_fd < 0) continue;

                setNonBlocking(client_fd);
                epoll_event cev{};
                cev.events = EPOLLIN | EPOLLET;  // Edge-triggered for client
                cev.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &cev);

                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
                std::cout << "New connection: " << ip << " (fd=" << client_fd << ")\n";
            } else {
                // Data from existing client
                char buf[512];
                ssize_t bytes = read(events[i].data.fd, buf, sizeof(buf) - 1);
                if (bytes <= 0) {
                    // Client disconnected
                    std::cout << "Client disconnected (fd=" << events[i].data.fd << ")\n";
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    close(events[i].data.fd);
                } else {
                    buf[bytes] = '\0';
                    std::cout << "fd=" << events[i].data.fd << " sent: " << buf;
                    // Echo back
                    write(events[i].data.fd, buf, bytes);
                }
                ++handled;
            }
        }
    }

    std::cout << "Epoll server demo complete.\n";
    return 0;
}

#else
#include <iostream>
int main() {
    std::cout << "epoll is Linux-specific. Compile on Linux/WSL.\n";
    return 0;
}
#endif
