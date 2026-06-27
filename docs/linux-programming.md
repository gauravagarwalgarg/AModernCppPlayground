# Linux Programming C++ Interview Guide

## Process vs Thread

| Aspect | Process | Thread |
|--------|---------|--------|
| Memory | Separate address space | Shared address space |
| Creation | `fork()` heavy (~ms) | `pthread_create` light (~μs) |
| Communication | IPC (pipes, shmem, sockets) | Shared memory (direct) |
| Crash isolation | One process crash won't kill others | One thread crash kills all |

## fork/exec Model

```cpp
pid_t pid = fork();      // Duplicates process (COW pages)
if (pid == 0) {
    execvp(prog, args);  // Replace child's image with new program
} else {
    waitpid(pid, &status, 0);  // Parent waits for child
}
```

**COW (Copy-on-Write):** Pages shared until written → efficient fork.

## Signals

- Asynchronous notifications to a process (`SIGTERM`, `SIGKILL`, `SIGSEGV`)
- `sigaction()` > `signal()` (portable, reliable)
- Signal handlers: only async-signal-safe functions (`write`, not `printf`)
- Block signals during critical sections with `sigprocmask`

→ `src/systems/linux/signal_handling.cpp`

## Memory-Mapped I/O (mmap)

```cpp
void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
// File access without read/write syscalls page-fault driven
```

Uses: shared memory IPC, file I/O, huge page allocation, memory-mapped databases.

→ `src/systems/linux/mmap_shared_memory.cpp`

## Futex (Fast Userspace Mutex)

- `FUTEX_WAIT`: sleep if `*addr == expected` (atomic check + sleep)
- `FUTEX_WAKE`: wake N waiters
- Foundation of `pthread_mutex`, `std::mutex` on Linux
- Fast path: atomic in user-space. Slow path: kernel syscall only on contention

## File Descriptors

- Integer handles to kernel objects (files, sockets, pipes, epoll, timers)
- Inheritance across `fork()` (unless `O_CLOEXEC`)
- Limit: `ulimit -n` (default 1024, increase for servers)
- `/proc/self/fd/` shows open FDs

## /proc Filesystem

| Path | Info |
|------|------|
| `/proc/[pid]/maps` | Virtual memory mappings |
| `/proc/[pid]/status` | Process state, memory usage |
| `/proc/[pid]/fd/` | Open file descriptors |
| `/proc/cpuinfo` | CPU topology, cache sizes |
| `/proc/meminfo` | System memory stats |

## Performance Tools

| Tool | Use |
|------|-----|
| `strace` | Trace syscalls (find bottlenecks, debug failures) |
| `perf stat` | CPU counters (cache misses, branch mispredictions) |
| `perf record` | Profile with flame graphs |
| `valgrind` | Memory leaks, use-after-free |
| `ltrace` | Library call tracing |

## cgroups & Namespaces

- **cgroups:** Resource limits (CPU, memory, I/O) for process groups → foundation of containers
- **Namespaces:** Isolation (PID, network, mount, user) → foundation of containers
- Together they enable Docker/LXC without full VM overhead
