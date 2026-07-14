#include "interface.h"

#include <cstring>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#define HAS_AVX2
#endif

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t sum = 0;
  const uint32_t* data = values.data();
  const uint32_t* min_data = mins.data();
  const uint32_t* max_data = maxs.data();
  const size_t n = values.size();
  const size_t num_blocks = (n + block_size - 1) / block_size;

  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;

    for (size_t block_idx = 0; block_idx < num_blocks; ++block_idx) {
      const uint32_t block_min = min_data[block_idx];
      const uint32_t block_max = max_data[block_idx];

      // Skip blocks completely outside range
      if (block_max < low || block_min > high) {
        continue;
      }

      const size_t block_start = block_idx * block_size;
      const size_t block_end = (block_start + block_size < n) ? (block_start + block_size) : n;
      const size_t block_len = block_end - block_start;
      const uint32_t* block_data = data + block_start;

      // Fast path: entire block is within range
      if (block_min >= low && block_max <= high) {
#ifdef HAS_AVX2
        size_t i = 0;
        __m256i sum_vec = _mm256_setzero_si256();
        
        // Process 8 uint32_t at a time
        for (; i + 8 <= block_len; i += 8) {
          __m256i vals = _mm256_loadu_si256((__m256i*)(block_data + i));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vals)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vals, 1)));
        }
        
        // Horizontal sum
        __m128i sum128 = _mm_add_epi64(_mm256_castsi256_si128(sum_vec), _mm256_extracti128_si256(sum_vec, 1));
        uint64_t tmp[2];
        _mm_storeu_si128((__m128i*)tmp, sum128);
        sum += tmp[0] + tmp[1];
        
        // Scalar tail
        for (; i < block_len; ++i) {
          sum += static_cast<uint64_t>(block_data[i]);
        }
#else
        // Unrolled scalar sum
        size_t i = 0;
        uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
        for (; i + 4 <= block_len; i += 4) {
          s0 += static_cast<uint64_t>(block_data[i]);
          s1 += static_cast<uint64_t>(block_data[i + 1]);
          s2 += static_cast<uint64_t>(block_data[i + 2]);
          s3 += static_cast<uint64_t>(block_data[i + 3]);
        }
        sum += s0 + s1 + s2 + s3;
        for (; i < block_len; ++i) {
          sum += static_cast<uint64_t>(block_data[i]);
        }
#endif
      } else {
        // Slow path: need to check each value
#ifdef HAS_AVX2
        size_t i = 0;
        __m256i low_vec = _mm256_set1_epi32(low);
        __m256i high_vec = _mm256_set1_epi32(high);
        __m256i sum_vec = _mm256_setzero_si256();
        
        for (; i + 8 <= block_len; i += 8) {
          __m256i vals = _mm256_loadu_si256((__m256i*)(block_data + i));
          __m256i ge_low = _mm256_cmpgt_epi32(vals, _mm256_sub_epi32(low_vec, _mm256_set1_epi32(1)));
          __m256i le_high = _mm256_cmpgt_epi32(_mm256_add_epi32(high_vec, _mm256_set1_epi32(1)), vals);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          __m256i masked = _mm256_and_si256(vals, mask);
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked)));
          sum_vec = _mm256_add_epi64(sum_vec, _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked, 1)));
        }
        
        __m128i sum128 = _mm_add_epi64(_mm256_castsi256_si128(sum_vec), _mm256_extracti128_si256(sum_vec, 1));
        uint64_t tmp[2];
        _mm_storeu_si128((__m128i*)tmp, sum128);
        sum += tmp[0] + tmp[1];
        
        for (; i < block_len; ++i) {
          uint32_t val = block_data[i];
          sum += static_cast<uint64_t>(val) * (val >= low && val <= high);
        }
#else
        for (size_t i = 0; i < block_len; ++i) {
          uint32_t val = block_data[i];
          sum += static_cast<uint64_t>(val) * (val >= low && val <= high);
        }
#endif
      }
    }
  }

  return sum;
}
