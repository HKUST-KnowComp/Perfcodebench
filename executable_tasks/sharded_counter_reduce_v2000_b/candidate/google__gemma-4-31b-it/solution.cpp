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
  
  // Use atomics to allow concurrent updates to different buckets
  // We use a vector of atomics. Note: std::vector<std::atomic<T>> is not allowed 
  // because atomic is not CopyInsertable. We use a unique_ptr or a raw array.
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space));
  
  uint64_t final_hash = 0;
  const std::size_t total_keys = keys.size();
  const std::size_t chunk = total_keys / static_cast<std::size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts for this iteration
    for (uint32_t i = 0; i < key_space; ++i) {
      counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t == num_threads - 1) ? total_keys : begin + chunk;
      
      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Use relaxed memory order for maximum throughput
          counts[keys[i]].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Collect results into a standard vector for the checksum function
    std::vector<uint64_t> snapshot(static_cast<std::size_t>(key_space));
    for (uint32_t i = 0; i < key_space; ++i) {
      snapshot[i] = counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(snapshot);
  }

  return final_hash;
}