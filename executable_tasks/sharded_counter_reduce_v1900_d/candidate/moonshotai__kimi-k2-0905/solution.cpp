#include "interface.h"

#include <thread>
#include <vector>
#include <numeric>

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
  std::vector<uint64_t> counts(static_cast<size_t>(key_space), 0);
  const size_t n = keys.size();
  const size_t chunk = (n + num_threads - 1) / static_cast<size_t>(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(counts.begin(), counts.end(), 0);
    std::vector<std::thread> threads;
    std::vector<std::vector<uint64_t>> local_counts(static_cast<size_t>(num_threads),
                                                    std::vector<uint64_t>(static_cast<size_t>(key_space), 0));

    for (int t = 0; t < num_threads; ++t) {
      const size_t begin = t * chunk;
      const size_t end = std::min(begin + chunk, n);
      threads.emplace_back([&, t, begin, end]() {
        auto& lc = local_counts[static_cast<size_t>(t)];
        for (size_t i = begin; i < end; ++i) {
          lc[static_cast<size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    for (auto& th : threads) th.join();

    for (size_t k = 0; k < static_cast<size_t>(key_space); ++k) {
      for (size_t t = 0; t < static_cast<size_t>(num_threads); ++t) {
        counts[k] += local_counts[t][k];
      }
    }
  }
  return checksum_u64(counts);
}