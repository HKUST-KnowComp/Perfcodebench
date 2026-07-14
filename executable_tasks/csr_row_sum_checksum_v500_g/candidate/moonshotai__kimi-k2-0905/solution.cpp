#include "interface.h"
#include <cstddef>
#include <immintrin.h>

#define UNROLL 8

static inline uint64_t mix_inline(uint64_t h, uint64_t s) {
  h ^= s;
  h *= 1099511628211ULL;
  return h;
}

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters)
{
  if (row_ptr.size() <= 1) return 0;
  const uint32_t * __restrict rp = row_ptr.data();
  const uint32_t * __restrict vp = values.data();
  const size_t nrows = row_ptr.size() - 1;

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (size_t r = 0; r < nrows; ++r) {
      const uint32_t beg = rp[r];
      const uint32_t end = rp[r + 1];

      const size_t len = end - beg;
      const uint32_t * v = vp + beg;

      uint64_t sum = 0;
      size_t i = 0;

      /* inner unrolled reduction */
      for (; i + UNROLL <= len; i += UNROLL) {
        sum += v[i + 0];
        sum += v[i + 1];
        sum += v[i + 2];
        sum += v[i + 3];
        sum += v[i + 4];
        sum += v[i + 5];
        sum += v[i + 6];
        sum += v[i + 7];
#ifndef __NO_PREFETCH
        _mm_prefetch((const char *)((const void *)(v + i + 256)), _MM_HINT_T0);
#endif
      }
      for (; i < len; ++i) sum += v[i];
      hash = mix_inline(hash, sum);
    }
  }
  return hash;
}