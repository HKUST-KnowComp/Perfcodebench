#include "interface.h"

#include <algorithm>
#include <cstdint>
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
  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);

  // Per-thread local counters
  std::vector<std::vector<uint64_t>> local_counts(num_threads,
      std::vector<uint64_t>(ks, 0));

  // Master counter for final aggregation
  std::vector<uint64_t> master(ks, 0);

  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::thread> threads;
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;

      threads.emplace_back([&, t, begin, end]() {
        // Reset local counter for this thread
        std::fill(local_counts[t].begin(), local_counts[t].end(), 0ULL);
        uint64_t* local = local_counts[t].data();

        // Process chunk
        for (std::size_t i = begin; i < end; ++i) {
          local[static_cast<std::size_t>(keys[i])] +=
              static_cast<uint64_t>(values[i]);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Aggregate per-thread counters into master
    std::fill(master.begin(), master.end(), 0ULL);
    for (int t = 0; t < num_threads; ++t) {
      const uint64_t* local = local_counts[t].data();
      for (std::size_t k = 0; k < ks; ++k) {
        master[k] += local[k];
      }
    }

    uint64_t hash = checksum_u64(master);
    // In the original code, hash is overwritten each iteration.
    // Return the last computed hash.
    if (iter == iters - 1) {
      return hash;
    }
  }
  // Should never reach here if iters > 0
  return 0ULL;
}
