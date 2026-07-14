#include "interface.h"

#include <thread>
#include <vector>
#include <algorithm>

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
  const std::size_t k_space = static_cast<std::size_t>(key_space);
  const std::size_t n_keys = keys.size();
  std::vector<uint64_t> global_counts(k_space);
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(global_counts.begin(), global_counts.end(), 0);
    std::vector<std::thread> threads;
    // Pre-allocate thread-local storage to avoid allocation inside the loop
    std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(k_space, 0));

    const std::size_t chunk = n_keys / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n_keys : begin + chunk;

      threads.emplace_back([&, t, begin, end]() {
        auto& my_counts = local_counts[t];
        for (std::size_t i = begin; i < end; ++i) {
          my_counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }

    for (std::thread& th : threads) {
      th.join();
    }

    // Reduce local counts into global counts
    for (int t = 0; t < num_threads; ++t) {
      for (std::size_t k = 0; k < k_space; ++k) {
        global_counts[k] += local_counts[t][k];
      }
    }

    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}