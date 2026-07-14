#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  uint64_t h = 1469598103934665603ULL;
  size_t i = 0;
  
  for (; i + 7 < n; i += 8) {
    uint64_t v0 = dict_vals[ids[i]];
    uint64_t v1 = dict_vals[ids[i+1]];
    uint64_t v2 = dict_vals[ids[i+2]];
    uint64_t v3 = dict_vals[ids[i+3]];
    uint64_t v4 = dict_vals[ids[i+4]];
    uint64_t v5 = dict_vals[ids[i+5]];
    uint64_t v6 = dict_vals[ids[i+6]];
    uint64_t v7 = dict_vals[ids[i+7]];

    h ^= v0 + 1ULL; h *= 1099511628211ULL;
    h ^= v1 + 1ULL; h *= 1099511628211ULL;
    h ^= v2 + 1ULL; h *= 1099511628211ULL;
    h ^= v3 + 1ULL; h *= 1099511628211ULL;
    h ^= v4 + 1ULL; h *= 1099511628211ULL;
    h ^= v5 + 1ULL; h *= 1099511628211ULL;
    h ^= v6 + 1ULL; h *= 1099511628211ULL;
    h ^= v7 + 1ULL; h *= 1099511628211ULL;
  }
  
  for (; i < n; ++i) {
    h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
    h *= 1099511628211ULL;
  }
  
  return h;
}
