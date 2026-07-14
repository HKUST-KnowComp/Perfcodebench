#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstdint>

namespace {

// A simple spin-barrier to synchronize threads between iterations without the overhead of std::barrier.
struct SpinBarrier {
    std::atomic<int> count;
    std::atomic<int> generation;
    int num_threads;

    SpinBarrier(int n) : count(n), generation(0), num_threads(n) {}

    void wait() {
        int gen = generation.load(std::memory_order_acquire);
        if (--count == 0) {
            count.store(num_threads, std::memory_order_relaxed);
            generation.fetch_add(1, std::memory_order_release);
        } else {
            while (generation.load(std::memory_order_acquire) == gen) {
                std::atomic_thread_fence(std::memory_order_acquire);
            }
        }
    }
};

uint64_t checksum_u64(const uint64_t* counts, uint32_t key_space) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < key_space; ++i) {
        hash ^= counts[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
    
    if (iters <= 0) return 0;

    // Use a raw array of atomics for the counters to avoid mutex contention.
    // We use uint64_t to match the baseline's count type.
    std::vector<std::atomic<uint64_t>> counts(key_space);
    for (uint32_t i = 0; i < key_space; ++i) counts[i].store(0, std::memory_order_relaxed);

    SpinBarrier barrier(num_threads);
    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    const std::size_t n = keys.size();
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
        threads.emplace_back([&, t]() {
            const std::size_t begin = static_cast<std::size_t>(t) * chunk;
            const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;

            for (int iter = 0; iter < iters; ++iter) {
                // Reset counts at the start of each iteration.
                // Only one thread needs to do the full clear, or we split it.
                std::size_t c_chunk = (key_space + num_threads - 1) / num_threads;
                std::size_t c_begin = std::min<std::size_t>(key_space, t * c_chunk);
                std::size_t c_end = std::min<std::size_t>(key_space, (t + 1) * c_chunk);
                for (std::size_t i = c_begin; i < c_end; ++i) {
                    counts[i].store(0, std::memory_order_relaxed);
                }
                
                barrier.wait();

                // Perform updates using atomic fetch_add.
                for (std::size_t i = begin; i < end; ++i) {
                    counts[keys[i]].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
                }

                barrier.wait();
            }
        });
    }

    for (auto& th : threads) {
        th.join();
    }

    // Final checksum calculation.
    std::vector<uint64_t> final_counts(key_space);
    for (uint32_t i = 0; i < key_space; ++i) {
        final_counts[i] = counts[i].load(std::memory_order_relaxed);
    }

    return checksum_u64(final_counts.data(), key_space);
}
