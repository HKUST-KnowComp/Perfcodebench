#include "interface.h"

#include <cstring>
#include <algorithm>

#if defined(__SSE4_1__)
#include <smmintrin.h>
#define USE_SSE 1
#elif defined(__SSE2__)
#include <emmintrin.h>
#define USE_SSE 1
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
  if (n == 0 || group_count == 0) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, 0u);
    }
    return hash;
  }
  
  std::vector<uint32_t> best(group_count);
  
  const uint32_t* __restrict g_ptr = groups.data();
  const uint32_t* __restrict v_ptr = values.data();
  uint32_t* __restrict b_ptr = best.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    std::memset(b_ptr, 0, group_count * sizeof(uint32_t));
    
    size_t i = 0;
    
    // Prefetch ahead
    const size_t prefetch_dist = 16;
    
    // Main loop with prefetching
    for (; i + prefetch_dist < n; ++i) {
      __builtin_prefetch(&g_ptr[i + prefetch_dist], 0, 0);
      __builtin_prefetch(&v_ptr[i + prefetch_dist], 0, 0);
      
      const uint32_t grp = g_ptr[i];
      const uint32_t val = v_ptr[i];
      if (val > b_ptr[grp]) {
        b_ptr[grp] = val;
      }
    }
    
    // Remainder
    for (; i < n; ++i) {
      const uint32_t grp = g_ptr[i];
      const uint32_t val = v_ptr[i];
      if (val > b_ptr[grp]) {
        b_ptr[grp] = val;
      }
    }
    
    // Compute hash
    hash = 1469598103934665603ULL;
    
#if USE_SSE && defined(__SSE4_1__)
    // Process 4 at a time for hash computation
    size_t g = 0;
    for (; g + 4 <= group_count; g += 4) {
      hash = mix(hash, b_ptr[g]);
      hash = mix(hash, b_ptr[g + 1]);
      hash = mix(hash, b_ptr[g + 2]);
      hash = mix(hash, b_ptr[g + 3]);
    }
    for (; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
#else
    for (uint32_t g = 0; g < group_count; ++g) {
      hash = mix(hash, b_ptr[g]);
    }
#endif
  }
  
  return hash;
}