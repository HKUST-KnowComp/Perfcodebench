#include "interface.h"

#include <algorithm>
#include <thread>
#include <vector>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
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
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);

  // Per-thread accumulators to eliminate lock contention
  std::vector<std::vector<uint64_t>> local_counts(
      static_cast<std::size_t>(num_threads),
      std::vector<uint64_t>(static_cast<std::size_t>(key_space), 0));

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts for this iteration
    std::fill(counts.begin(), counts.end(), 0);

    // Reset thread-local counters (needed only after the first iteration,
    // but unconditional fill keeps the logic simple and is cheap for small key_space)
    for (int t = 0; t < num_threads; ++t) {
      std::fill(local_counts[t].begin(), local_counts[t].end(), 0);
    }

    // Partition the work and run threads
    std::vector<std::thread> threads;
    const std::size_t total_keys = keys.size();
    const std::size_t chunk = total_keys / static_cast<std::size_t>(num_threads);
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? total_keys : begin + chunk;
      threads.emplace_back([&, t, begin, end]() {
        auto& local = local_counts[static_cast<std::size_t>(t)];
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] +=
              static_cast<uint64_t>(values[i]);
        }
      });
    }

    // Wait for all threads to finish
    for (auto& th : threads) {
      th.join();
    }

    // Reduce: merge local accumulators into the global array
    for (int t = 0; t < num_threads; ++t) {
      const auto& local = local_counts[static_cast<std::size_t>(t)];
      for (std::size_t k = 0; k < key_space; ++k) {
        counts[k] += local[k];
      }
    }

    // Compute checksum of the final counter state for this iteration
    hash = checksum_u64(counts);
  }

  return hash;
}