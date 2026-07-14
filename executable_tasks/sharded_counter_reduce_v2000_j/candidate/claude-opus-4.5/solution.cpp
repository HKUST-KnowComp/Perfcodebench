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
  
  const std::size_t ks = static_cast<std::size_t>(key_space);
  const std::size_t n = keys.size();
  const int nt = num_threads;
  
  // Thread-local counters - each thread gets its own array
  std::vector<std::vector<uint64_t>> local_counts(nt);
  for (int t = 0; t < nt; ++t) {
    local_counts[t].resize(ks);
  }
  
  // Final merged counts
  std::vector<uint64_t> counts(ks);
  
  // Precompute chunk boundaries
  const std::size_t chunk = n / static_cast<std::size_t>(nt);
  std::vector<std::size_t> begins(nt), ends(nt);
  for (int t = 0; t < nt; ++t) {
    begins[t] = static_cast<std::size_t>(t) * chunk;
    ends[t] = (t + 1 == nt) ? n : begins[t] + chunk;
  }
  
  const uint32_t* keys_ptr = keys.data();
  const uint32_t* vals_ptr = values.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear thread-local counters
    for (int t = 0; t < nt; ++t) {
      std::memset(local_counts[t].data(), 0, ks * sizeof(uint64_t));
    }
    
    std::vector<std::thread> threads;
    threads.reserve(nt);
    
    for (int t = 0; t < nt; ++t) {
      threads.emplace_back([&, t]() {
        uint64_t* lc = local_counts[t].data();
        const std::size_t b = begins[t];
        const std::size_t e = ends[t];
        
        for (std::size_t i = b; i < e; ++i) {
          lc[keys_ptr[i]] += static_cast<uint64_t>(vals_ptr[i]);
        }
      });
    }
    
    for (std::thread& th : threads) {
      th.join();
    }
    
    // Merge thread-local counts into final counts
    std::memset(counts.data(), 0, ks * sizeof(uint64_t));
    for (int t = 0; t < nt; ++t) {
      const uint64_t* lc = local_counts[t].data();
      for (std::size_t k = 0; k < ks; ++k) {
        counts[k] += lc[k];
      }
    }
    
    hash = checksum_u64(counts.data(), ks);
  }
  
  return hash;
}