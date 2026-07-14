#include "interface.h"

#include <thread>
#include <vector>
#include <cstring>

namespace {

uint64_t checksum_u64(const uint64_t* counts, std::size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < n; ++i) {
    hash ^= counts[i];
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
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  
  // Global counts array
  std::vector<uint64_t> counts(ks, 0);
  
  // Thread-local counters - each thread gets its own array
  std::vector<std::vector<uint64_t>> local_counts(static_cast<std::size_t>(num_threads));
  for (int t = 0; t < num_threads; ++t) {
    local_counts[static_cast<std::size_t>(t)].resize(ks);
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear global counts
    std::memset(counts.data(), 0, ks * sizeof(uint64_t));
    
    // Clear all thread-local counts
    for (int t = 0; t < num_threads; ++t) {
      std::memset(local_counts[static_cast<std::size_t>(t)].data(), 0, ks * sizeof(uint64_t));
    }
    
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(num_threads));
    
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);
    
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;
      uint64_t* local = local_counts[static_cast<std::size_t>(t)].data();
      
      threads.emplace_back([keys_ptr, values_ptr, local, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          local[keys_ptr[i]] += values_ptr[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Merge thread-local counts into global counts
    for (int t = 0; t < num_threads; ++t) {
      const uint64_t* local = local_counts[static_cast<std::size_t>(t)].data();
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local[k];
      }
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}