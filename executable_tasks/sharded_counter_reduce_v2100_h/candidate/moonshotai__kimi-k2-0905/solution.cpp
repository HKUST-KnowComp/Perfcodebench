#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>

namespace {

uint64_t checksum_u64(const std::vector<std::atomic<uint64_t>>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (const auto& v : counts) {
    uint64_t val = v.load(std::memory_order_relaxed);
    hash ^= val;
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
  std::vector<std::atomic<uint64_t>> counts(key_space);
  for (auto& c : counts) c.store(0, std::memory_order_relaxed);

  const std::size_t n = keys.size();
  const std::size_t chunk = (n + num_threads - 1) / num_threads;

  for (int iter = 0; iter < iters; ++iter) {
    // reset counters
    for (auto& c : counts) c.store(0, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      std::size_t begin = t * chunk;
      std::size_t end   = std::min(begin + chunk, n);
      threads.emplace_back([&, begin, end]() {
        // private accumulation to avoid atomic RMW on every update
        std::vector<uint64_t> local(key_space, 0);
        for (std::size_t i = begin; i < end; ++i) {
          local[keys[i]] += static_cast<uint64_t>(values[i]);
        }
        // merge once
        for (uint32_t k = 0; k < key_space; ++k) {
          if (local[k] != 0) {
            counts[k].fetch_add(local[k], std::memory_order_relaxed);
          }
        }
      });
    }

    for (auto& th : threads) th.join();
  }

  return checksum_u64(counts);
}