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
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t n_intervals = starts.size();
  const std::size_t n_probes = probes.size();
  
  uint64_t sum = 0;
  
#if defined(__AVX2__)
  const __m256i ones = _mm256_set1_epi32(1);
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 8 probes at a time
    for (; i + 8 <= n_probes; i += 8) {
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Vectorized binary search (upper_bound - 1)
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      while (true) {
        __m256i diff = _mm256_sub_epi32(hi, lo);
        __m256i max_diff = _mm256_set1_epi32(1);
        int done = _mm256_movemask_epi8(_mm256_cmpgt_epi32(max_diff, _mm256_sub_epi32(hi, lo)));
        if (done == -1) break;
        
        __m256i mid = _mm256_add_epi32(lo, _mm256_srli_epi32(diff, 1));
        __m256i mid_vals = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] <= probe (unsigned comparison via signed trick)
        // For unsigned: a <= b iff (a ^ 0x80000000) <= (b ^ 0x80000000) as signed
        __m256i bias = _mm256_set1_epi32(static_cast<int>(0x80000000u));
        __m256i mid_biased = _mm256_xor_si256(mid_vals, bias);
        __m256i probe_biased = _mm256_xor_si256(probe_vec, bias);
        
        // cmp_gt: mid_vals > probe (as unsigned)
        __m256i cmp_gt = _mm256_cmpgt_epi32(mid_biased, probe_biased);
        
        // If mid_vals > probe: hi = mid, else lo = mid + 1
        hi = _mm256_blendv_epi8(hi, mid, cmp_gt);
        lo = _mm256_blendv_epi8(_mm256_add_epi32(mid, ones), lo, cmp_gt);
      }
      
      // idx = lo - 1 (upper_bound result - 1)
      __m256i idx = _mm256_sub_epi32(lo, ones);
      
      // Gather payloads
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Multiply and accumulate as 64-bit
      // payload_vec and weight_vec are 32-bit, we need 64-bit products
      __m256i prod_lo = _mm256_mul_epu32(payload_vec, weight_vec);
      __m256i prod_hi = _mm256_mul_epu32(_mm256_srli_epi64(payload_vec, 32), _mm256_srli_epi64(weight_vec, 32));
      
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
    }
    
    // Horizontal sum of accumulators
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i lo128 = _mm256_castsi256_si128(total);
    __m128i hi128 = _mm256_extracti128_si256(total, 1);
    __m128i sum128 = _mm_add_epi64(lo128, hi128);
    sum = static_cast<uint64_t>(_mm_extract_epi64(sum128, 0)) + static_cast<uint64_t>(_mm_extract_epi64(sum128, 1));
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
#else
  // Scalar fallback
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
#endif
  
  return sum;
}
