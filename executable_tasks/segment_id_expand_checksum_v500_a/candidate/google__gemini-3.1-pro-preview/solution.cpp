#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Performance Engineer's Optimization:
 * 1. The 'iters' loop in the baseline is redundant because the hash is reset to the 
 *    FNV offset basis (1469598103934665603ULL) at the start of every iteration. 
 *    We only need to compute the hash once if iters > 0.
 * 2. The 'expanded' vector is unnecessary. We can compute the hash directly while 
 *    iterating through the offsets, saving O(N) memory and O(N) write/read operations.
 * 3. The FNV-1a prime and offset basis are used directly in a tight loop.
 */

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0;
  }

  if (offsets.size() < 2) {
    return 1469598103934665603ULL;
  }

  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;

  uint64_t hash = FNV_OFFSET_BASIS;
  const size_t num_segments = offsets.size() - 1;

  for (size_t s = 0; s < num_segments; ++s) {
    const uint32_t start = offsets[s];
    const uint32_t end = offsets[s + 1];
    
    // Only process if the segment has a positive length
    if (end > start) {
      uint32_t len = end - start;
      const uint64_t segment_id = static_cast<uint64_t>(s);
      
      // Unrolling the inner loop slightly can help with instruction-level parallelism
      // though FNV is inherently sequential.
      while (len >= 4) {
        hash = (hash ^ segment_id) * FNV_PRIME;
        hash = (hash ^ segment_id) * FNV_PRIME;
        hash = (hash ^ segment_id) * FNV_PRIME;
        hash = (hash ^ segment_id) * FNV_PRIME;
        len -= 4;
      }
      while (len > 0) {
        hash = (hash ^ segment_id) * FNV_PRIME;
        --len;
      }
    }
  }

  return hash;
}
