#include "interface.h"

#include <algorithm>
#include <cstdint>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

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
    
#if defined(__AVX2__)
    // Process 8 probes at a time using AVX2
    const std::size_t vec_end = (n_probes / 8) * 8;
    
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    for (std::size_t i = 0; i < vec_end; i += 8) {
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Vectorized binary search for 8 probes
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations (log2(n_intervals) iterations needed)
      // For safety, do enough iterations for large arrays
      for (int b = 0; b < 32; ++b) {
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        
        // Gather starts[mid] for each lane
        __m256i starts_mid = _mm256_i32gather_epi32(
            reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] <= probe ? update lo : update hi
        // We want upper_bound behavior: find first index where starts[idx] > probe
        // So we update lo = mid + 1 when starts[mid] <= probe
        __m256i cmp = _mm256_cmpgt_epi32(probe_vec, _mm256_sub_epi32(starts_mid, _mm256_set1_epi32(1)));
        // cmp is all 1s where probe >= starts[mid], i.e., starts[mid] <= probe
        
        __m256i mid_plus_1 = _mm256_add_epi32(mid, _mm256_set1_epi32(1));
        lo = _mm256_blendv_epi8(lo, mid_plus_1, cmp);
        hi = _mm256_blendv_epi8(mid, hi, cmp);
      }
      
      // lo now contains upper_bound indices, we need idx = lo - 1
      __m256i idx = _mm256_sub_epi32(lo, _mm256_set1_epi32(1));
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(
          reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Multiply payload * weight and accumulate as 64-bit
      // Split into low and high 4 elements
      __m256i prod_lo = _mm256_mul_epu32(payload_vec, weight_vec);
      __m256i prod_hi = _mm256_mul_epu32(
          _mm256_srli_epi64(payload_vec, 32),
          _mm256_srli_epi64(weight_vec, 32));
      
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
    }
    
    // Horizontal sum of 64-bit accumulators
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i sum128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    
    // Handle remaining probes with scalar code
    for (std::size_t i = vec_end; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }
#else
    // Scalar fallback
    for (std::size_t i = 0; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads[idx]) * static_cast<uint64_t>(weights[i]);
    }
#endif
  }
  
  return sum;
}