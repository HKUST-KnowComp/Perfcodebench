#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>

namespace {

uint64_t checksum_u64(const std::atomic<uint64_t>* counts, uint32_t key_space) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < key_space; ++i) {
    uint64_t v = counts[i].load(std::memory_order_relaxed);
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
  std::unique_ptr<std::atomic<uint64_t>[]> counts(new std::atomic<uint64_t>[key_space]);

  for (int iter = 0; iter < iters; ++iter) {
    for (uint32_t i = 0; i < key_space; ++i) {
      counts[i].store(0, std::memory_order_relaxed);
    }

    std::vector<std::thread> threads;
    const size_t n = keys.size();
    const size_t chunk = (n + num_threads - 1) / num_threads;

    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = std::min(begin + chunk, n);
      threads.emplace_back([&, begin, end]() {
        for (size_t i = begin; i < end; ++i) {
          counts[keys[i]].fetch_add(values[i], std::memory_order_relaxed);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }
  }
  return checksum_u64(counts.get(), key_space);
}