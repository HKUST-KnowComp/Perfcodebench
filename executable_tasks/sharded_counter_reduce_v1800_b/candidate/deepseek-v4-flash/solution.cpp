#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
  // Use atomic counters for lock-free updates
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all counters to zero (relaxed order is safe because no concurrent access yet)
    for (auto& c : counts) {
      c.store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    const std::size_t chunk = keys.size() / static_cast<std::size_t>(num_threads);
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? keys.size() : begin + chunk;
      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          // Atomic fetch-add without locks; relaxed order suffices because we join threads later
          counts[static_cast<std::size_t>(keys[i])].fetch_add(
              static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }

    // Compute checksum of final counts (relaxed load is safe after join)
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < counts.size(); ++i) {
      uint64_t v = counts[i].load(std::memory_order_relaxed);
      hash ^= v;
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
