#include <stddef.h>
#include <stdint.h>

/**
 * Optimized dictionary decode and checksum calculation.
 * 
 * Performance improvements:
 * 1. Eliminated the 'decoded' array allocation and the separate decoding loop.
 *    This reduces memory bandwidth requirements and cache footprint.
 * 2. Combined decoding (dict_vals[ids[i]]) and hashing into a single loop.
 * 3. Applied loop unrolling (factor of 8) to the main loop. This allows the CPU
 *    to issue multiple independent memory loads from 'dict_vals' in parallel,
 *    hiding memory latency while maintaining the serial dependency of the FNV-1a hash.
 * 4. Used a local constant for the FNV prime and offset basis.
 */
uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  
  const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  uint64_t h = 0;

  for (int iter = 0; iter < iters; ++iter) {
    h = FNV_OFFSET_BASIS;
    size_t i = 0;

    // Unroll loop to process 8 elements at a time
    for (; i + 7 < n; i += 8) {
      // Fetch values first to encourage parallel memory access
      uint32_t v0 = dict_vals[ids[i]];
      uint32_t v1 = dict_vals[ids[i + 1]];
      uint32_t v2 = dict_vals[ids[i + 2]];
      uint32_t v3 = dict_vals[ids[i + 3]];
      uint32_t v4 = dict_vals[ids[i + 4]];
      uint32_t v5 = dict_vals[ids[i + 5]];
      uint32_t v6 = dict_vals[ids[i + 6]];
      uint32_t v7 = dict_vals[ids[i + 7]];

      // Serial hash updates
      h = (h ^ ((uint64_t)v0 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v1 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v2 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v3 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v4 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v5 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v6 + 1ULL)) * FNV_PRIME;
      h = (h ^ ((uint64_t)v7 + 1ULL)) * FNV_PRIME;
    }

    // Handle remaining elements
    for (; i < n; ++i) {
      h = (h ^ ((uint64_t)dict_vals[ids[i]] + 1ULL)) * FNV_PRIME;
    }
  }

  return h;
}
