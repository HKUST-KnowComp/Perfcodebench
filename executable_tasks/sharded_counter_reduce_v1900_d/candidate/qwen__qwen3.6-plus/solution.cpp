#include "interface.h"

#include <algorithm>
#include <cstddef>
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
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  std::vector<uint64_t> local_counts(static_cast<std::size_t>(num_threads) * key_space, 0);
  uint64_t hash = 0;

  const std::size_t total_keys = keys.size();
  const std::size_t chunk = total_keys / static_cast<std::size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(local_counts.begin(), local_counts.end(), 0);
    std::fill(counts.begin(), counts.end(), 0);

    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? total_keys : begin + chunk;
      threads.emplace_back([&, t, begin, end]() {
        const std::size_t offset = static_cast<std::size_t>(t) * key_space;
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[offset + static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (std::thread& th : threads) {
      th.join();
    }

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t offset = static_cast<std::size_t>(t) * key_space;
      for (std::size_t k = 0; k < key_space; ++k) {
        counts[k] += local_counts[offset + k];
      }
    }

    hash = checksum_u64(counts);
  }
  return hash;
}
