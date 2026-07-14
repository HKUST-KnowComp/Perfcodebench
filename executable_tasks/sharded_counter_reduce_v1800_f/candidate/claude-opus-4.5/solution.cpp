#include "interface.h"

#include <atomic>
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
  
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const std::size_t nt = static_cast<std::size_t>(num_threads);
  const std::size_t n = keys.size();
  
  // Allocate thread-local counters - each thread gets its own array
  // Pad to avoid false sharing between thread arrays
  constexpr std::size_t cache_line = 64;
  const std::size_t padded_ks = ((ks * sizeof(uint64_t) + cache_line - 1) / cache_line) * cache_line / sizeof(uint64_t);
  
  std::vector<uint64_t> local_counts(nt * padded_ks, 0);
  std::vector<uint64_t> global_counts(ks, 0);
  
  uint64_t hash = 0;
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all counters
    std::memset(local_counts.data(), 0, local_counts.size() * sizeof(uint64_t));
    std::memset(global_counts.data(), 0, ks * sizeof(uint64_t));
    
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const std::size_t chunk = n / nt;
    
    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* my_counts = local_counts.data() + t * padded_ks;
      
      threads.emplace_back([keys_ptr, values_ptr, my_counts, begin, end]() {
        // Phase 1: Accumulate into thread-local counters (no synchronization needed)
        for (std::size_t i = begin; i < end; ++i) {
          my_counts[keys_ptr[i]] += static_cast<uint64_t>(values_ptr[i]);
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Phase 2: Merge thread-local counters into global counts
    // Parallelize the merge for large key spaces
    if (ks > 10000 && nt > 1) {
      std::vector<std::thread> merge_threads;
      merge_threads.reserve(nt);
      
      const std::size_t keys_per_thread = (ks + nt - 1) / nt;
      
      for (std::size_t t = 0; t < nt; ++t) {
        const std::size_t k_begin = t * keys_per_thread;
        const std::size_t k_end = (k_begin + keys_per_thread > ks) ? ks : k_begin + keys_per_thread;
        
        merge_threads.emplace_back([&local_counts, &global_counts, padded_ks, nt, k_begin, k_end]() {
          for (std::size_t k = k_begin; k < k_end; ++k) {
            uint64_t sum = 0;
            for (std::size_t t2 = 0; t2 < nt; ++t2) {
              sum += local_counts[t2 * padded_ks + k];
            }
            global_counts[k] = sum;
          }
        });
      }
      
      for (std::thread& th : merge_threads) {
        th.join();
      }
    } else {
      // Sequential merge for small key spaces
      for (std::size_t k = 0; k < ks; ++k) {
        uint64_t sum = 0;
        for (std::size_t t = 0; t < nt; ++t) {
          sum += local_counts[t * padded_ks + k];
        }
        global_counts[k] = sum;
      }
    }
    
    hash = checksum_u64(global_counts.data(), ks);
  }
  
  return hash;
}