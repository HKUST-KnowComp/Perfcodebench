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
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
    // Process 8 probes at a time with AVX2
    const std::size_t vec_end = (n_probes / 8) * 8;
    
    for (; i < vec_end; i += 8) {
      // Prefetch future probes and weights
      if (i + 64 < n_probes) {
        _mm_prefetch(reinterpret_cast<const char*>(probes_ptr + i + 64), _MM_HINT_T0);
        _mm_prefetch(reinterpret_cast<const char*>(weights_ptr + i + 64), _MM_HINT_T0);
      }
      
      // Load 8 probes and weights
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Binary search for each probe (vectorized approach)
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i max_diff = _mm256_max_epi32(diff, _mm256_setzero_si256());
        
        // Check if all searches are done (diff <= 1)
        __m256i one = _mm256_set1_epi32(1);
        __m256i cmp = _mm256_cmpgt_epi32(max_diff, one);
        if (_mm256_testz_si256(cmp, cmp)) break;
        
        // mid = lo + (hi - lo) / 2
        __m256i half = _mm256_srli_epi32(diff, 1);
        __m256i mid = _mm256_add_epi32(lo, half);
        
        // Gather starts[mid]
        __m256i mid_vals = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: if starts[mid] <= probe, lo = mid + 1, else hi = mid
        __m256i le_mask = _mm256_or_si256(
            _mm256_cmpgt_epi32(probe_vec, mid_vals),
            _mm256_cmpeq_epi32(probe_vec, mid_vals)
        );
        
        __m256i mid_plus_one = _mm256_add_epi32(mid, one);
        lo = _mm256_blendv_epi8(lo, mid_plus_one, le_mask);
        hi = _mm256_blendv_epi8(mid, hi, le_mask);
      }
      
      // idx = lo - 1 (upper_bound result - 1)
      __m256i idx = _mm256_sub_epi32(lo, _mm256_set1_epi32(1));
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Compute payload * weight for each element and accumulate
      // Split into low and high 4 elements for 64-bit multiplication
      __m128i payload_lo = _mm256_castsi256_si128(payload_vec);
      __m128i payload_hi = _mm256_extracti128_si256(payload_vec, 1);
      __m128i weight_lo = _mm256_castsi256_si128(weight_vec);
      __m128i weight_hi = _mm256_extracti128_si256(weight_vec, 1);
      
      // Extend to 64-bit and multiply
      __m256i p_lo_64 = _mm256_cvtepu32_epi64(payload_lo);
      __m256i w_lo_64 = _mm256_cvtepu32_epi64(weight_lo);
      __m256i p_hi_64 = _mm256_cvtepu32_epi64(payload_hi);
      __m256i w_hi_64 = _mm256_cvtepu32_epi64(weight_hi);
      
      __m256i prod_lo = _mm256_mul_epu32(p_lo_64, w_lo_64);
      __m256i prod_hi = _mm256_mul_epu32(p_hi_64, w_hi_64);
      
      // Horizontal sum
      __m256i prod_sum = _mm256_add_epi64(prod_lo, prod_hi);
      __m128i sum_128 = _mm_add_epi64(_mm256_castsi256_si128(prod_sum), _mm256_extracti128_si256(prod_sum, 1));
      sum_128 = _mm_add_epi64(sum_128, _mm_srli_si128(sum_128, 8));
      sum += static_cast<uint64_t>(_mm_cvtsi128_si64(sum_128));
    }
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}