/**
 * @file scheduler_sim.cpp
 * @brief Process Scheduler Simulation - Round Robin & Priority Scheduling
 * 
 * Algorithms implemented:
 *   1. Round Robin (RR): Each process gets a time quantum, preempted if not done.
 *      Fair, good response time, used in general-purpose OS (Linux CFS is weighted RR).
 *   2. Priority Scheduling: Highest priority runs first (preemptive).
 *      Risk: starvation of low-priority processes (solved with aging).
 * 
 * Metrics:
 *   - Turnaround time: completion_time - arrival_time (total time in system)
 *   - Waiting time: turnaround_time - burst_time (time spent NOT running)
 *   - Response time: first_run_time - arrival_time
 * 
 * Compile: g++ -std=c++20 -Wall -pthread scheduler_sim.cpp -o scheduler_sim
 */

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <numeric>
#include <iomanip>

struct Process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;       // Lower number = higher priority
    int remaining_time;
    int completion_time = 0;
    int first_run_time = -1;
};

struct SchedulerResult {
    std::vector<Process> processes;
    double avg_turnaround;
    double avg_waiting;
};

// Round Robin Scheduler
SchedulerResult roundRobin(std::vector<Process> procs, int quantum) {
    std::queue<int> ready_queue;
    int n = procs.size();
    int current_time = 0, completed = 0;
    std::vector<bool> in_queue(n, false);

    // Sort by arrival time
    std::sort(procs.begin(), procs.end(),
              [](const auto& a, const auto& b) { return a.arrival_time < b.arrival_time; });

    // Add initially arrived processes
    for (int i = 0; i < n; ++i) {
        if (procs[i].arrival_time <= current_time) {
            ready_queue.push(i);
            in_queue[i] = true;
        }
    }

    while (completed < n) {
        if (ready_queue.empty()) {
            ++current_time;
            for (int i = 0; i < n; ++i) {
                if (!in_queue[i] && procs[i].arrival_time <= current_time && procs[i].remaining_time > 0) {
                    ready_queue.push(i);
                    in_queue[i] = true;
                }
            }
            continue;
        }

        int idx = ready_queue.front();
        ready_queue.pop();

        if (procs[idx].first_run_time < 0)
            procs[idx].first_run_time = current_time;

        int run_time = std::min(quantum, procs[idx].remaining_time);
        procs[idx].remaining_time -= run_time;
        current_time += run_time;

        // Check for newly arrived processes
        for (int i = 0; i < n; ++i) {
            if (!in_queue[i] && procs[i].arrival_time <= current_time && procs[i].remaining_time > 0) {
                ready_queue.push(i);
                in_queue[i] = true;
            }
        }

        if (procs[idx].remaining_time == 0) {
            procs[idx].completion_time = current_time;
            ++completed;
        } else {
            ready_queue.push(idx);  // Back to queue
        }
    }

    // Calculate metrics
    double total_tat = 0, total_wt = 0;
    for (const auto& p : procs) {
        int tat = p.completion_time - p.arrival_time;
        int wt = tat - p.burst_time;
        total_tat += tat;
        total_wt += wt;
    }
    return {procs, total_tat / n, total_wt / n};
}

// Priority Scheduler (Non-preemptive)
SchedulerResult priorityScheduling(std::vector<Process> procs) {
    int n = procs.size();
    int current_time = 0, completed = 0;
    std::vector<bool> done(n, false);

    while (completed < n) {
        int best = -1;
        for (int i = 0; i < n; ++i) {
            if (!done[i] && procs[i].arrival_time <= current_time) {
                if (best == -1 || procs[i].priority < procs[best].priority)
                    best = i;
            }
        }

        if (best == -1) { ++current_time; continue; }

        procs[best].first_run_time = current_time;
        current_time += procs[best].burst_time;
        procs[best].completion_time = current_time;
        procs[best].remaining_time = 0;
        done[best] = true;
        ++completed;
    }

    double total_tat = 0, total_wt = 0;
    for (const auto& p : procs) {
        int tat = p.completion_time - p.arrival_time;
        int wt = tat - p.burst_time;
        total_tat += tat;
        total_wt += wt;
    }
    return {procs, total_tat / n, total_wt / n};
}

void printResults(const std::string& name, const SchedulerResult& result) {
    std::cout << "\n=== " << name << " ===\n";
    std::cout << std::setw(5) << "PID" << std::setw(10) << "Burst"
              << std::setw(10) << "Priority" << std::setw(12) << "Completion"
              << std::setw(12) << "Turnaround" << std::setw(10) << "Waiting" << "\n";

    for (const auto& p : result.processes) {
        int tat = p.completion_time - p.arrival_time;
        int wt = tat - p.burst_time;
        std::cout << std::setw(5) << p.pid << std::setw(10) << p.burst_time
                  << std::setw(10) << p.priority << std::setw(12) << p.completion_time
                  << std::setw(12) << tat << std::setw(10) << wt << "\n";
    }
    std::cout << "Avg Turnaround: " << std::fixed << std::setprecision(2)
              << result.avg_turnaround << "\n";
    std::cout << "Avg Waiting:    " << result.avg_waiting << "\n";
}

int main() {
    std::cout << "=== Process Scheduler Simulation ===\n";

    // Test processes: {pid, arrival, burst, priority, remaining}
    std::vector<Process> processes = {
        {1, 0, 10, 3, 10},
        {2, 1,  5, 1,  5},
        {3, 2,  8, 4,  8},
        {4, 3,  3, 2,  3},
        {5, 4,  6, 5,  6},
    };

    // Round Robin with quantum = 3
    auto rr_result = roundRobin(processes, 3);
    printResults("Round Robin (quantum=3)", rr_result);

    // Priority Scheduling
    auto pr_result = priorityScheduling(processes);
    printResults("Priority Scheduling (non-preemptive)", pr_result);

    // Comparison
    std::cout << "\n=== Comparison ===\n";
    std::cout << "RR is fairer (bounded wait), Priority gives better throughput for important tasks.\n";
    std::cout << "Linux CFS = weighted Round Robin with virtual runtime (vruntime).\n";

    return 0;
}
