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
  
  const uint32_t* __restrict starts_ptr = starts.data();
  const uint32_t* __restrict payloads_ptr = payloads.data();
  const uint32_t* __restrict probes_ptr = probes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
#if defined(__AVX2__)
    const std::size_t vec_end = (n_probes / 8) * 8;
    
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    for (std::size_t i = 0; i < vec_end; i += 8) {
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Binary search for 8 probes in parallel
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i max_diff = _mm256_set1_epi32(1);
        __m256i cmp_done = _mm256_cmpgt_epi32(diff, max_diff);
        if (_mm256_testz_si256(cmp_done, cmp_done)) break;
        
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        __m256i starts_at_mid = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        __m256i cmp = _mm256_cmpgt_epi32(starts_at_mid, probe_vec);
        
        lo = _mm256_blendv_epi8(mid, lo, cmp);
        hi = _mm256_blendv_epi8(hi, mid, cmp);
      }
      
      // idx = lo - 1, but we need lo (which is the upper_bound result - 1 already due to our search)
      // Actually we found the largest index where starts[idx] <= probe
      __m256i idx = lo;
      
      // Gather payloads
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Multiply and accumulate as 64-bit
      __m256i prod_lo = _mm256_mul_epu32(payload_vec, weight_vec);
      __m256i payload_hi = _mm256_srli_epi64(payload_vec, 32);
      __m256i weight_hi = _mm256_srli_epi64(weight_vec, 32);
      __m256i prod_hi = _mm256_mul_epu32(payload_hi, weight_hi);
      
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
    }
    
    // Horizontal sum
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i lo128 = _mm256_castsi256_si128(total);
    __m128i hi128 = _mm256_extracti128_si256(total, 1);
    __m128i sum128 = _mm_add_epi64(lo128, hi128);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    
    // Handle remaining elements
    for (std::size_t i = vec_end; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#else
    // Scalar fallback with unrolling
    const std::size_t unroll_end = (n_probes / 4) * 4;
    
    for (std::size_t i = 0; i < unroll_end; i += 4) {
      const auto it0 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i]);
      const auto it1 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i+1]);
      const auto it2 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i+2]);
      const auto it3 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i+3]);
      
      const std::size_t idx0 = static_cast<std::size_t>(it0 - starts_ptr - 1);
      const std::size_t idx1 = static_cast<std::size_t>(it1 - starts_ptr - 1);
      const std::size_t idx2 = static_cast<std::size_t>(it2 - starts_ptr - 1);
      const std::size_t idx3 = static_cast<std::size_t>(it3 - starts_ptr - 1);
      
      sum += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i+1]);
      sum += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i+2]);
      sum += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i+3]);
    }
    
    for (std::size_t i = unroll_end; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#endif
  }
  
  return sum;
}