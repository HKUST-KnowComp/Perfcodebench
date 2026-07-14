#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  const uint32_t* restrict r_ids = ids;
  const uint32_t* restrict r_dict = dict_vals;
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    h = 1469598103934665603ULL;
    size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      h ^= (uint64_t)r_dict[r_ids[i]] + 1ULL;
      h *= 1099511628211ULL;
      h ^= (uint64_t)r_dict[r_ids[i+1]] + 1ULL;
      h *= 1099511628211ULL;
      h ^= (uint64_t)r_dict[r_ids[i+2]] + 1ULL;
      h *= 1099511628211ULL;
      h ^= (uint64_t)r_dict[r_ids[i+3]] + 1ULL;
      h *= 1099511628211ULL;
    }
    for (; i < n; ++i) {
      h ^= (uint64_t)r_dict[r_ids[i]] + 1ULL;
      h *= 1099511628211ULL;
    }
  }
  return h;
}
