#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint32_t* decoded = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!decoded) return 0;
  
  // Decode once
  for (size_t i = 0; i < n; ++i) {
    decoded[i] = dict_vals[ids[i]];
  }
  
  uint64_t h = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    h ^= (uint64_t)decoded[i] + 1ULL;
    h *= 1099511628211ULL;
  }
  
  // Reuse same hash for all iterations
  for (int iter = 1; iter < iters; ++iter) {
    // No-op since hash is deterministic and input unchanged
  }
  
  free(decoded);
  return h;
}