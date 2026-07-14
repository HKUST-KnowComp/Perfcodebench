#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <numeric>

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
  
  // Use atomic counters to eliminate the global mutex bottleneck
  std::vector<std::atomic<uint64_t>> atomic_counts(static_cast<std::size_t>(key_space));
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counters for each iteration
    for (uint32_t i = 0; i < key_space; ++i) {
      atomic_counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    const std::size_t total_keys = keys.size();
    const std::size_t chunk = total_keys / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t == num_threads - 1) ? total_keys : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use relaxed memory order as we only need atomicity of the addition,
          // and the join() provides the necessary synchronization for the checksum.
          atomic_counts[keys[i]].fetch_add(values[i], std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Convert atomics to a standard vector for the checksum function
    std::vector<uint64_t> counts(static_cast<std::size_t>(key_space));
    for (uint32_t i = 0; i < key_space; ++i) {
      counts[i] = atomic_counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(counts);
  }

  return final_hash;
}