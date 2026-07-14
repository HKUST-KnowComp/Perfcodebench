#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <cstring>

namespace {

constexpr size_t CACHE_LINE = 64;

uint64_t checksum_u64(const uint64_t* counts, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
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
  
  const size_t ks = static_cast<size_t>(key_space);
  const size_t n = keys.size();
  const int nt = num_threads;
  
  // Allocate thread-local counters with padding to avoid false sharing
  // Each thread gets its own counter array
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (int t = 0; t < nt; ++t) {
    local_counts[t].resize(ks, 0);
  }
  
  // Final merged counts
  std::vector<uint64_t> counts(ks, 0);
  
  uint64_t hash = 0;
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* vals_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear local counts
    for (int t = 0; t < nt; ++t) {
      std::memset(local_counts[t].data(), 0, ks * sizeof(uint64_t));
    }
    
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const size_t chunk = n / static_cast<size_t>(nt);
    
    for (int t = 0; t < nt; ++t) {
      const size_t begin = static_cast<size_t>(t) * chunk;
      const size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* my_counts = local_counts[t].data();
      
      threads.emplace_back([keys_ptr, vals_ptr, my_counts, begin, end]() {
        // No synchronization needed - each thread writes to its own array
        for (size_t i = begin; i < end; ++i) {
          my_counts[keys_ptr[i]] += static_cast<uint64_t>(vals_ptr[i]);
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Merge local counts into final counts
    // Parallelize the merge as well
    std::memset(counts.data(), 0, ks * sizeof(uint64_t));
    
    if (ks >= 1024 && nt > 1) {
      // Parallel merge for large key spaces
      std::vector<std::thread> merge_threads;
      merge_threads.reserve(nt);
      
      const size_t merge_chunk = ks / static_cast<size_t>(nt);
      uint64_t* counts_ptr = counts.data();
      
      for (int t = 0; t < nt; ++t) {
        const size_t kb = static_cast<size_t>(t) * merge_chunk;
        const size_t ke = (t + 1 == nt) ? ks : kb + merge_chunk;
        
        merge_threads.emplace_back([&local_counts, counts_ptr, kb, ke, nt]() {
          for (size_t k = kb; k < ke; ++k) {
            uint64_t sum = 0;
            for (int tt = 0; tt < nt; ++tt) {
              sum += local_counts[tt][k];
            }
            counts_ptr[k] = sum;
          }
        });
      }
      
      for (std::thread& th : merge_threads) {
        th.join();
      }
    } else {
      // Sequential merge for small key spaces
      for (int t = 0; t < nt; ++t) {
        const uint64_t* lc = local_counts[t].data();
        for (size_t k = 0; k < ks; ++k) {
          counts[k] += lc[k];
        }
      }
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}