#include "interface.h"
#include <immintrin.h>

uint64_t row_group_range_sum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& mins,
    const std::vector<uint32_t>& maxs,
    uint32_t low,
    uint32_t high,
    uint32_t block_size,
    int iters) {
  uint64_t sum = 0;
  const size_t n = values.size();
  const size_t num_blocks = mins.size();
  const uint32_t* __restrict vptr = values.data();
  const uint32_t* __restrict minptr = mins.data();
  const uint32_t* __restrict maxptr = maxs.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = minptr[b];
      uint32_t block_max = maxptr[b];
      
      // Skip block entirely if no overlap with [low, high]
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within [low, high]
      if (block_min >= low && block_max <= high) {
        // Sum all values in block
        uint64_t block_sum = 0;
        size_t i = start;
        
        // Use SIMD for summing
        __m256i vsum = _mm256_setzero_si256();
        for (; i + 8 <= end; i += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(vptr + i));
          // Extend to 64-bit and accumulate
          __m128i lo = _mm256_castsi256_si128(v);
          __m128i hi = _mm256_extracti128_si256(v, 1);
          __m256i lo64 = _mm256_cvtepu32_epi64(lo);
          __m256i hi64 = _mm256_cvtepu32_epi64(hi);
          vsum = _mm256_add_epi64(vsum, lo64);
          vsum = _mm256_add_epi64(vsum, hi64);
        }
        
        // Horizontal sum
        __m128i vsum_lo = _mm256_castsi256_si128(vsum);
        __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
        __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
        block_sum = _mm_extract_epi64(vsum128, 0) + _mm_extract_epi64(vsum128, 1);
        
        // Scalar tail
        for (; i < end; ++i) {
          block_sum += vptr[i];
        }
        sum += block_sum;
      } else {
        // Partial overlap - need to check each value
        size_t i = start;
        uint64_t block_sum = 0;
        
        // SIMD with masking for partial blocks
        __m256i vlow = _mm256_set1_epi32(low);
        __m256i vhigh = _mm256_set1_epi32(high);
        __m256i vsum = _mm256_setzero_si256();
        
        for (; i + 8 <= end; i += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(vptr + i));
          
          // Check v >= low: use signed comparison trick
          // For unsigned: a >= b iff max(a,b) == a
          __m256i ge_low = _mm256_cmpeq_epi32(_mm256_max_epu32(v, vlow), v);
          // Check v <= high: max(v, high) == high
          __m256i le_high = _mm256_cmpeq_epi32(_mm256_max_epu32(v, vhigh), vhigh);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          
          // Zero out values not in range
          __m256i masked = _mm256_and_si256(v, mask);
          
          // Extend to 64-bit and accumulate
          __m128i lo = _mm256_castsi256_si128(masked);
          __m128i hi = _mm256_extracti128_si256(masked, 1);
          __m256i lo64 = _mm256_cvtepu32_epi64(lo);
          __m256i hi64 = _mm256_cvtepu32_epi64(hi);
          vsum = _mm256_add_epi64(vsum, lo64);
          vsum = _mm256_add_epi64(vsum, hi64);
        }
        
        // Horizontal sum
        __m128i vsum_lo = _mm256_castsi256_si128(vsum);
        __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
        __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
        block_sum = _mm_extract_epi64(vsum128, 0) + _mm_extract_epi64(vsum128, 1);
        
        // Scalar tail
        for (; i < end; ++i) {
          uint32_t val = vptr[i];
          if (val >= low && val <= high) {
            block_sum += val;
          }
        }
        sum += block_sum;
      }
    }
  }
  return sum;
}