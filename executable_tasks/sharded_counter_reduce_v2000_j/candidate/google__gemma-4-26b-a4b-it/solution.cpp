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
  // We use a vector of atomics. Note: std::atomic is not copyable, 
  // so we manage the memory manually or use a vector of a wrapper.
  struct AtomicCounter {
    std::atomic<uint64_t> val{0};
  };
  
  // To avoid false sharing between threads, we could pad, but since 
  // key_space is likely large, the primary concern is the contention 
  // on specific indices. The main bottleneck in the baseline was the mutex.
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space));
  for(size_t i = 0; i < counts.size(); ++i) counts[i].store(0, std::memory_order_relaxed);

  uint64_t final_hash = 0;
  const std::size_t n = keys.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Reset counts
    for(size_t i = 0; i < counts.size(); ++i) counts[i].store(0, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;

      threads.emplace_back([&, begin, end]() {
        // Local aggregation to reduce atomic contention on the global array
        // We use a small local cache or just perform atomic adds.
        // Given the task, atomic adds are much faster than a mutex.
        for (std::size_t i = begin; i < end; ++i) {
          counts[static_cast<std::size_t>(keys[i])].fetch_add(
              static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Prepare for checksum
    std::vector<uint64_t> checksum_input(counts.size());
    for(size_t i = 0; i < counts.size(); ++i) {
      checksum_input[i] = counts[i].load(std::memory_order_relaxed);
    }
    final_hash = checksum_u64(checksum_input);
  }

  return final_hash;
}
