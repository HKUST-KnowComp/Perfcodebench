#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Performance Engineer's Optimization:
 * 1. Loop Fusion: Eliminated the intermediate std::vector<uint32_t> 'expanded'.
 *    Hashing is now performed directly during the expansion process.
 * 2. Memory Efficiency: Removed repeated allocations and deallocations of the large vector.
 * 3. Inner Loop Unrolling: The hash calculation (FNV-1a) is unrolled to reduce 
 *    loop control overhead and improve instruction throughput.
 * 4. Constant Propagation: The segment ID 's' is cast once per segment.
 */

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    return 0;
  }

  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET_BASIS;
    
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t val = static_cast<uint64_t>(s);
      
      uint32_t i = 0;
      // Unroll the inner loop by 8 to maximize throughput
      if (len >= 8) {
        uint32_t limit = len - 7;
        for (; i < limit; i += 8) {
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
          hash = (hash ^ val) * FNV_PRIME;
        }
      }
      
      // Handle remaining elements
      for (; i < len; ++i) {
        hash = (hash ^ val) * FNV_PRIME;
      }
    }
  }

  return hash;
}
