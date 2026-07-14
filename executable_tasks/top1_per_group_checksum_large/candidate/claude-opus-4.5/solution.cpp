#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>
#include <algorithm>

#ifdef __AVX2__
#include <immintrin.h>
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  
  const size_t n = groups.size();
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  
  uint64_t hash = 0;
  
  // Use flat vector instead of hash map for dense groups
  std::vector<uint32_t> best(group_count);
  
  for (int iter = 0; iter < iters; ++iter) {
    // Clear the best array
    std::memset(best.data(), 0, group_count * sizeof(uint32_t));
    
    // Main aggregation loop - find max per group
#ifdef _OPENMP
    #pragma omp parallel
    {
      // Thread-local best array
      std::vector<uint32_t> local_best(group_count, 0);
      
      #pragma omp for nowait
      for (size_t i = 0; i < n; ++i) {
        uint32_t grp = g_ptr[i];
        uint32_t val = v_ptr[i];
        if (val > local_best[grp]) {
          local_best[grp] = val;
        }
      }
      
      // Merge local results into global
      #pragma omp critical
      {
        for (uint32_t g = 0; g < group_count; ++g) {
          if (local_best[g] > best[g]) {
            best[g] = local_best[g];
          }
        }
      }
    }
#else
    // Sequential version
    for (size_t i = 0; i < n; ++i) {
      uint32_t grp = g_ptr[i];
      uint32_t val = v_ptr[i];
      if (val > best[grp]) {
        best[grp] = val;
      }
    }
#endif
    
    // Compute hash over all groups in order
    hash = 1469598103934665603ULL;
    const uint32_t* best_ptr = best.data();
    
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, best_ptr[g]);
    }
  }
  
  return hash;
}