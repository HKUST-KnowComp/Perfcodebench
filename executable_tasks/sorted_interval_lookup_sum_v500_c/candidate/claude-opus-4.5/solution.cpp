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
  
  const uint32_t* starts_data = starts.data();
  const uint32_t* payloads_data = payloads.data();
  const uint32_t* probes_data = probes.data();
  const uint32_t* weights_data = weights.data();
  const std::size_t n_probes = probes.size();
  const std::size_t n_intervals = starts.size();
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
#if defined(__AVX2__)
    __m256i vsum0 = _mm256_setzero_si256();
    __m256i vsum1 = _mm256_setzero_si256();
    
    std::size_t i = 0;
    
    // Process 8 probes at a time
    for (; i + 8 <= n_probes; i += 8) {
      // Binary search for each probe
      uint32_t idx0, idx1, idx2, idx3, idx4, idx5, idx6, idx7;
      
      {
        uint32_t probe = probes_data[i];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx0 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 1];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx1 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 2];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx2 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 3];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx3 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 4];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx4 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 5];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx5 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 6];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx6 = static_cast<uint32_t>(it - starts_data - 1);
      }
      {
        uint32_t probe = probes_data[i + 7];
        const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probe);
        idx7 = static_cast<uint32_t>(it - starts_data - 1);
      }
      
      // Gather payloads and weights
      __m256i vidx = _mm256_setr_epi32(idx0, idx1, idx2, idx3, idx4, idx5, idx6, idx7);
      __m256i vpayloads = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_data), vidx, 4);
      __m256i vweights = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(weights_data + i));
      
      // Multiply and accumulate with 64-bit precision
      // Split into low and high 4 elements
      __m256i vpay_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vpayloads));
      __m256i vpay_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vpayloads, 1));
      __m256i vwgt_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vweights));
      __m256i vwgt_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vweights, 1));
      
      // 64-bit multiply
      __m256i vmul_lo = _mm256_mul_epu32(vpay_lo, vwgt_lo);
      __m256i vmul_hi = _mm256_mul_epu32(vpay_hi, vwgt_hi);
      
      vsum0 = _mm256_add_epi64(vsum0, vmul_lo);
      vsum1 = _mm256_add_epi64(vsum1, vmul_hi);
    }
    
    // Horizontal sum
    __m256i vsum = _mm256_add_epi64(vsum0, vsum1);
    __m128i vsum_lo = _mm256_castsi256_si128(vsum);
    __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
    __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
    vsum128 = _mm_add_epi64(vsum128, _mm_srli_si128(vsum128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(vsum128));
    
    // Handle remaining elements
    for (; i < n_probes; ++i) {
      const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_data - 1);
      sum += static_cast<uint64_t>(payloads_data[idx]) * static_cast<uint64_t>(weights_data[i]);
    }
#else
    // Scalar fallback with unrolling
    std::size_t i = 0;
    for (; i + 4 <= n_probes; i += 4) {
      const uint32_t* it0 = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i]);
      const uint32_t* it1 = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i + 1]);
      const uint32_t* it2 = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i + 2]);
      const uint32_t* it3 = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i + 3]);
      
      std::size_t idx0 = static_cast<std::size_t>(it0 - starts_data - 1);
      std::size_t idx1 = static_cast<std::size_t>(it1 - starts_data - 1);
      std::size_t idx2 = static_cast<std::size_t>(it2 - starts_data - 1);
      std::size_t idx3 = static_cast<std::size_t>(it3 - starts_data - 1);
      
      sum += static_cast<uint64_t>(payloads_data[idx0]) * static_cast<uint64_t>(weights_data[i]);
      sum += static_cast<uint64_t>(payloads_data[idx1]) * static_cast<uint64_t>(weights_data[i + 1]);
      sum += static_cast<uint64_t>(payloads_data[idx2]) * static_cast<uint64_t>(weights_data[i + 2]);
      sum += static_cast<uint64_t>(payloads_data[idx3]) * static_cast<uint64_t>(weights_data[i + 3]);
    }
    
    for (; i < n_probes; ++i) {
      const uint32_t* it = std::upper_bound(starts_data, starts_data + n_intervals, probes_data[i]);
      const std::size_t idx = static_cast<std::size_t>(it - starts_data - 1);
      sum += static_cast<uint64_t>(payloads_data[idx]) * static_cast<uint64_t>(weights_data[i]);
    }
#endif
  }
  
  return sum;
}