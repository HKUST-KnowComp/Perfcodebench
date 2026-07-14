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
  const uint32_t* __restrict keys_ptr = keys.data();
  const uint32_t* __restrict values_ptr = values.data();
  
  // Allocate thread-local counters with padding to avoid false sharing
  constexpr std::size_t CACHE_LINE = 64;
  const std::size_t padded_ks = ((ks * sizeof(uint64_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint64_t);
  
  std::vector<uint64_t> all_local(static_cast<std::size_t>(num_threads) * padded_ks, 0);
  std::vector<uint64_t> counts(ks, 0);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all thread-local counters
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint64_t));
    
    std::vector<std::thread> threads;
    threads.reserve(static_cast<std::size_t>(num_threads));
    
    const std::size_t chunk = n / static_cast<std::size_t>(num_threads);
    
    for (int t = 0; t < num_threads; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == num_threads) ? n : begin + chunk;
      uint64_t* local_counts = all_local.data() + static_cast<std::size_t>(t) * padded_ks;
      
      threads.emplace_back([keys_ptr, values_ptr, local_counts, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[keys_ptr[i]] += values_ptr[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Merge thread-local counters into final counts
    std::memset(counts.data(), 0, ks * sizeof(uint64_t));
    for (int t = 0; t < num_threads; ++t) {
      const uint64_t* local_counts = all_local.data() + static_cast<std::size_t>(t) * padded_ks;
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += local_counts[k];
      }
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}