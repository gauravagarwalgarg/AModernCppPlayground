# Networking C++ Interview Guide

## TCP vs UDP

| Aspect | TCP | UDP |
|--------|-----|-----|
| Connection | Connection-oriented (3-way handshake) | Connectionless |
| Reliability | Guaranteed delivery, ordering | Best-effort, no ordering |
| Overhead | Higher (headers, ACKs, retransmits) | Minimal |
| Use case | HTTP, file transfer, order submission | Market data feed, gaming, DNS |
| HFT choice | Order entry (must be reliable) | Market data multicast (speed > reliability) |

## Socket Lifecycle (TCP)

```
Server: socket() → bind() → listen() → accept() → read/write → close()
Client: socket() → connect() → read/write → close()
```

→ `src/systems/networking/tcp_server.cpp`

## I/O Multiplexing

| Method | Max FDs | Mechanism | Performance |
|--------|---------|-----------|-------------|
| `select` | 1024 (FD_SETSIZE) | Bitmap scan | O(n) per call |
| `poll` | Unlimited | Array of pollfd | O(n) per call |
| `epoll` | Unlimited | Kernel event list | O(1) for ready events |

**epoll edge-triggered vs level-triggered:**
- Level: notifies while data available (safe, may repeat)
- Edge: notifies only on state change (must drain buffer, more efficient)

→ `src/systems/networking/epoll_server.cpp`

## Non-Blocking I/O

```cpp
fcntl(fd, F_SETFL, O_NONBLOCK);
// read() returns EAGAIN/EWOULDBLOCK when no data ready
// Use with epoll for event-driven architecture
```

## Zero-Copy Techniques

| Technique | Description | Use Case |
|-----------|-------------|----------|
| `sendfile()` | Kernel copies file→socket directly | Static file serving |
| `splice()` | Move data between FDs via pipe (no user-space copy) | Proxy servers |
| `mmap` + `write` | Map file to memory, write mapped region | Large file transfer |
| `MSG_ZEROCOPY` | Socket sends from user buffer without copying | High-throughput networking |

## Kernel Bypass (Ultra-Low-Latency)

| Technology | Approach | Latency |
|------------|----------|---------|
| DPDK | User-space NIC driver, poll-mode | ~1-2μs |
| Solarflare OpenOnload | ef_vi, kernel bypass via special NIC | ~2-5μs |
| RDMA | Direct memory access across network | ~1μs |
| XDP/eBPF | Programmable packet processing in kernel | ~5μs |

Kernel bypass eliminates: syscall overhead, context switches, kernel TCP stack.

## Common Interview Questions

| Question | Key Points |
|----------|------------|
| TCP 3-way handshake | SYN → SYN+ACK → ACK. Why 3? Confirm both directions work |
| TIME_WAIT purpose | Ensure late packets don't corrupt new connections. 2*MSL duration |
| Nagle's algorithm | Coalesces small sends. Disable with `TCP_NODELAY` for low-latency |
| What happens when you call `connect()`? | DNS → ARP → SYN sent → wait for SYN+ACK (blocks or EINPROGRESS) |
| Design a chat server | epoll + non-blocking, thread-per-core, message framing |
| How does `epoll` scale? | Red-black tree for monitoring, ready list for events. O(1) per event |
| Multicast vs broadcast | Multicast = specific group (IGMP). Broadcast = all on subnet |
