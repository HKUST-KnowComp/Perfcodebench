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
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  if (n_intervals == 0 || n_probes == 0) {
    return 0;
  }
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 8 probes at a time using AVX2
    for (; i + 8 <= n_probes; i += 8) {
      // Prefetch future data
      if (i + 32 < n_probes) {
        _mm_prefetch(reinterpret_cast<const char*>(probes_ptr + i + 32), _MM_HINT_T0);
        _mm_prefetch(reinterpret_cast<const char*>(weights_ptr + i + 32), _MM_HINT_T0);
      }
      
      // Load 8 probes and weights
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Vectorized binary search (upper_bound - 1)
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i ones = _mm256_set1_epi32(1);
        __m256i all_one = _mm256_cmpeq_epi32(diff, ones);
        if (_mm256_movemask_epi8(all_one) == -1) break;
        
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        
        // Gather starts[mid]
        __m256i starts_mid = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] <= probe ? (we want upper_bound, so starts[mid] <= probe means go right)
        __m256i cmp = _mm256_cmpgt_epi32(probe_vec, _mm256_sub_epi32(starts_mid, ones));
        
        // Update lo and hi based on comparison
        lo = _mm256_blendv_epi8(lo, mid, cmp);
        hi = _mm256_blendv_epi8(mid, hi, cmp);
      }
      
      // lo now contains the indices (upper_bound - 1)
      __m256i idx_vec = lo;
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx_vec, 4);
      
      // Multiply payload * weight (need 64-bit result)
      // Split into low and high 4 elements
      __m256i payload_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(payload_vec));
      __m256i payload_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(payload_vec, 1));
      __m256i weight_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(weight_vec));
      __m256i weight_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(weight_vec, 1));
      
      // 64-bit multiply
      __m256i prod_lo = _mm256_mul_epu32(payload_lo, weight_lo);
      __m256i prod_hi = _mm256_mul_epu32(payload_hi, weight_hi);
      
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
    }
    
    // Horizontal sum of vector accumulators
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_128 = _mm_add_epi64(_mm256_castsi256_si128(total), _mm256_extracti128_si256(total, 1));
    total_128 = _mm_add_epi64(total_128, _mm_srli_si128(total_128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(total_128));
    
    // Handle remaining probes with scalar code
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}