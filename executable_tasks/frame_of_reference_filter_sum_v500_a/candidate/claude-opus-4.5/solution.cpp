#include "interface.h"

#include <immintrin.h>
#include <cstdint>
#include <cstddef>

uint64_t frame_filter_sum(
    const std::vector<uint16_t>& deltas,
    uint32_t base,
    uint32_t low,
    uint32_t high,
    int iters) {
  
  const uint16_t* data = deltas.data();
  const size_t n = deltas.size();
  uint64_t sum = 0;
  
  // Precompute adjusted bounds for range check
  // value >= low && value <= high
  // (base + delta) >= low && (base + delta) <= high
  // delta >= (low - base) && delta <= (high - base)
  // But we need to handle underflow carefully
  
  const __m256i base_vec = _mm256_set1_epi32(static_cast<int32_t>(base));
  const __m256i low_vec = _mm256_set1_epi32(static_cast<int32_t>(low));
  const __m256i high_vec = _mm256_set1_epi32(static_cast<int32_t>(high));
  
  for (int iter = 0; iter < iters; ++iter) {
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    size_t i = 0;
    
    // Process 16 elements at a time (256 bits / 16 bits = 16 elements)
    for (; i + 16 <= n; i += 16) {
      // Load 16 uint16_t values
      __m256i deltas16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + i));
      
      // Zero-extend lower 8 uint16 to 8 uint32
      __m256i deltas32_lo = _mm256_cvtepu16_epi32(_mm256_castsi256_si128(deltas16));
      // Zero-extend upper 8 uint16 to 8 uint32
      __m256i deltas32_hi = _mm256_cvtepu16_epi32(_mm256_extracti128_si256(deltas16, 1));
      
      // Decode: value = base + delta
      __m256i values_lo = _mm256_add_epi32(base_vec, deltas32_lo);
      __m256i values_hi = _mm256_add_epi32(base_vec, deltas32_hi);
      
      // Range check: value >= low && value <= high
      // Using unsigned comparison trick: (value - low) <= (high - low)
      // But simpler: use signed comparison after adjusting
      // For unsigned: cmpgt gives us > , we need >= and <=
      // value >= low: NOT(value < low) = NOT(low > value)
      // value <= high: NOT(value > high)
      
      // cmpgt_epi32 is signed, but our values fit in int32 range (base + uint16 max = ~65535 + base)
      // Use unsigned comparison by subtracting and checking
      
      // Simpler approach: use _mm256_cmpgt_epi32 with adjustment
      // For unsigned comparison a >= b: (a | 0x80000000) >= (b | 0x80000000) as signed
      // Or: a - b doesn't borrow (but complex)
      
      // Actually for values that fit in 31 bits (which they do since max is ~4B but typically much less),
      // signed comparison works. Let's check: base + 65535 could be up to ~4B which exceeds INT32_MAX.
      // So we need unsigned comparison.
      
      // Unsigned comparison: a >= b iff max(a,b) == a
      // a <= b iff min(a,b) == a
      __m256i max_lo_low = _mm256_max_epu32(values_lo, low_vec);
      __m256i min_lo_high = _mm256_min_epu32(values_lo, high_vec);
      __m256i ge_low_lo = _mm256_cmpeq_epi32(max_lo_low, values_lo);  // value >= low
      __m256i le_high_lo = _mm256_cmpeq_epi32(min_lo_high, values_lo); // value <= high
      __m256i mask_lo = _mm256_and_si256(ge_low_lo, le_high_lo);
      
      __m256i max_hi_low = _mm256_max_epu32(values_hi, low_vec);
      __m256i min_hi_high = _mm256_min_epu32(values_hi, high_vec);
      __m256i ge_low_hi = _mm256_cmpeq_epi32(max_hi_low, values_hi);
      __m256i le_high_hi = _mm256_cmpeq_epi32(min_hi_high, values_hi);
      __m256i mask_hi = _mm256_and_si256(ge_low_hi, le_high_hi);
      
      // Apply mask and accumulate
      __m256i masked_lo = _mm256_and_si256(values_lo, mask_lo);
      __m256i masked_hi = _mm256_and_si256(values_hi, mask_hi);
      
      sum_lo = _mm256_add_epi64(sum_lo, _mm256_add_epi64(
        _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_lo)),
        _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_lo, 1))));
      sum_hi = _mm256_add_epi64(sum_hi, _mm256_add_epi64(
        _mm256_cvtepu32_epi64(_mm256_castsi256_si128(masked_hi)),
        _mm256_cvtepu32_epi64(_mm256_extracti128_si256(masked_hi, 1))));
    }
    
    // Horizontal sum of 64-bit accumulators
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    
    // Scalar cleanup
    for (; i < n; ++i) {
      uint32_t value = base + static_cast<uint32_t>(data[i]);
      if (value >= low && value <= high) {
        sum += value;
      }
    }
  }
  
  return sum;
}