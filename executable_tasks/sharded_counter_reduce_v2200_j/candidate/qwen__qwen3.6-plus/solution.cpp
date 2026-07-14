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

} // namespace

uint64_t sharded_counter_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int num_threads,
    int iters) {
    
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const std::size_t n_threads = static_cast<std::size_t>(num_threads > 0 ? num_threads : 1);
  const std::size_t total_size = ks * n_threads;
  
  std::vector<uint64_t> local_counts(total_size, 0);
  std::vector<uint64_t> sums(ks, 0);
  
  const std::size_t chunk = keys.size() / n_threads;
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(local_counts.begin(), local_counts.end(), 0);
    
    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    
    for (std::size_t t = 0; t < n_threads; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == n_threads) ? keys.size() : begin + chunk;
      const std::size_t offset = t * ks;
      
      threads.emplace_back([&, begin, end, offset]() {
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[offset + static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    std::fill(sums.begin(), sums.end(), 0);
    for (std::size_t t = 0; t < n_threads; ++t) {
      const std::size_t offset = t * ks;
      for (std::size_t k = 0; k < ks; ++k) {
        sums[k] += local_counts[offset + k];
      }
    }
    
    hash = checksum_u64(sums);
  }
  
  return hash;
}