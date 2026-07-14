#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters) {
  (void)dict_size;
  uint32_t* decoded = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!decoded) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) decoded[i] = dict_vals[ids[i]];
    h = 0;
    for (size_t i = 0; i < n; ++i) {
      h = h * 0x100000001b3ULL + (uint64_t)decoded[i];
    }
  }
  free(decoded);
  return h;
}