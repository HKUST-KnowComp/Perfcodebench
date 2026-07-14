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
    std::size_t i = 0;
    
    // Process 8 probes at a time using SIMD binary search
    for (; i + 8 <= n_probes; i += 8) {
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
        
        // Gather starts[mid] for all 8 lanes
        __m256i mid_vals = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] <= probe ? 1 : 0
        // We want upper_bound behavior: find first index where starts[idx] > probe
        // So we update lo when starts[mid] <= probe
        __m256i cmp = _mm256_cmpgt_epi32(probe_vec, mid_vals); // probe > mid_vals
        __m256i cmp_eq = _mm256_cmpeq_epi32(probe_vec, mid_vals); // probe == mid_vals
        __m256i le_mask = _mm256_or_si256(cmp, cmp_eq); // probe >= mid_vals means starts[mid] <= probe
        
        // For upper_bound: if starts[mid] <= probe, lo = mid + 1, else hi = mid
        __m256i mid_plus_1 = _mm256_add_epi32(mid, ones);
        lo = _mm256_blendv_epi8(lo, mid_plus_1, le_mask);
        hi = _mm256_blendv_epi8(mid, hi, le_mask);
      }
      
      // lo now contains upper_bound indices, we need idx = lo - 1
      __m256i idx = _mm256_sub_epi32(lo, _mm256_set1_epi32(1));
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Compute payload * weight and accumulate
      // Need to do 32x32->64 multiplication
      // Split into low and high 4 elements
      __m128i payload_lo = _mm256_castsi256_si128(payload_vec);
      __m128i payload_hi = _mm256_extracti128_si256(payload_vec, 1);
      __m128i weight_lo = _mm256_castsi256_si128(weight_vec);
      __m128i weight_hi = _mm256_extracti128_si256(weight_vec, 1);
      
      // 32x32->64 multiply using _mm_mul_epu32 (multiplies elements 0,2)
      __m128i prod0_lo = _mm_mul_epu32(payload_lo, weight_lo);
      __m128i prod1_lo = _mm_mul_epu32(_mm_srli_si128(payload_lo, 4), _mm_srli_si128(weight_lo, 4));
      __m128i prod0_hi = _mm_mul_epu32(payload_hi, weight_hi);
      __m128i prod1_hi = _mm_mul_epu32(_mm_srli_si128(payload_hi, 4), _mm_srli_si128(weight_hi, 4));
      
      // Horizontal sum of 64-bit products
      __m128i sum_lo = _mm_add_epi64(prod0_lo, prod1_lo);
      __m128i sum_hi = _mm_add_epi64(prod0_hi, prod1_hi);
      __m128i sum_all = _mm_add_epi64(sum_lo, sum_hi);
      
      // Final horizontal add
      sum += static_cast<uint64_t>(_mm_extract_epi64(sum_all, 0)) + 
             static_cast<uint64_t>(_mm_extract_epi64(sum_all, 1));
    }
    
    // Handle remaining probes with scalar code
    for (; i < n_probes; ++i) {
      const uint32_t probe = probes_ptr[i];
      
      // Binary search
      std::size_t lo = 0, hi = n_intervals;
      while (hi - lo > 1) {
        std::size_t mid = (lo + hi) >> 1;
        if (starts_ptr[mid] <= probe) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      // lo is now the index we want (upper_bound - 1)
      std::size_t idx = lo;
      if (starts_ptr[lo] > probe && lo > 0) idx = lo - 1;
      else if (starts_ptr[lo] > probe) idx = 0;
      
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#else
    // Scalar fallback with manual unrolling
    std::size_t i = 0;
    for (; i + 4 <= n_probes; i += 4) {
      uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
      
      const uint32_t p0 = probes_ptr[i];
      const uint32_t p1 = probes_ptr[i + 1];
      const uint32_t p2 = probes_ptr[i + 2];
      const uint32_t p3 = probes_ptr[i + 3];
      
      auto it0 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p0);
      auto it1 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p1);
      auto it2 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p2);
      auto it3 = std::upper_bound(starts_ptr, starts_ptr + n_intervals, p3);
      
      std::size_t idx0 = static_cast<std::size_t>(it0 - starts_ptr - 1);
      std::size_t idx1 = static_cast<std::size_t>(it1 - starts_ptr - 1);
      std::size_t idx2 = static_cast<std::size_t>(it2 - starts_ptr - 1);
      std::size_t idx3 = static_cast<std::size_t>(it3 - starts_ptr - 1);
      
      s0 = static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      s1 = static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      s2 = static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      s3 = static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
      
      sum += s0 + s1 + s2 + s3;
    }
    
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts_ptr, starts_ptr + n_intervals, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#endif
  }
  
  return sum;
}