#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <algorithm>

namespace {

uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
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
  // Use atomics to allow concurrent updates without a global mutex.
  // We use a vector of atomics to avoid the overhead of a mutex per bucket.
  std::vector<std::atomic<uint64_t>> atomic_counts(static_cast<std::size_t>(key_space));
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset atomics for the new iteration
    for (auto& count : atomic_counts) {
      count.store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    const std::size_t total_elements = keys.size();
    const std::size_t chunk_size = (total_elements + num_threads - 1) / num_threads;

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk_size;
      if (begin >= total_elements) break;
      const std::size_t end = std::min(begin + chunk_size, total_elements);

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use fetch_add for atomic increment. 
          // memory_order_relaxed is sufficient here as we only care about the final sum
          // and there are no dependencies between iterations or threads other than the sum.
          atomic_counts[static_cast<std::size_t>(keys[i])].fetch_add(
              static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Convert atomics to a standard vector for the checksum function
    std::vector<uint64_t> counts_snapshot(static_cast<std::size_t>(key_space));
    for (std::size_t i = 0; i < static_cast<std::size_t>(key_space); ++i) {
      counts_snapshot[i] = atomic_counts[i].load(std::memory_order_relaxed);
    }
    
    final_hash = checksum_u64(counts_snapshot);
  }

  return final_hash;
}