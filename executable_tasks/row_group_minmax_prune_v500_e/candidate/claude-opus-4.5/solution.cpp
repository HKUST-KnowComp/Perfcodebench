#include "interface.h"
#include <cstdint>
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
  const uint32_t* vptr = values.data();
  
  // Precompute block sums for fully-included blocks
  std::vector<uint64_t> block_sums(num_blocks, 0);
  for (size_t b = 0; b < num_blocks; ++b) {
    size_t start = b * block_size;
    size_t end = start + block_size;
    if (end > n) end = n;
    uint64_t bs = 0;
    for (size_t i = start; i < end; ++i) {
      bs += vptr[i];
    }
    block_sums[b] = bs;
  }
  
  __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    for (size_t b = 0; b < num_blocks; ++b) {
      uint32_t block_min = mins[b];
      uint32_t block_max = maxs[b];
      
      // Skip block entirely if no overlap
      if (block_max < low || block_min > high) {
        continue;
      }
      
      size_t start = b * block_size;
      size_t end = start + block_size;
      if (end > n) end = n;
      
      // Include entire block if fully within range
      if (block_min >= low && block_max <= high) {
        sum += block_sums[b];
        continue;
      }
      
      // Partial scan with SIMD
      size_t i = start;
      uint64_t partial = 0;
      
      // Process 8 elements at a time
      size_t simd_end = start + ((end - start) / 8) * 8;
      __m256i acc_lo = _mm256_setzero_si256();
      __m256i acc_hi = _mm256_setzero_si256();
      
      for (; i < simd_end; i += 8) {
        __m256i v = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(vptr + i));
        
        // Compare: value >= low && value <= high
        // For unsigned comparison, we use signed comparison with offset trick
        // Or use _mm256_cmpgt_epi32 with careful handling
        // Actually, for unsigned: a >= b is equivalent to (a - b) doesn't underflow
        // Simpler: use signed comparison after adjusting for unsigned
        
        // mask_ge_low: v >= low
        // mask_le_high: v <= high
        // For unsigned comparison using signed instructions:
        // v >= low (unsigned) => (v ^ 0x80000000) >= (low ^ 0x80000000) (signed)
        
        __m256i bias = _mm256_set1_epi32(static_cast<int32_t>(0x80000000u));
        __m256i v_biased = _mm256_xor_si256(v, bias);
        __m256i low_biased = _mm256_xor_si256(low_vec, bias);
        __m256i high_biased = _mm256_xor_si256(high_vec, bias);
        
        // v >= low: NOT(v < low) = NOT(low > v)
        __m256i cmp_ge_low = _mm256_andnot_si256(_mm256_cmpgt_epi32(low_biased, v_biased), _mm256_set1_epi32(-1));
        // v <= high: NOT(v > high)
        __m256i cmp_le_high = _mm256_andnot_si256(_mm256_cmpgt_epi32(v_biased, high_biased), _mm256_set1_epi32(-1));
        
        __m256i mask = _mm256_and_si256(cmp_ge_low, cmp_le_high);
        __m256i masked_v = _mm256_and_si256(v, mask);
        
        // Extend to 64-bit and accumulate
        __m256i lo = _mm256_unpacklo_epi32(masked_v, _mm256_setzero_si256());
        __m256i hi = _mm256_unpackhi_epi32(masked_v, _mm256_setzero_si256());
        acc_lo = _mm256_add_epi64(acc_lo, lo);
        acc_hi = _mm256_add_epi64(acc_hi, hi);
      }
      
      // Horizontal sum of accumulators
      __m256i acc_total = _mm256_add_epi64(acc_lo, acc_hi);
      __m128i acc_128 = _mm_add_epi64(_mm256_extracti128_si256(acc_total, 0), _mm256_extracti128_si256(acc_total, 1));
      partial = static_cast<uint64_t>(_mm_extract_epi64(acc_128, 0)) + static_cast<uint64_t>(_mm_extract_epi64(acc_128, 1));
      
      // Handle remaining elements
      for (; i < end; ++i) {
        uint32_t val = vptr[i];
        if (val >= low && val <= high) {
          partial += val;
        }
      }
      
      sum += partial;
    }
  }
  
  return sum;
}