#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <immintrin.h>

static __m128i blend_pixel(__m128i src, __m128i dst, __m128i alpha) {
  __m128i zero = _mm_setzero_si128();
  __m128i one = _mm_set1_epi32(1);
  __m128i a = _mm_unpacklo_epi8(alpha, zero);
  __m128i s = _mm_unpacklo_epi8(src, zero);
  __m128i d = _mm_unpacklo_epi8(dst, zero);
  __m128i sa = _mm_mullo_epi32(s, a);
  __m128i da = _mm_mullo_epi32(d, _mm_sub_epi32(_mm_set1_epi32(255), a));
  __m128i res = _mm_add_epi32(sa, da);
  res = _mm_add_epi32(res, _mm_set1_epi32(127));
  res = _mm_div_epi32(res, _mm_set1_epi32(255));
  res = _mm_packus_epi32(res, zero);
  return res;
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  __m128i* out = (__m128i*)_mm_malloc(pixels * 4u, 16);
  if (!out) return 0;
  __m128i* src_vec = (__m128i*)src;
  __m128i* dst_vec = (__m128i*)dst;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < pixels; ++i) {
      __m128i s = src_vec[i];
      __m128i d = dst_vec[i];
      __m128i a = _mm_unpacklo_epi8(_mm_shuffle_epi32(s, 0xFF), _mm_setzero_si128());
      __m128i r = blend_pixel(s, d, a);
      out[i] = _mm_set_epi32(0xFF, r.m128i_u32[1], r.m128i_u32[2], r.m128i_u32[3]);
    }
    hash = checksum_bytes((uint8_t*)out, pixels * 4u);
  }
  _mm_free(out);
  return hash;
}