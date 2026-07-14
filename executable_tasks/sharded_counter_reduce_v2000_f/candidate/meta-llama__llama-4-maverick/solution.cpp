#include "interface.h"

#include <atomic>
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
  std::vector<std::atomic<uint64_t>> counts(static_cast<std::size_t>(key_space), 0);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (auto& count : counts) {
      count.store(0, std::memory_order_relaxed);
    }
    std::vector<std::thread> threads;
    const std::size_t chunk = keys.size() / static_cast<std::size_t>(num_threads);
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? keys.size() : begin + chunk;
      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          counts[static_cast<std::size_t>(keys[i])].fetch_add(static_cast<uint64_t>(values[i]), std::memory_order_relaxed);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }
    std::vector<uint64_t> counts_snapshot(counts.size());
    for (std::size_t i = 0; i < counts.size(); ++i) {
      counts_snapshot[i] = counts[i].load(std::memory_order_relaxed);
    }
    hash = checksum_u64(counts_snapshot);
  }
  return hash;
}