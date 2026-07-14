#include "interface.h"

#include <atomic>
#include <cstring>
#include <thread>
#include <vector>

namespace {

constexpr std::size_t CACHE_LINE = 64;

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
  
  // Global counts using atomics for lock-free merging
  std::vector<std::atomic<uint64_t>> global_counts(ks);
  
  // Thread-local storage: each thread gets its own counter array
  // Padded to avoid false sharing between threads
  const std::size_t padded_ks = ((ks * sizeof(uint64_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint64_t);
  std::vector<uint64_t> local_storage(nt * padded_ks, 0);
  
  // Final counts buffer for checksum
  std::vector<uint64_t> final_counts(ks);
  
  uint64_t hash = 0;
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* values_ptr = values.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset global counts
    for (std::size_t i = 0; i < ks; ++i) {
      global_counts[i].store(0, std::memory_order_relaxed);
    }
    
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const std::size_t chunk = n / nt;
    
    for (std::size_t t = 0; t < nt; ++t) {
      const std::size_t begin = t * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* local_counts = &local_storage[t * padded_ks];
      
      threads.emplace_back([=, &global_counts]() {
        // Clear local counts
        std::memset(local_counts, 0, ks * sizeof(uint64_t));
        
        // Accumulate into thread-local storage (no synchronization needed)
        for (std::size_t i = begin; i < end; ++i) {
          local_counts[keys_ptr[i]] += static_cast<uint64_t>(values_ptr[i]);
        }
        
        // Merge into global counts using atomics
        for (std::size_t i = 0; i < ks; ++i) {
          if (local_counts[i] != 0) {
            global_counts[i].fetch_add(local_counts[i], std::memory_order_relaxed);
          }
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Copy to final buffer for checksum
    for (std::size_t i = 0; i < ks; ++i) {
      final_counts[i] = global_counts[i].load(std::memory_order_relaxed);
    }
    
    hash = checksum_u64(final_counts.data(), ks);
  }
  
  return hash;
}