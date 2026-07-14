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
  
  const std::size_t n = keys.size();
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const int nt = num_threads;
  
  // Pre-allocate thread-local counters for all threads
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (int t = 0; t < nt; ++t) {
    local_counts[t].resize(ks, 0);
  }
  
  // Final merged counts
  std::vector<uint64_t> counts(ks, 0);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Reset all local counters
    for (int t = 0; t < nt; ++t) {
      std::memset(local_counts[t].data(), 0, ks * sizeof(uint64_t));
    }
    
    // Phase 1: Each thread accumulates into its own local counter (no synchronization needed)
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    const std::size_t chunk = n / static_cast<std::size_t>(nt);
    
    for (int t = 0; t < nt; ++t) {
      const std::size_t begin = static_cast<std::size_t>(t) * chunk;
      const std::size_t end = (t + 1 == nt) ? n : begin + chunk;
      
      threads.emplace_back([&, t, begin, end]() {
        uint64_t* lc = local_counts[t].data();
        const uint32_t* k = keys.data();
        const uint32_t* v = values.data();
        
        for (std::size_t i = begin; i < end; ++i) {
          lc[k[i]] += v[i];
        }
      });
    }
    
    for (auto& th : threads) {
      th.join();
    }
    
    // Phase 2: Merge local counters into final counts in parallel
    // Each thread handles a range of keys
    threads.clear();
    
    const std::size_t key_chunk = ks / static_cast<std::size_t>(nt);
    
    for (int t = 0; t < nt; ++t) {
      const std::size_t kb = static_cast<std::size_t>(t) * key_chunk;
      const std::size_t ke = (t + 1 == nt) ? ks : kb + key_chunk;
      
      threads.emplace_back([&, kb, ke]() {
        for (std::size_t k = kb; k < ke; ++k) {
          uint64_t sum = 0;
          for (int tt = 0; tt < nt; ++tt) {
            sum += local_counts[tt][k];
          }
          counts[k] = sum;
        }
      });
    }
    
    for (auto& th : threads) {
      th.join();
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}