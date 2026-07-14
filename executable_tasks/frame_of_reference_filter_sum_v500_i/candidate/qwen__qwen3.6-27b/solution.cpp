#include "interface.h"
#include <immintrin.h>
#include <vector>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  uint64_t sum = 0;
  size_t n = deltas.size();
  const uint16_t* d_ptr = deltas.data();

  __m256i vbase = _mm256_set1_epi32(base);
  __m256i vlow = _mm256_set1_epi32(low);
  __m256i vhigh = _mm256_set1_epi32(high);
  __m256i vzero = _mm256_setzero_si256();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    size_t i = 0;
    for (; i + 7 < n; i += 8) {
      __m256i vd16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(d_ptr + i));
      __m256i vd32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(vd16));
      __m256i vd32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(vd16, 1));

      __m256i vval_lo = _mm256_add_epi32(vd32_lo, vbase);
      __m256i vval_hi = _mm256_add_epi32(vd32_hi, vbase);

      __m256i diff_lo_low = _mm256_sub_epi32(vval_lo, vlow);
      __m256i diff_lo_high = _mm256_sub_epi32(vhigh, vval_lo);
      __m256i out_lo = _mm256_or_si256(_mm256_cmpgt_epi32(vzero, diff_lo_low), _mm256_cmpgt_epi32(vzero, diff_lo_high));
      __m256i masked_lo = _mm256_andnot_si256(out_lo, vval_lo);
      __m256i s_lo = _mm256_add_epi32(masked_lo, _mm256_srli_si256(masked_lo, 4));
      s_lo = _mm256_add_epi32(s_lo, _mm256_srli_si256(s_lo, 8));
      s_lo = _mm256_add_epi32(s_lo, _mm256_srli_si256(s_lo, 16));
      uint32_t sum_lo = _mm256_extract_epi32(s_lo, 0);

      __m256i diff_hi_low = _mm256_sub_epi32(vval_hi, vlow);
      __m256i diff_hi_high = _mm256_sub_epi32(vhigh, vval_hi);
      __m256i out_hi = _mm256_or_si256(_mm256_cmpgt_epi32(vzero, diff_hi_low), _mm256_cmpgt_epi32(vzero, diff_hi_high));
      __m256i masked_hi = _mm256_andnot_si256(out_hi, vval_hi);
      __m256i s_hi = _mm256_add_epi32(masked_hi, _mm256_srli_si256(masked_hi, 4));
      s_hi = _mm256_add_epi32(s_hi, _mm256_srli_si256(s_hi, 8));
      s_hi = _mm256_add_epi32(s_hi, _mm256_srli_si256(s_hi, 16));
      uint32_t sum_hi = _mm256_extract_epi32(s_hi, 0);

      iter_sum += sum_lo + sum_hi;
    }
    for (; i < n; ++i) {
      uint32_t val = base + d_ptr[i];
      if (val >= low && val <= high) {
        iter_sum += val;
      }
    }
    sum = iter_sum;
  }
  return sum;
}