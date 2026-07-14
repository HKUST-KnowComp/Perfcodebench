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
  
  const std::size_t n = probes.size();
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  const std::size_t starts_size = starts.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
#if defined(__AVX2__)
    __m256i vsum = _mm256_setzero_si256();
    
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      // Prefetch ahead
      if (i + 32 < n) {
        __builtin_prefetch(probes_ptr + i + 32, 0, 1);
        __builtin_prefetch(weights_ptr + i + 32, 0, 1);
      }
      
      // Process 4 probes at a time
      uint32_t probe0 = probes_ptr[i];
      uint32_t probe1 = probes_ptr[i + 1];
      uint32_t probe2 = probes_ptr[i + 2];
      uint32_t probe3 = probes_ptr[i + 3];
      
      // Binary searches
      const uint32_t* it0 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe0);
      const uint32_t* it1 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe1);
      const uint32_t* it2 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe2);
      const uint32_t* it3 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probe3);
      
      std::size_t idx0 = static_cast<std::size_t>(it0 - starts_ptr - 1);
      std::size_t idx1 = static_cast<std::size_t>(it1 - starts_ptr - 1);
      std::size_t idx2 = static_cast<std::size_t>(it2 - starts_ptr - 1);
      std::size_t idx3 = static_cast<std::size_t>(it3 - starts_ptr - 1);
      
      // Load payloads and weights
      __m128i pay = _mm_set_epi32(
        static_cast<int>(payloads_ptr[idx3]),
        static_cast<int>(payloads_ptr[idx2]),
        static_cast<int>(payloads_ptr[idx1]),
        static_cast<int>(payloads_ptr[idx0])
      );
      
      __m128i wgt = _mm_loadu_si128(reinterpret_cast<const __m128i*>(weights_ptr + i));
      
      // Extend to 64-bit and multiply
      __m256i pay64 = _mm256_cvtepu32_epi64(pay);
      __m256i wgt64 = _mm256_cvtepu32_epi64(wgt);
      
      // Multiply (using mullo for 64-bit)
      __m256i prod = _mm256_mul_epu32(pay64, wgt64);
      
      vsum = _mm256_add_epi64(vsum, prod);
    }
    
    // Horizontal sum of vsum
    __m128i vsum_lo = _mm256_castsi256_si128(vsum);
    __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
    __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
    vsum128 = _mm_add_epi64(vsum128, _mm_srli_si128(vsum128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(vsum128));
    
    // Handle remaining elements
    for (; i < n; ++i) {
      const uint32_t* it = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#else
    // Scalar fallback with unrolling
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      const uint32_t* it0 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i]);
      const uint32_t* it1 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i + 1]);
      const uint32_t* it2 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i + 2]);
      const uint32_t* it3 = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i + 3]);
      
      std::size_t idx0 = static_cast<std::size_t>(it0 - starts_ptr - 1);
      std::size_t idx1 = static_cast<std::size_t>(it1 - starts_ptr - 1);
      std::size_t idx2 = static_cast<std::size_t>(it2 - starts_ptr - 1);
      std::size_t idx3 = static_cast<std::size_t>(it3 - starts_ptr - 1);
      
      sum += static_cast<uint64_t>(payloads_ptr[idx0]) * static_cast<uint64_t>(weights_ptr[i]);
      sum += static_cast<uint64_t>(payloads_ptr[idx1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      sum += static_cast<uint64_t>(payloads_ptr[idx2]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      sum += static_cast<uint64_t>(payloads_ptr[idx3]) * static_cast<uint64_t>(weights_ptr[i + 3]);
    }
    
    for (; i < n; ++i) {
      const uint32_t* it = std::upper_bound(starts_ptr, starts_ptr + starts_size, probes_ptr[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_ptr - 1);
      sum += static_cast<uint64_t>(payloads_ptr[idx]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#endif
  }
  
  return sum;
}