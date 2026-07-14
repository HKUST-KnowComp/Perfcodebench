#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  (void)iters;
  uint32_t* decoded = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!decoded) return 0;
  
  for (size_t i = 0; i < n; ++i) {
    decoded[i] = dict_vals[ids[i]];
  }
  
  uint64_t h = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    h ^= (uint64_t)decoded[i] + 1ULL;
    h *= 1099511628211ULL;
  }
  
  free(decoded);
  return h;
}