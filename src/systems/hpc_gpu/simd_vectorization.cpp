/**
 * @file simd_vectorization.cpp
 * @brief SIMD Concepts - Vectorization, Cache-Friendly Access, SoA vs AoS
 * 
 * Key concepts:
 *   1. Loop vectorization: compiler auto-vectorizes simple loops (with -O2)
 *      Manual hints: restrict pointers, alignment, trip count known
 *   2. SoA (Struct of Arrays) vs AoS (Array of Structs):
 *      - AoS: struct{x,y,z} arr[N] - bad for SIMD (scattered access)
 *      - SoA: float x[N], y[N], z[N] - SIMD-friendly (contiguous access)
 *   3. Cache-friendly access: sequential > strided > random
 *   4. __builtin_expect: branch prediction hints
 * 
 * Modern CPUs process 4-8 floats simultaneously (SSE=4, AVX=8, AVX-512=16).
 * Proper data layout = free 4-16x speedup.
 * 
 * Compile: g++ -std=c++20 -Wall -O2 -pthread simd_vectorization.cpp -o simd_vectorization
 */

#include <iostream>
#include <vector>
#include <chrono>
#include <numeric>
#include <cmath>
#include <random>

// === Array of Structs (AoS) - Traditional, cache-unfriendly for bulk ops ===
struct Particle_AoS {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    float padding; // Align to 32 bytes
};

// === Struct of Arrays (SoA) - SIMD-friendly, contiguous memory per field ===
struct Particles_SoA {
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;

    explicit Particles_SoA(size_t n) : x(n), y(n), z(n), vx(n), vy(n), vz(n), mass(n) {}
};

// Dot product - simple loop the compiler can auto-vectorize
float dotProduct(const float* __restrict__ a, const float* __restrict__ b, size_t n) {
    float sum = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        sum += a[i] * b[i];  // Compiler emits SIMD: vmulps + vaddps
    }
    return sum;
}

// Update positions - AoS version (poor vectorization: x,y,z interleaved)
void updatePositions_AoS(std::vector<Particle_AoS>& particles, float dt) {
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.z += p.vz * dt;
    }
}

// Update positions - SoA version (great vectorization: contiguous arrays)
void updatePositions_SoA(Particles_SoA& p, float dt, size_t n) {
    // Each loop processes 8 elements per cycle (AVX)
    for (size_t i = 0; i < n; ++i) p.x[i] += p.vx[i] * dt;
    for (size_t i = 0; i < n; ++i) p.y[i] += p.vy[i] * dt;
    for (size_t i = 0; i < n; ++i) p.z[i] += p.vz[i] * dt;
}

template<typename Func>
double measureUs(Func&& fn, int iterations = 100) {
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) fn();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::micro>(end - start).count() / iterations;
}

int main() {
    std::cout << "=== SIMD & Vectorization Concepts ===\n\n";
    constexpr size_t N = 100'000;

    // --- Dot Product Demo ---
    std::vector<float> a(N, 1.0f), b(N, 2.0f);
    float result = dotProduct(a.data(), b.data(), N);
    std::cout << "Dot product (" << N << " elements): " << result << "\n";

    double dot_time = measureUs([&]() { dotProduct(a.data(), b.data(), N); });
    std::cout << "  Time: " << dot_time << " µs\n\n";

    // --- AoS vs SoA Benchmark ---
    std::cout << "--- AoS vs SoA Position Update (" << N << " particles) ---\n";
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    // Setup AoS
    std::vector<Particle_AoS> aos(N);
    for (auto& p : aos) {
        p.x = dist(rng); p.y = dist(rng); p.z = dist(rng);
        p.vx = dist(rng); p.vy = dist(rng); p.vz = dist(rng);
    }

    // Setup SoA
    Particles_SoA soa(N);
    for (size_t i = 0; i < N; ++i) {
        soa.x[i] = dist(rng); soa.y[i] = dist(rng); soa.z[i] = dist(rng);
        soa.vx[i] = dist(rng); soa.vy[i] = dist(rng); soa.vz[i] = dist(rng);
    }

    float dt = 0.016f;
    double aos_time = measureUs([&]() { updatePositions_AoS(aos, dt); });
    double soa_time = measureUs([&]() { updatePositions_SoA(soa, dt, N); });

    std::cout << "  AoS time: " << aos_time << " µs\n";
    std::cout << "  SoA time: " << soa_time << " µs\n";
    std::cout << "  Speedup:  " << (aos_time / soa_time) << "x\n\n";

    // --- Branch Prediction Hints ---
    std::cout << "--- __builtin_expect (branch hints) ---\n";
    int error_count = 0;
    auto process = [&](int value) {
        if (__builtin_expect(value < 0, 0)) {  // Hint: rarely true
            ++error_count;  // Cold path
        }
        // Hot path continues here
    };
    for (int i = 0; i < 1000; ++i) process(i);
    std::cout << "  Errors (expected 0): " << error_count << "\n";
    std::cout << "  __builtin_expect helps CPU branch predictor on cold paths\n\n";

    std::cout << "=== Key Takeaways ===\n";
    std::cout << "1. SoA layout enables SIMD auto-vectorization\n";
    std::cout << "2. Contiguous memory = cache-line friendly\n";
    std::cout << "3. __restrict__ tells compiler no aliasing → better codegen\n";
    std::cout << "4. Compile with -O2 -march=native for auto-vectorization\n";

    return 0;
}
