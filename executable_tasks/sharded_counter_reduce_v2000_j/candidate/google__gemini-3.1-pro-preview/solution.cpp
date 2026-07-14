#include "interface.h"

#include <vector>
#include <thread>

namespace {

/**
 * FNV-1a hash for checksum calculation as per the baseline.
 */
uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

}

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  
  std::vector<uint64_t> global_counts(key_space, 0);
  uint64_t final_hash = 0;
  size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);

    // To avoid contention on a single mutex or atomic, we use thread-local buffers.
    // Each thread accumulates its portion of the input into its own local count array.
    std::vector<std::thread> threads;
    std::vector<std::vector<uint64_t>> local_buffers(num_threads, std::vector<uint64_t>(key_space, 0));

    size_t chunk_size = n / num_threads;
    for (int t = 0; t < num_threads; ++t) {
      size_t start = t * chunk_size;
      size_t end = (t == num_threads - 1) ? n : (t + 1) * chunk_size;

      threads.emplace_back([&, t, start, end]() {
        auto& local_counts = local_buffers[t];
        for (size_t i = start; i < end; ++i) {
          local_counts[keys[i]] += values[i];
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Merge local buffers into the global counts.
    // Since we need the checksum of the final state, we sum them up.
    // This part is single-threaded but efficient as it's a simple linear scan.
    for (int t = 0; t < num_threads; ++t) {
      const auto& local_counts = local_buffers[t];
      for (uint32_t k = 0; k < key_space; ++k) {
        global_counts[k] += local_counts[k];
      }
    }

    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}