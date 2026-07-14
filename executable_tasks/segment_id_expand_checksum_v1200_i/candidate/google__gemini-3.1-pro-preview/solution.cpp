#include "interface.h"
#include <vector>
#include <cstdint>

/**
 * Performance Engineer's Optimization:
 * 
 * 1. Redundancy Elimination: The baseline implementation resets the hash and clears the 
 *    'expanded' vector inside the 'iters' loop. This means the final hash is identical 
 *    regardless of the number of iterations (for iters >= 1). We only need to compute it once.
 * 
 * 2. Memory Efficiency: The 'expanded' vector is unnecessary. We can calculate the hash 
 *    directly by iterating through the segments and their lengths, avoiding expensive 
 *    allocations and memory writes.
 * 
 * 3. Loop Unrolling: The inner loop performing the FNV-1a mix is the hot path. 
 *    By calculating the hash directly, we reduce the complexity from O(iters * N) to O(N).
 */

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0 || offsets.size() < 2) {
    return 0;
  }

  // FNV-1a constants
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;

  uint64_t hash = FNV_OFFSET_BASIS;

  // Process segments directly from offsets
  const size_t num_segments = offsets.size() - 1;
  for (size_t s = 0; s < num_segments; ++s) {
    const uint32_t start = offsets[s];
    const uint32_t end = offsets[s + 1];
    
    // Skip empty segments
    if (end <= start) continue;

    const uint32_t len = end - start;
    const uint32_t segment_id = static_cast<uint32_t>(s);

    // Inner loop: Apply FNV-1a mix 'len' times for the current segment ID
    // The compiler can optimize this loop effectively as segment_id is constant here.
    for (uint32_t i = 0; i < len; ++i) {
      hash ^= segment_id;
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
