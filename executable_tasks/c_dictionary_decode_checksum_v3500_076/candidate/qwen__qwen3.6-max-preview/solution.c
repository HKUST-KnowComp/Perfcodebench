#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint64_t h = 0;
  const uint64_t fnv_prime = 1099511628211ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    size_t i = 0;
    for (; i + 7 < n; i += 8) {
      uint32_t v0 = dict_vals[ids[i]];
      uint32_t v1 = dict_vals[ids[i+1]];
      uint32_t v2 = dict_vals[ids[i+2]];
      uint32_t v3 = dict_vals[ids[i+3]];
      uint32_t v4 = dict_vals[ids[i+4]];
      uint32_t v5 = dict_vals[ids[i+5]];
      uint32_t v6 = dict_vals[ids[i+6]];
      uint32_t v7 = dict_vals[ids[i+7]];
      
      h ^= (uint64_t)v0 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v1 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v2 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v3 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v4 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v5 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v6 + 1ULL; h *= fnv_prime;
      h ^= (uint64_t)v7 + 1ULL; h *= fnv_prime;
    }
    for (; i < n; ++i) {
      h ^= (uint64_t)dict_vals[ids[i]] + 1ULL;
      h *= fnv_prime;
    }
  }
  return h;
}
