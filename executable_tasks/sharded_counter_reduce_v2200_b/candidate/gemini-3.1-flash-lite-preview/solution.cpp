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
  
  std::vector<std::vector<uint64_t>> local_counts(num_threads, std::vector<uint64_t>(key_space, 0));
  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (int t = 0; t < num_threads; ++t) {
      std::fill(local_counts[t].begin(), local_counts[t].end(), 0);
    }

    std::vector<std::thread> threads;
    const std::size_t n = keys.size();
    const std::size_t chunk = (n + num_threads - 1) / num_threads;

    for (int t = 0; t < num_threads; ++t) {
      threads.emplace_back([&, t]() {
        const std::size_t begin = static_cast<std::size_t>(t) * chunk;
        const std::size_t end = std::min(begin + chunk, n);
        auto& counts = local_counts[t];
        for (std::size_t i = begin; i < end; ++i) {
          counts[keys[i]] += values[i];
        }
      });
    }

    for (auto& th : threads) th.join();

    std::vector<uint64_t> total_counts(key_space, 0);
    for (int t = 0; t < num_threads; ++t) {
      for (uint32_t k = 0; k < key_space; ++k) {
        total_counts[k] += local_counts[t][k];
      }
    }
    final_hash = checksum_u64(total_counts);
  }
  return final_hash;
}