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
  
  // Use atomics to avoid global mutex contention
  // We use a vector of atomics. Note: std::vector<std::atomic<T>> is not allowed 
  // because atomic is not CopyInsertable. We use a unique_ptr or a raw array.
  std::vector<std::atomic<uint64_t>> atomic_counts(static_cast<std::size_t>(key_space));
  
  const std::size_t total_keys = keys.size();
  const std::size_t chunk = total_keys / static_cast<std::size_t>(num_threads);
  
  // Pre-calculate thread ranges
  struct Range { std::size_t begin, end; };
  std::vector<Range> ranges(num_threads);
  for (int t = 0; t < num_threads; ++t) {
    ranges[t].begin = static_cast<std::size_t>(t) * chunk;
    ranges[t].end = (t + 1 == num_threads) ? total_keys : ranges[t].begin + chunk;
  }

  uint64_t final_hash = 0;
  std::vector<uint64_t> counts_buffer(static_cast<std::size_t>(key_space));

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counters
    for (uint32_t i = 0; i < key_space; ++i) {
      atomic_counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      threads.emplace_back([&, t]() {
        const auto& range = ranges[t];
        for (std::size_t i = range.begin; i < range.end; ++i) {
          // Use relaxed memory order as we only need atomicity of the addition,
          // and the join() provides the necessary synchronization for the checksum.
          atomic_counts[keys[i]].fetch_add(values[i], std::memory_order_relaxed);
        }
      });
    }

    for (auto& th : threads) {
      th.join();
    }

    // Collect results into a non-atomic vector for checksumming
    for (uint32_t i = 0; i < key_space; ++i) {
      counts_buffer[i] = atomic_counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(counts_buffer);
  }

  return final_hash;
}