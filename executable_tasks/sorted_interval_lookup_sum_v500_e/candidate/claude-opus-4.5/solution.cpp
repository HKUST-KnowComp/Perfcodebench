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
    
    __m256i sum_lo = _mm256_setzero_si256();
    __m256i sum_hi = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 8 probes at a time using AVX2
    for (; i + 8 <= n_probes; i += 8) {
      __m256i probe_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(probes_ptr + i));
      __m256i weight_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_ptr + i));
      
      // Binary search for each probe - vectorized
      __m256i lo = _mm256_setzero_si256();
      __m256i hi = _mm256_set1_epi32(static_cast<int>(n_intervals));
      
      // Binary search iterations (log2 of max intervals)
      for (int step = 0; step < 32; ++step) {
        __m256i mid = _mm256_srli_epi32(_mm256_add_epi32(lo, hi), 1);
        
        // Gather starts[mid]
        __m256i starts_mid = _mm256_i32gather_epi32(reinterpret_cast<const int*>(starts_ptr), mid, 4);
        
        // Compare: starts[mid] <= probe (unsigned comparison via signed trick)
        // For unsigned comparison: a <= b iff (a ^ 0x80000000) <= (b ^ 0x80000000) as signed
        __m256i bias = _mm256_set1_epi32(static_cast<int>(0x80000000u));
        __m256i starts_biased = _mm256_xor_si256(starts_mid, bias);
        __m256i probe_biased = _mm256_xor_si256(probe_vec, bias);
        __m256i cmp = _mm256_cmpgt_epi32(probe_biased, starts_biased); // probe > starts[mid]
        __m256i cmp_eq = _mm256_cmpeq_epi32(starts_mid, probe_vec);
        __m256i cmp_le = _mm256_or_si256(cmp, cmp_eq); // probe >= starts[mid], i.e., starts[mid] <= probe
        
        // If starts[mid] <= probe, lo = mid + 1, else hi = mid
        __m256i mid_plus_1 = _mm256_add_epi32(mid, _mm256_set1_epi32(1));
        lo = _mm256_blendv_epi8(lo, mid_plus_1, cmp_le);
        hi = _mm256_blendv_epi8(mid, hi, cmp_le);
      }
      
      // idx = lo - 1 (upper_bound result - 1)
      __m256i idx = _mm256_sub_epi32(lo, _mm256_set1_epi32(1));
      
      // Gather payloads[idx]
      __m256i payload_vec = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), idx, 4);
      
      // Multiply payload * weight (32x32 -> 64 bit)
      // Split into low and high parts
      __m256i payload_lo = _mm256_unpacklo_epi32(payload_vec, _mm256_setzero_si256());
      __m256i payload_hi = _mm256_unpackhi_epi32(payload_vec, _mm256_setzero_si256());
      __m256i weight_lo = _mm256_unpacklo_epi32(weight_vec, _mm256_setzero_si256());
      __m256i weight_hi = _mm256_unpackhi_epi32(weight_vec, _mm256_setzero_si256());
      
      __m256i prod_lo = _mm256_mul_epu32(payload_lo, weight_lo);
      __m256i prod_hi = _mm256_mul_epu32(payload_hi, weight_hi);
      
      sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
      sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
    }
    
    // Horizontal sum of vectors
    __m256i total = _mm256_add_epi64(sum_lo, sum_hi);
    __m128i total_lo = _mm256_castsi256_si128(total);
    __m128i total_hi = _mm256_extracti128_si256(total, 1);
    __m128i total_128 = _mm_add_epi64(total_lo, total_hi);
    sum = static_cast<uint64_t>(_mm_extract_epi64(total_128, 0)) + 
          static_cast<uint64_t>(_mm_extract_epi64(total_128, 1));
    
    // Handle remaining probes with scalar code
    for (; i < n_probes; ++i) {
      const auto it = std::upper_bound(starts.begin(), starts.end(), probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts.begin() - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
  }
  
  return sum;
}