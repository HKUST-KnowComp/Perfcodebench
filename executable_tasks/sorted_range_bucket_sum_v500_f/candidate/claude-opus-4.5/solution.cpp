#include "interface.h"

#include <cstdint>
#include <immintrin.h>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
  
  const size_t n_bounds = boundaries.size();
  const size_t n_probes = probes.size();
  
  if (n_probes == 0) return 0;
  
  const uint32_t* __restrict__ b_ptr = boundaries.data();
  const uint32_t* __restrict__ p_ptr = probes.data();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    size_t b_idx = 0;
    
    // Process in chunks for better cache utilization
    size_t i = 0;
    
    // Main loop with manual unrolling and prefetching
    const size_t PREFETCH_DIST = 64;
    
    // Use AVX2 to accumulate sums
    __m256i vsum = _mm256_setzero_si256();
    
    // Process 4 probes at a time when possible
    for (; i + 4 <= n_probes; i += 4) {
      // Prefetch future probes
      if (i + PREFETCH_DIST < n_probes) {
        _mm_prefetch(reinterpret_cast<const char*>(&p_ptr[i + PREFETCH_DIST]), _MM_HINT_T0);
      }
      
      uint32_t p0 = p_ptr[i];
      uint32_t p1 = p_ptr[i + 1];
      uint32_t p2 = p_ptr[i + 2];
      uint32_t p3 = p_ptr[i + 3];
      
      // Advance boundary index for p0
      while (b_idx < n_bounds && b_ptr[b_idx] <= p0) {
        ++b_idx;
      }
      uint64_t idx0 = b_idx;
      
      // Advance boundary index for p1 (probes are sorted, so we continue from current b_idx)
      while (b_idx < n_bounds && b_ptr[b_idx] <= p1) {
        ++b_idx;
      }
      uint64_t idx1 = b_idx;
      
      // Advance boundary index for p2
      while (b_idx < n_bounds && b_ptr[b_idx] <= p2) {
        ++b_idx;
      }
      uint64_t idx2 = b_idx;
      
      // Advance boundary index for p3
      while (b_idx < n_bounds && b_ptr[b_idx] <= p3) {
        ++b_idx;
      }
      uint64_t idx3 = b_idx;
      
      // Pack into vector and add
      __m256i vidx = _mm256_set_epi64x(idx3, idx2, idx1, idx0);
      vsum = _mm256_add_epi64(vsum, vidx);
    }
    
    // Horizontal sum of vsum
    __m128i vsum_lo = _mm256_castsi256_si128(vsum);
    __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
    __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
    sum = _mm_extract_epi64(vsum128, 0) + _mm_extract_epi64(vsum128, 1);
    
    // Handle remaining probes
    for (; i < n_probes; ++i) {
      uint32_t p = p_ptr[i];
      while (b_idx < n_bounds && b_ptr[b_idx] <= p) {
        ++b_idx;
      }
      sum += b_idx;
    }
  }
  
  return sum;
}