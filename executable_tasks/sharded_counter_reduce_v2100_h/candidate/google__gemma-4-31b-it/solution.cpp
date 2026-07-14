#include "interface.h"

#include <vector>
#include <thread>
#include <atomic>
#include <cstring>

namespace {

uint64_t checksum_u64(const uint64_t* counts, uint32_t key_space) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t i = 0; i < key_space; ++i) {
        hash ^= counts[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
    
    const size_t n = keys.size();
    const size_t ks = static_cast<size_t>(key_space);
    uint64_t final_hash = 0;

    // Pre-allocate a buffer for the global counts to avoid repeated allocations
    std::vector<uint64_t> global_counts(ks);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(global_counts.begin(), global_counts.end(), 0);
        
        std::vector<std::thread> workers;
        workers.reserve(num_threads);

        for (int t = 0; t < num_threads; ++t) {
            workers.emplace_back([&, t]() {
                // Thread-local storage to avoid contention and false sharing
                std::vector<uint64_t> local_counts(ks, 0);
                
                size_t start = (n * t) / num_threads;
                size_t end = (n * (t + 1)) / num_threads;
                
                for (size_t i = start; i < end; ++i) {
                    local_counts[keys[i]] += values[i];
                }

                // Merge local results into global counts using atomics or a mutex
                // Since we only do this once per thread per iteration, we can use a simple lock
                // or just use atomics for the merge phase.
                static std::mutex merge_mutex;
                std::lock_guard<std::mutex> lock(merge_mutex);
                for (size_t i = 0; i < ks; ++i) {
                    global_counts[i] += local_counts[i];
                }
            });
        }

        for (auto& worker : workers) {
            worker.join();
        }

        final_hash = checksum_u64(global_counts.data(), key_space);
    }

    return final_hash;
}