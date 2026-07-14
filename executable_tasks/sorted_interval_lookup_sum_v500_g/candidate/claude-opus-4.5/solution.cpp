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
  
  const uint32_t* __restrict starts_ptr = starts.data();
  const uint32_t* __restrict payloads_ptr = payloads.data();
  const uint32_t* __restrict probes_ptr = probes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    std::size_t i = 0;
    
#if defined(__AVX2__)
    // Process 8 probes at a time using AVX2
    const std::size_t simd_end = (n_probes / 8) * 8;
    
    for (; i < simd_end; i += 8) {
      // Load 8 probes and weights
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Parallel binary search for 8 probes
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations (log2(n_intervals) iterations needed)
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i ones = _mm256_set1_epi32(1);
        __m256i all_one = _mm256_cmpeq_epi32(diff, ones);
        if (_mm256_movemask_epi8(all_one) == -1) break;
        
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        
        // Gather starts[mid] for each lane
        __m256i starts_mid = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] > probe ? hi = mid : lo = mid
        __m256i cmp = _mm256_cmpgt_epi32(starts_mid, probe_vec);
        hi = _mm256_blendv_epi8(hi, mid, cmp);
        lo = _mm256_blendv_epi8(mid, lo, cmp);
      }
      
      // lo now contains the indices (upper_bound - 1)
      // Gather payloads[lo]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), lo, 4);
      
      // Compute payload * weight for each lane and accumulate
      // Split into low and high 4 elements for 64-bit multiplication
      __m128i payload_lo = _mm256_castsi256_si128(payload_vec);
      __m128i payload_hi = _mm256_extracti128_si256(payload_vec, 1);
      __m128i weight_lo = _mm256_castsi256_si128(weight_vec);
      __m128i weight_hi = _mm256_extracti128_si256(weight_vec, 1);
      
      // 32x32->64 multiplication
      __m256i prod_lo = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_lo), _mm256_cvtepu32_epi64(weight_lo));
      __m256i prod_hi = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_hi), _mm256_cvtepu32_epi64(weight_hi));
      
      // Shuffle to get odd elements for multiplication
      __m128i payload_lo_odd = _mm_shuffle_epi32(payload_lo, _MM_SHUFFLE(3, 3, 1, 1));
      __m128i payload_hi_odd = _mm_shuffle_epi32(payload_hi, _MM_SHUFFLE(3, 3, 1, 1));
      __m128i weight_lo_odd = _mm_shuffle_epi32(weight_lo, _MM_SHUFFLE(3, 3, 1, 1));
      __m128i weight_hi_odd = _mm_shuffle_epi32(weight_hi, _MM_SHUFFLE(3, 3, 1, 1));
      
      __m256i prod_lo_odd = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_lo_odd), _mm256_cvtepu32_epi64(weight_lo_odd));
      __m256i prod_hi_odd = _mm256_mul_epu32(_mm256_cvtepu32_epi64(payload_hi_odd), _mm256_cvtepu32_epi64(weight_hi_odd));
      
      // Horizontal sum of all 8 products
      __m256i sum_vec = _mm256_add_epi64(prod_lo, prod_lo_odd);
      sum_vec = _mm256_add_epi64(sum_vec, prod_hi);
      sum_vec = _mm256_add_epi64(sum_vec, prod_hi_odd);
      
      __m128i sum_128 = _mm_add_epi64(_mm256_castsi256_si128(sum_vec), _mm256_extracti128_si256(sum_vec, 1));
      sum_128 = _mm_add_epi64(sum_128, _mm_shuffle_epi32(sum_128, _MM_SHUFFLE(1, 0, 3, 2)));
      
      sum += static_cast<uint64_t>(_mm_cvtsi128_si64(sum_128));
    }
#endif
    
    // Scalar fallback for remaining elements
    for (; i < n_probes; ++i) {
      uint32_t probe = probes_ptr[i];
      
      // Binary search: find largest index where starts[idx] <= probe
      std::size_t lo = 0, hi = n_intervals;
      while (hi - lo > 1) {
        std::size_t mid = (lo + hi) >> 1;
        if (starts_ptr[mid] > probe) {
          hi = mid;
        } else {
          lo = mid;
        }
      }
      
      sum += static_cast<uint64_t>(payloads_ptr[lo]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}