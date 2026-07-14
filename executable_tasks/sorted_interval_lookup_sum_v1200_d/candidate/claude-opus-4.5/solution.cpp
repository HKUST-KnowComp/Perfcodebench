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
  
  const uint32_t* __restrict__ starts_ptr = starts.data();
  const uint32_t* __restrict__ payloads_ptr = payloads.data();
  const uint32_t* __restrict__ probes_ptr = probes.data();
  const uint32_t* __restrict__ weights_ptr = weights.data();
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    std::size_t i = 0;
    
    // Process 8 probes at a time using AVX2
    const std::size_t vec_end = (n_probes >= 8) ? (n_probes - 7) : 0;
    
    for (; i < vec_end; i += 8) {
      // Prefetch future data
      if (i + 64 < n_probes) {
        _mm_prefetch(reinterpret_cast<const char*>(probes_ptr + i + 64), _MM_HINT_T0);
        _mm_prefetch(reinterpret_cast<const char*>(weights_ptr + i + 64), _MM_HINT_T0);
      }
      
      // Load 8 probes
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      
      // Vectorized binary search - find upper_bound - 1 for each probe
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i ones = _mm256_set1_epi32(1);
        __m256i max_diff = _mm256_max_epi32(diff, ones);
        
        // Check if all ranges have converged
        __m256i cmp_done = _mm256_cmpgt_epi32(max_diff, ones);
        if (_mm256_testz_si256(cmp_done, cmp_done)) break;
        
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        
        // Gather starts[mid] for each lane
        __m256i starts_mid = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] > probe ? hi = mid : lo = mid
        __m256i cmp = _mm256_cmpgt_epi32(starts_mid, probe_vec);
        
        hi = _mm256_blendv_epi8(hi, mid, cmp);
        lo = _mm256_blendv_epi8(mid, lo, cmp);
      }
      
      // lo now contains the index (upper_bound - 1)
      __m256i idx_vec = lo;
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx_vec, 4);
      
      // Load weights
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Multiply and accumulate (need 64-bit results)
      // Split into low and high 4 elements
      __m128i payload_lo = _mm256_castsi256_si128(payload_vec);
      __m128i payload_hi = _mm256_extracti128_si256(payload_vec, 1);
      __m128i weight_lo = _mm256_castsi256_si128(weight_vec);
      __m128i weight_hi = _mm256_extracti128_si256(weight_vec, 1);
      
      // 32x32 -> 64 bit multiply
      __m256i prod_lo = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_lo), _mm256_cvtepu32_epi64(weight_lo));
      __m256i prod_hi = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_hi), _mm256_cvtepu32_epi64(weight_hi));
      
      // Also need odd elements
      __m128i payload_lo_odd = _mm_srli_epi64(payload_lo, 32);
      __m128i payload_hi_odd = _mm_srli_epi64(payload_hi, 32);
      __m128i weight_lo_odd = _mm_srli_epi64(weight_lo, 32);
      __m128i weight_hi_odd = _mm_srli_epi64(weight_hi, 32);
      
      __m256i prod_lo_odd = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_lo_odd), _mm256_cvtepu32_epi64(weight_lo_odd));
      __m256i prod_hi_odd = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_hi_odd), _mm256_cvtepu32_epi64(weight_hi_odd));
      
      // Horizontal sum all products
      __m256i sum1 = _mm256_add_epi64(prod_lo, prod_hi);
      __m256i sum2 = _mm256_add_epi64(prod_lo_odd, prod_hi_odd);
      __m256i sum3 = _mm256_add_epi64(sum1, sum2);
      
      __m128i sum_lo = _mm256_castsi256_si128(sum3);
      __m128i sum_hi = _mm256_extracti128_si256(sum3, 1);
      __m128i sum4 = _mm_add_epi64(sum_lo, sum_hi);
      sum4 = _mm_add_epi64(sum4, _mm_srli_si128(sum4, 8));
      
      sum += static_cast<uint64_t>(_mm_cvtsi128_si64(sum4));
    }
    
    // Scalar remainder with unrolling
    for (; i + 4 <= n_probes; i += 4) {
      uint32_t p0 = probes_ptr[i];
      uint32_t p1 = probes_ptr[i + 1];
      uint32_t p2 = probes_ptr[i + 2];
      uint32_t p3 = probes_ptr[i + 3];
      
      auto it0 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p0);
      auto it1 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p1);
      auto it2 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p2);
      auto it3 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p3);
      
      std::size_t idx0 = static_cast<std::size_t>(it0 - starts_ptr - 1);
      std::size_t idx1 = static_cast<std::size_t>(it1 - starts_ptr - 1);
      std::size_t idx2 = static_cast<std::size_t>(it2 - starts_ptr - 1);
      std::size_t idx3 = static_cast<std::size_t>(it3 - starts_ptr - 1);
      
      sum += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      sum += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      sum += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}