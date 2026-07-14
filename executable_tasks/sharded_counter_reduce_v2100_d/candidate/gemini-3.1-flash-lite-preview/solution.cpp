#include "interface.h"
#include <vector>
#include <thread>
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

  const size_t n = keys.size();
  const size_t k_space = static_cast<size_t>(key_space);
  std::vector<uint64_t> final_counts(k_space);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(k_space, 0));
    std::vector<std::thread> threads;
    size_t chunk = (n + num_threads - 1) / num_threads;

    for (int t = 0; t < num_threads; ++t) {
      threads.emplace_back([&, t]() {
        size_t start = static_cast<size_t>(t) * chunk;
        size_t end = std::min(start + chunk, n);
        auto& my_counts = local_counts[t];
        for (size_t i = start; i < end; ++i) {
          my_counts[keys[i]] += values[i];
        }
      });
    }

    for (auto& th : threads) th.join();

    std::fill(final_counts.begin(), final_counts.end(), 0);
    for (int t = 0; t < num_threads; ++t) {
      for (size_t i = 0; i < k_space; ++i) {
        final_counts[i] += local_counts[t][i];
      }
    }
    hash = checksum_u64(final_counts);
  }
  return hash;
}