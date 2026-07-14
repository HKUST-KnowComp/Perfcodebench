#include "interface.h"

#include <algorithm>
#include <cstddef>

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
  
  const std::size_t n_probes = probes.size();
  const std::size_t n_intervals = starts.size();
  
  if (n_probes == 0 || n_intervals == 0) {
    return 0;
  }
  
  const uint32_t* starts_ptr = starts.data();
  const uint32_t* payloads_ptr = payloads.data();
  const uint32_t* probes_ptr = probes.data();
  const uint32_t* weights_ptr = weights.data();
  
  // Precompute indices for all probes (since probes are sorted, we can use galloping search)
  std::vector<uint32_t> indices(n_probes);
  
  uint64_t sum = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
    // Compute indices using galloping/exponential search since probes are sorted
    std::size_t hint = 0;
    for (std::size_t i = 0; i < n_probes; ++i) {
      uint32_t probe = probes_ptr[i];
      
      // Galloping search starting from hint
      std::size_t lo = hint;
      std::size_t hi = n_intervals;
      
      // Exponential search forward
      std::size_t step = 1;
      while (lo + step < hi && starts_ptr[lo + step] <= probe) {
        lo += step;
        step *= 2;
      }
      hi = std::min(lo + step, hi);
      
      // Binary search in [lo, hi)
      while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (starts_ptr[mid] <= probe) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      
      indices[i] = static_cast<uint32_t>(lo - 1);
      hint = lo > 0 ? lo - 1 : 0;
    }
    
    // Vectorized accumulation
#if defined(__AVX2__)
    __m256i vsum0 = _mm256_setzero_si256();
    __m256i vsum1 = _mm256_setzero_si256();
    
    std::size_t i = 0;
    const std::size_t vec_end = (n_probes / 8) * 8;
    
    for (; i < vec_end; i += 8) {
      // Gather payloads using indices
      __m256i vidx = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&indices[i]));
      __m256i vpayload = _mm256_i32gather_epi32(reinterpret_cast<const int*>(payloads_ptr), vidx, 4);
      __m256i vweight = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&weights_ptr[i]));
      
      // Multiply and accumulate as 64-bit
      // Split into low and high 4 elements
      __m256i vpayload_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vpayload));
      __m256i vpayload_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vpayload, 1));
      __m256i vweight_lo = _mm256_cvtepu32_epi64(_mm256_castsi256_si128(vweight));
      __m256i vweight_hi = _mm256_cvtepu32_epi64(_mm256_extracti128_si256(vweight, 1));
      
      // 64-bit multiply
      __m256i vprod_lo = _mm256_mul_epu32(vpayload_lo, vweight_lo);
      __m256i vprod_hi = _mm256_mul_epu32(vpayload_hi, vweight_hi);
      
      vsum0 = _mm256_add_epi64(vsum0, vprod_lo);
      vsum1 = _mm256_add_epi64(vsum1, vprod_hi);
    }
    
    // Horizontal sum
    __m256i vsum = _mm256_add_epi64(vsum0, vsum1);
    __m128i vsum_lo = _mm256_castsi256_si128(vsum);
    __m128i vsum_hi = _mm256_extracti128_si256(vsum, 1);
    __m128i vsum128 = _mm_add_epi64(vsum_lo, vsum_hi);
    vsum128 = _mm_add_epi64(vsum128, _mm_srli_si128(vsum128, 8));
    sum = static_cast<uint64_t>(_mm_cvtsi128_si64(vsum128));
    
    // Scalar cleanup
    for (; i < n_probes; ++i) {
      sum += static_cast<uint64_t>(payloads_ptr[indices[i]]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#else
    // Scalar fallback with unrolling
    std::size_t i = 0;
    const std::size_t unroll_end = (n_probes / 4) * 4;
    
    for (; i < unroll_end; i += 4) {
      sum += static_cast<uint64_t>(payloads_ptr[indices[i]]) * static_cast<uint64_t>(weights_ptr[i]);
      sum += static_cast<uint64_t>(payloads_ptr[indices[i+1]]) * static_cast<uint64_t>(weights_ptr[i+1]);
      sum += static_cast<uint64_t>(payloads_ptr[indices[i+2]]) * static_cast<uint64_t>(weights_ptr[i+2]);
      sum += static_cast<uint64_t>(payloads_ptr[indices[i+3]]) * static_cast<uint64_t>(weights_ptr[i+3]);
    }
    
    for (; i < n_probes; ++i) {
      sum += static_cast<uint64_t>(payloads_ptr[indices[i]]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#endif
  }
  
  return sum;
}
