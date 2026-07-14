#include "interface.h"

#include <atomic>
#include <thread>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_u64(const uint64_t* counts, std::size_t n) {
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
  constexpr std::size_t CACHE_LINE = 64;
  const std::size_t padded_ks = ((ks * sizeof(uint64_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint64_t);
  
  std::vector<uint64_t> all_local(nt * padded_ks, 0);
  std::vector<uint64_t> counts(ks, 0);
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* vals_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear thread-local counters
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint64_t));
    
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const std::size_t chunk = n / nt;
    
    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* local_counts = all_local.data() + t * padded_ks;
      
      threads.emplace_back([=]() {
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[keys_ptr[i]] += vals_ptr[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Merge thread-local counters into final counts
    // Parallelize the merge for large key spaces
    if (ks >= 4096 && nt > 1) {
      std::vector<std::thread> merge_threads;
      merge_threads.reserve(nt);
      const std::size_t merge_chunk = ks / nt;
      
      for (std::size_t t = 0; t < nt; ++t) {
        const std::size_t kb = t * merge_chunk;
        const std::size_t ke = (t + 1 == nt) ? ks : kb + merge_chunk;
        
        merge_threads.emplace_back([&, kb, ke]() {
          for (std::size_t k = kb; k < ke; ++k) {
            uint64_t sum = 0;
            for (std::size_t tid = 0; tid < nt; ++tid) {
              sum += all_local[tid * padded_ks + k];
            }
            counts[k] = sum;
          }
        });
      }
      
      for (std::thread& th : merge_threads) {
        th.join();
      }
    } else {
      // Sequential merge for small key spaces
      std::memset(counts.data(), 0, ks * sizeof(uint64_t));
      for (std::size_t tid = 0; tid < nt; ++tid) {
        const uint64_t* local = all_local.data() + tid * padded_ks;
        for (std::size_t k = 0; k < ks; ++k) {
          counts[k] += local[k];
        }
      }
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}