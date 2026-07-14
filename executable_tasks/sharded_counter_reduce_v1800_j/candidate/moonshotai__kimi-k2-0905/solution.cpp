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
  std::vector<std::atomic<uint64_t>> counts(key_space);
  for (auto& c : counts) c.store(0, std::memory_order_relaxed);

  uint64_t hash = 0;
  const std::size_t n = keys.size();
  const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    // reset counters
    for (auto& c : counts) c.store(0, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(num_threads));

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end   = (t + 1 == num_threads) ? n : begin + chunk;
      threads.emplace_back([&, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          uint32_t k = keys[i];
          uint32_t v = values[i];
          counts[k].fetch_add(static_cast<uint64_t>(v), std::memory_order_relaxed);
        }
      });
    }

    for (auto& th : threads) th.join();

    // snapshot into plain vector for checksum
    std::vector<uint64_t> snapshot(key_space);
    for (std::size_t i = 0; i < key_space; ++i) {
      snapshot[i] = counts[i].load(std::memory_order_relaxed);
    }
    hash = checksum_u64(snapshot);
  }

  return hash;
}