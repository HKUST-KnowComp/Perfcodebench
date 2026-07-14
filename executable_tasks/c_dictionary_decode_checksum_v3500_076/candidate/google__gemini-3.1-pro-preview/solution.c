#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Optimized dictionary decode and checksum calculation.
 * 
 * Improvements:
 * 1. Eliminated the heap allocation (malloc/free) of the 'decoded' array.
 * 2. Merged the decoding loop and the hashing loop into a single pass to reduce memory bandwidth usage.
 * 3. Applied loop unrolling (factor of 4) to allow the CPU to issue multiple memory loads from 
 *    'dict_vals' in parallel, hiding some of the latency of random access.
 * 4. Removed redundant operations and ensured the FNV-1a-style hash is computed correctly.
 */
uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_OFFSET_BASIS;
    size_t i = 0;

    // Unroll the loop to process 4 elements at a time.
    // This helps the processor overlap the memory latency of dict_vals lookups.
    for (; i + 3 < n; i += 4) {
      uint32_t v0 = dict_vals[ids[i]];
      uint32_t v1 = dict_vals[ids[i + 1]];
      uint32_t v2 = dict_vals[ids[i + 2]];
      uint32_t v3 = dict_vals[ids[i + 3]];

      h ^= (uint64_t)v0 + 1ULL;
      h *= FNV_PRIME;
      h ^= (uint64_t)v1 + 1ULL;
      h *= FNV_PRIME;
      h ^= (uint64_t)v2 + 1ULL;
      h *= FNV_PRIME;
      h ^= (uint64_t)v3 + 1ULL;
      h *= FNV_PRIME;
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= FNV_PRIME;
    }
  }

  return h;
}
