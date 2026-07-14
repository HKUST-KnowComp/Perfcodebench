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
  const std::size_t n = keys.size();
  uint64_t final_hash = 0;

  // Pre-allocate thread-local buffers to avoid repeated allocation in the loop
  std::vector<std::vector<uint64_t>> thread_local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
  std::vector<std::thread> threads(num_threads);

  for (int iter = 0; iter < iters; ++iter) {
    // Reset thread-local buffers
    for (int t = 0; t < num_threads; ++t) {
      std::fill(thread_local_counts[t].begin(), thread_local_counts[t].end(), 0);
    }

    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);

    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;

      threads[t] = std::thread([&, t, begin, end]() {
        auto& my_counts = thread_local_counts[t];
        for (std::size_t i = begin; i < end; ++i) {
          my_counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }

    for (int t = 0; t < num_threads; ++t) {
      threads[t].join();
    }

    // Reduce thread-local counts into a single global count vector for checksumming
    std::vector<uint64_t> global_counts(k_space, 0);
    for (int t = 0; t < num_threads; ++t) {
      for (std::size_t k = 0; k < k_space; ++k) {
        global_counts[k] += thread_local_counts[t][k];
      }
    }

    final_hash = checksum_u64(global_counts);
  }

  return final_hash;
}