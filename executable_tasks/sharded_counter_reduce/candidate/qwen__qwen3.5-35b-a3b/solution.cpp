#include "interface.h"

#include <vector>
#include <thread>
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
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Create per-thread local counters
    std::vector<std::vector<uint64_t>> local_counts(
        num_threads, std::vector<uint64_t>(key_space, 0));
    
    std::vector<std::thread> threads;
    const std::size_t chunk = keys.size() / static_cast<std::size_t>(num_threads);
    
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? keys.size() : begin + chunk;
      
      threads.emplace_back([&local_counts, t, &keys, &values, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[t][keys[i]] += values[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Aggregate local counters
    std::fill(counts.begin(), counts.end(), 0);
    for (int t = 0; t < num_threads; ++t) {
      for (std::size_t i = 0; i < key_space; ++i) {
        counts[i] += local_counts[t][i];
      }
    }
    
    hash = checksum_u64(counts);
  }
  
  return hash;
}