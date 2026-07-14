#include "interface.h"

#include <algorithm>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 0;
  }

  // Since keys are dense, we can use direct indexing
  // Find the minimum key to compute offsets
  const uint32_t min_key = keys.front(); // keys are sorted, so first element is minimum
  const uint32_t* values_ptr = values.data();
  const uint32_t* probes_ptr = probes.data();
  const std::size_t probes_size = probes.size();

  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    // Process with prefetching and unrolling
    std::size_t i = 0;
    const std::size_t unroll_limit = (probes_size >= 4) ? (probes_size - 3) : 0;
    
    // Unrolled loop with prefetching
    for (; i < unroll_limit; i += 4) {
      const std::size_t idx0 = probes_ptr[i] - min_key;
      const std::size_t idx1 = probes_ptr[i + 1] - min_key;
      const std::size_t idx2 = probes_ptr[i + 2] - min_key;
      const std::size_t idx3 = probes_ptr[i + 3] - min_key;
      
      // Prefetch ahead
      if (i + 8 < probes_size) {
        __builtin_prefetch(&values_ptr[probes_ptr[i + 4] - min_key], 0, 0);
        __builtin_prefetch(&values_ptr[probes_ptr[i + 5] - min_key], 0, 0);
        __builtin_prefetch(&values_ptr[probes_ptr[i + 6] - min_key], 0, 0);
        __builtin_prefetch(&values_ptr[probes_ptr[i + 7] - min_key], 0, 0);
      }
      
      hash = mix(hash, values_ptr[idx0]);
      hash = mix(hash, values_ptr[idx1]);
      hash = mix(hash, values_ptr[idx2]);
      hash = mix(hash, values_ptr[idx3]);
    }
    
    // Handle remaining elements
    for (; i < probes_size; ++i) {
      const std::size_t idx = probes_ptr[i] - min_key;
      hash = mix(hash, values_ptr[idx]);
    }
  }
  
  return hash;
}