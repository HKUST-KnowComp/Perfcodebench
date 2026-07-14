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
  // Each thread gets its own cache-line-aligned counter array
  const size_t padded_ks = ((ks * sizeof(uint64_t) + CACHE_LINE - 1) / CACHE_LINE) * CACHE_LINE / sizeof(uint64_t);
  std::vector<uint64_t> all_local(static_cast<size_t>(nt) * padded_ks, 0);
  
  // Final merged counts
  std::vector<uint64_t> counts(ks, 0);
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* vals_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear thread-local counters
    std::memset(all_local.data(), 0, all_local.size() * sizeof(uint64_t));
    
    std::vector<std::thread> threads;
    threads.reserve(static_cast<size_t>(nt));
    
    const size_t chunk = n / static_cast<size_t>(nt);
    
    for (int t = 0; t < nt; ++t) {
      const size_t begin = static_cast<size_t>(t) * chunk;
      const size_t end = (t + 1 == nt) ? n : begin + chunk;
      uint64_t* local_counts = all_local.data() + static_cast<size_t>(t) * padded_ks;
      
      threads.emplace_back([keys_ptr, vals_ptr, local_counts, begin, end]() {
        for (size_t i = begin; i < end; ++i) {
          local_counts[keys_ptr[i]] += vals_ptr[i];
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Parallel reduction of thread-local counters into final counts
    // Partition key_space among threads for merging
    std::vector<std::thread> merge_threads;
    merge_threads.reserve(static_cast<size_t>(nt));
    
    const size_t keys_per_thread = (ks + static_cast<size_t>(nt) - 1) / static_cast<size_t>(nt);
    
    for (int t = 0; t < nt; ++t) {
      const size_t k_begin = static_cast<size_t>(t) * keys_per_thread;
      const size_t k_end = (k_begin + keys_per_thread > ks) ? ks : k_begin + keys_per_thread;
      
      if (k_begin >= ks) break;
      
      merge_threads.emplace_back([&all_local, &counts, padded_ks, nt, k_begin, k_end]() {
        for (size_t k = k_begin; k < k_end; ++k) {
          uint64_t sum = 0;
          for (int tt = 0; tt < nt; ++tt) {
            sum += all_local[static_cast<size_t>(tt) * padded_ks + k];
          }
          counts[k] = sum;
        }
      });
    }
    
    for (std::thread& th : merge_threads) {
      th.join();
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}