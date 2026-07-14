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
  
  const size_t n = values.size();
  const size_t num_blocks = mins.size();
  const uint32_t* data = values.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = mins[b];
      uint32_t block_max = maxs[b];
      
      // Prune: block entirely outside range
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block: block entirely inside range
      if (block_min >= low && block_max <= high) {
        // Sum all values in block without filtering
        size_t i = start;
        
        // AVX2 vectorized sum for full block inclusion
        __m256i vsum = _mm256_setzero_si256();
        for (; i + 8 <= end; i += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
          // Extend to 64-bit and accumulate
          __m128i lo128 = _mm256_castsi256_si128(v);
          __m128i hi128 = _mm256_extracti128_si256(v, 1);
          __m256i lo64 = _mm256_cvtepu32_epi64(lo128);
          __m256i hi64 = _mm256_cvtepu32_epi64(hi128);
          vsum = _mm256_add_epi64(vsum, lo64);
          vsum = _mm256_add_epi64(vsum, hi64);
        }
        
        // Horizontal sum
        uint64_t tmp[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), vsum);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
        
        // Scalar remainder
        for (; i < end; ++i) {
          sum += data[i];
        }
      } else {
        // Partial overlap: must check each value
        size_t i = start;
        
        // AVX2 vectorized filtered sum
        __m256i vlow = _mm256_set1_epi32(static_cast<int32_t>(low));
        __m256i vhigh = _mm256_set1_epi32(static_cast<int32_t>(high));
        __m256i vsum = _mm256_setzero_si256();
        
        for (; i + 8 <= end; i += 8) {
          __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
          
          // Unsigned comparison: v >= low && v <= high
          // For unsigned: a >= b is equivalent to max(a,b) == a
          // For unsigned: a <= b is equivalent to min(a,b) == a
          __m256i ge_low = _mm256_cmpeq_epi32(_mm256_max_epu32(v, vlow), v);
          __m256i le_high = _mm256_cmpeq_epi32(_mm256_min_epu32(v, vhigh), v);
          __m256i mask = _mm256_and_si256(ge_low, le_high);
          
          // Masked values (zero out values outside range)
          __m256i masked = _mm256_and_si256(v, mask);
          
          // Extend to 64-bit and accumulate
          __m128i lo128 = _mm256_castsi256_si128(masked);
          __m128i hi128 = _mm256_extracti128_si256(masked, 1);
          __m256i lo64 = _mm256_cvtepu32_epi64(lo128);
          __m256i hi64 = _mm256_cvtepu32_epi64(hi128);
          vsum = _mm256_add_epi64(vsum, lo64);
          vsum = _mm256_add_epi64(vsum, hi64);
        }
        
        // Horizontal sum
        uint64_t tmp[4];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), vsum);
        sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
        
        // Scalar remainder
        for (; i < end; ++i) {
          uint32_t val = data[i];
          if (val >= low && val <= high) {
            sum += val;
          }
        }
      }
    }
  }
  
  return sum;
}