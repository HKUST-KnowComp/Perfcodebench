#include "interface.h"

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

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
  
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const std::size_t nt = static_cast<std::size_t>(num_threads);
  const std::size_t n = keys.size();
  
  // Thread-local counters - each thread gets its own array
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (std::size_t t = 0; t < nt; ++t) {
    local_counts[t].resize(ks, 0);
  }
  
  // Final merged counts
  std::vector<uint64_t> counts(ks, 0);
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear thread-local counts
    for (std::size_t t = 0; t < nt; ++t) {
      std::memset(local_counts[t].data(), 0, ks * sizeof(uint64_t));
    }
    
    // Phase 1: Each thread accumulates into its own local counter array
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const std::size_t chunk = n / nt;
    
    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* my_counts = local_counts[t].data();
      
      threads.emplace_back([keys_ptr, values_ptr, my_counts, begin, end]() {
        for (std::size_t i = begin; i < end; ++i) {
          my_counts[keys_ptr[i]] += values_ptr[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Phase 2: Parallel merge of thread-local counts into final counts
    threads.clear();
    
    const std::size_t key_chunk = (ks + nt - 1) / nt;
    
    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t k_begin = t * key_chunk;
      const std::size_t k_end = (k_begin + key_chunk > ks) ? ks : k_begin + key_chunk;
      
      if (k_begin >= ks) break;
      
      threads.emplace_back([&local_counts, &counts, k_begin, k_end, nt]() {
        for (std::size_t k = k_begin; k < k_end; ++k) {
          uint64_t sum = 0;
          for (std::size_t t2 = 0; t2 < nt; ++t2) {
            sum += local_counts[t2][k];
          }
          counts[k] = sum;
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}