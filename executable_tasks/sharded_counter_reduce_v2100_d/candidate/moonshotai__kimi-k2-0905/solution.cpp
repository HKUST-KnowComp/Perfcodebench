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
  std::vector<std::atomic<uint64_t>> acounts(static_cast<std::size_t>(key_space));
  for (auto& a : acounts) a.store(0, std::memory_order_relaxed);

  const std::size_t n = keys.size();
  const std::size_t chunk = (n + num_threads - 1) / static_cast<std::size_t>(num_threads);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // reset counters
    for (auto& a : acounts) a.store(0, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(num_threads));

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = t * chunk;
      if (begin >= n) break;
      const std::size_t end = std::min(begin + chunk, n);
      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          const std::size_t bucket = static_cast<std::size_t>(keys[i]);
          const uint64_t delta = static_cast<uint64_t>(values[i]);
          acounts[bucket].fetch_add(delta, std::memory_order_relaxed);
        }
      });
    }

    for (std::thread& th : threads) th.join();

    // materialize snapshot for checksum
    std::vector<uint64_t> counts(static_cast<std::size_t>(key_space));
    for (std::size_t i = 0; i < key_space; ++i) {
      counts[i] = acounts[i].load(std::memory_order_relaxed);
    }
    hash = checksum_u64(counts);
  }
  return hash;
}