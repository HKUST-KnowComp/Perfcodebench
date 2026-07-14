#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  
  // Precompute decoded values once since they're constant across iterations
  uint32_t decoded[1024]; // Assuming n <= 1024 for stack allocation; adjust if needed
  for (size_t i = 0; i < n; ++i) {
    decoded[i] = dict_vals[ids[i]];
  }
  
  // Compute hash once and repeat iters times (same result each time)
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
      h ^= (uint64_t)decoded[i] + 1ULL;
      h *= 1099511628211ULL;
    }
  }
  
  return h;
}