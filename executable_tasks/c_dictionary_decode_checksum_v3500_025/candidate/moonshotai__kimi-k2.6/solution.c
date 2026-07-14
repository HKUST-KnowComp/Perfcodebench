#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  if (iters <= 0) return 0;

  uint64_t h = 1469598103934665603ULL;
  const uint32_t* d = dict_vals;
  const uint32_t* s = ids;
  size_t i = 0;

  for (; i + 7 < n; i += 8) {
    h ^= (uint64_t)d[s[i]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+1]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+2]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+3]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+4]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+5]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+6]] + 1ULL;
    h *= 1099511628211ULL;
    h ^= (uint64_t)d[s[i+7]] + 1ULL;
    h *= 1099511628211ULL;
  }

  for (; i < n; ++i) {
    h ^= (uint64_t)d[s[i]] + 1ULL;
    h *= 1099511628211ULL;
  }

  return h;
}
