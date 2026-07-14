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

}  // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  
  // Use atomics to allow concurrent updates without a global mutex.
  // std::atomic<uint64_t> ensures correctness across threads.
  std::vector<std::atomic<uint64_t>> atomic_counts(static_cast<std::size_t>(key_space));
  std::vector<uint64_t> final_counts(static_cast<std::size_t>(key_space));
  
  uint64_t total_hash = 0;
  const std::size_t n_keys = keys.size();
  const std::size_t chunk_size = (n_keys + num_threads - 1) / num_threads;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for each iteration
    for (std::size_t i = 0; i < atomic_counts.size(); ++i) {
      atomic_counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk_size;
      const std::size_t end = std::min(begin + chunk_size, n_keys);
      
      if (begin >= n_keys) break;

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use relaxed memory order as we only care about the final sum
          // and there are no dependencies between different keys.
          atomic_counts[keys[i]].fetch_add(values[i], std::memory_order_relaxed);
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Collect results into a non-atomic vector for checksumming
    for (std::size_t i = 0; i < atomic_counts.size(); ++i) {
      final_counts[i] = atomic_counts[i].load(std::memory_order_relaxed);
    }
    
    total_hash = checksum_u64(final_counts);
  }

  return total_hash;
}
