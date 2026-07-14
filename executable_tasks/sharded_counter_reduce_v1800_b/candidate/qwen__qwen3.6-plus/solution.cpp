#include "interface.h"
#include <thread>
#include <vector>
#include <algorithm>
#include <cstdint>

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
  const size_t ks = static_cast<size_t>(key_space);
  const size_t n_threads = static_cast<size_t>(num_threads);
  const size_t n_keys = keys.size();

  std::vector<uint64_t> counts(ks);
  std::vector<uint64_t> shards(n_threads * ks);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    const size_t chunk = n_keys / n_threads;
    for (size_t t = 0; t < n_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = (t + 1 == n_threads) ? n_keys : begin + chunk;
      threads.emplace_back([&, begin, end, t]() {
        const size_t base = t * ks;
        std::fill(shards.begin() + base, shards.begin() + base + ks, 0);
        for (size_t i = begin; i < end; ++i) {
          shards[base + static_cast<size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (auto& th : threads) th.join();

    std::copy(shards.begin(), shards.begin() + ks, counts.begin());
    for (size_t t = 1; t < n_threads; ++t) {
      const size_t base = t * ks;
      for (size_t i = 0; i < ks; ++i) {
        counts[i] += shards[base + i];
      }
    }
    hash = checksum_u64(counts);
  }
  return hash;
}