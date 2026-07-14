#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  uint64_t sum = 0;
  const std::size_t n = values.size();
  if (n == 0) return 0;

  const uint32_t* re = run_ends.data();
  const uint32_t* va = values.data();

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    uint32_t prev = 0;
    std::size_t i = 0;

#if defined(__AVX2__)
    if (n >= 8) {
      __m256i sum_lo = _mm256_setzero_si256();
      __m256i sum_hi = _mm256_setzero_si256();
      __m256i prev_vec = _mm256_setzero_si256();

      for (; i + 7 < n; i += 8) {
        __m256i ends = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(re + i));
        __m256i vals = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(va + i));

        // Compute lengths: for position k in [0..7], length[k] = ends[k] - prev_for_k
        // prev_for_k: prev_vec shifted right by one lane, with prev inserted at lane 0
        // Shift prev_vec: [p0,p1,p2,p3,p4,p5,p6,p7] -> [prev,p0,p1,p2,p3,p4,p5,p6]
        // Use permute and blend
        __m256i shifted = _mm256_permutevar8x32_epi32(ends, _mm256_set_epi32(6,5,4,3,2,1,0,7));
        // Now shifted = [ends[7], ends[0], ends[1], ends[2], ends[3], ends[4], ends[5], ends[6]]
        // We need [prev, ends[0], ends[1], ..., ends[6]]
        // Insert prev at position 0
        shifted = _mm256_insert_epi32(shifted, static_cast<int>(prev), 0);

        __m256i lens = _mm256_sub_epi32(ends, shifted);

        // Multiply vals * lens (32-bit), accumulate as 64-bit
        // _mm256_mul_epu32 multiplies lanes 0,2,4,6 and produces 64-bit results
        __m256i prod_lo = _mm256_mul_epu32(vals, lens);
        // Shift right by 4 bytes to get lanes 1,3,5,7
        __m256i vals_hi = _mm256_srli_epi64(vals, 32);
        __m256i lens_hi = _mm256_srli_epi64(lens, 32);
        __m256i prod_hi = _mm256_mul_epu32(vals_hi, lens_hi);

        sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
        sum_hi = _mm256_add_epi64(sum_hi, prod_hi);

        prev = re[i + 7];
        prev_vec = ends;
      }

      // Horizontal sum of sum_lo and sum_hi
      __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
      __m128i lo128 = _mm256_castsi256_si128(total);
      __m128i hi128 = _mm256_extracti128_si256(total, 1);
      __m128i sum128 = _mm_add_epi64(lo128, hi128);
      sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) +
            static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    }
#endif

    // Scalar tail
    for (; i < n; ++i) {
      uint32_t len = re[i] - prev;
      sum += static_cast<uint64_t>(va[i]) * len;
      prev = re[i];
    }
  }
  return sum;
}