#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  /* No iterations => result is initial 0 */
  if (iters <= 0) return 0;
  /* No elements => hash stays at the starting constant each iter */
  if (n == 0) {
    uint64_t h = 1469598103934665603ULL;
    return h;
  }

  uint32_t* decoded = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!decoded) return 0;   /* allocation failure */

  /* Pre‑decode all ids once */
  for (size_t i = 0; i < n; ++i) {
    decoded[i] = dict_vals[ids[i]];
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t i = 0;
    /* Unrolled loop processing 4 elements at a time */
    for (; i + 3 < n; i += 4) {
      hash ^= (uint64_t)decoded[i] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)decoded[i+1] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)decoded[i+2] + 1ULL;
      hash *= 1099511628211ULL;
      hash ^= (uint64_t)decoded[i+3] + 1ULL;
      hash *= 1099511628211ULL;
    }
    /* Remainder */
    for (; i < n; ++i) {
      hash ^= (uint64_t)decoded[i] + 1ULL;
      hash *= 1099511628211ULL;
    }
  }

  free(decoded);
  return hash;
}
