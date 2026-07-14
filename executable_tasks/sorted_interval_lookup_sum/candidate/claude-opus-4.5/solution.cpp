#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <immintrin.h>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)ends;
  
  const std::size_t n = probes.size();
  const std::size_t m = starts.size();
  
  // Precompute indices for all probes
  std::vector<uint32_t> indices(n);
  for (std::size_t i = 0; i < n; ++i) {
    const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
    indices[i] = static_cast<uint32_t>(it - starts.begin() - 1);
  }
  
  uint64_t sum = 0;
  const uint32_t* payload_ptr = payloads.data();
  const uint32_t* weight_ptr = weights.data();
  const uint32_t* idx_ptr = indices.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
#ifdef __AVX2__
    // Process 8 elements at a time using AVX2
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    for (; i + 8 <= n; i += 8) {
      // Load 8 indices
      __m256i vidx = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(idx_ptr + i));
      
      // Gather 8 payloads using indices
      __m256i vpayloads = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payload_ptr), vidx, 4);
      
      // Load 8 weights
      __m256i vweights = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weight_ptr + i));
      
      // Multiply: need 64-bit results
      // Split into low and high 4 elements
      __m256i p_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vpayloads));
      __m256i p_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vpayloads, 1));
      __m256i w_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vweights));
      __m256i w_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vweights, 1));
      
      // 64-bit multiply
      __m256i prod_lo = _mm256_mul_epu32(p_lo, w_lo);
      __m256i prod_hi = _mm256_mul_epu32(p_hi, w_hi);
      
      // For the odd elements (indices 1, 3), we need to shift and multiply separately
      __m256i p_lo_odd = _mm256_srli_epi64(p_lo, 32);
      __m256i w_lo_odd = _mm256_srli_epi64(w_lo, 32);
      __m256i p_hi_odd = _mm256_srli_epi64(p_hi, 32);
      __m256i w_hi_odd = _mm256_srli_epi64(w_hi, 32);
      
      __m256i prod_lo_odd = _mm256_mul_epu32(p_lo_odd, w_lo_odd);
      __m256i prod_hi_odd = _mm256_mul_epu32(p_hi_odd, w_hi_odd);
      
      // Accumulate all products
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo_odd);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi_odd);
    }
    
    // Horizontal sum of sum_lo and sum_hi
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i total128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(total128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(total128, 1));
#endif
    
    // Handle remaining elements
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(payloads[indices[i]]) * static_cast<uint64_t>(weights[i]);
    }
  }
  
  return sum;
}