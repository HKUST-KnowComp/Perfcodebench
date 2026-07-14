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
    // Process in batches - first find all indices, then vectorize the multiply-accumulate
    constexpr std::size_t BATCH_SIZE = 256;
    alignas(32) uint32_t batch_payloads[BATCH_SIZE];
    alignas(32) uint32_t batch_weights[BATCH_SIZE];
    
    std::size_t i = 0;
    while (i + BATCH_SIZE <= n) {
      // Gather payloads for this batch using binary search
      for (std::size_t j = 0; j < BATCH_SIZE; ++j) {
        const uint32_t probe = probes_ptr[i + j];
        // Binary search for upper_bound
        std::size_t lo = 0, hi = starts_size;
        while (lo < hi) {
          std::size_t mid = lo + (hi - lo) / 2;
          if (starts_ptr[mid] <= probe) {
            lo = mid + 1;
          } else {
            hi = mid;
          }
        }
        batch_payloads[j] = payloads_ptr[lo - 1];
        batch_weights[j] = weights_ptr[i + j];
      }
      
      // Vectorized multiply-accumulate
      __m256i acc_lo = _mm256_setzero_si256();
      __m256i acc_hi = _mm256_setzero_si256();
      
      for (std::size_t j = 0; j < BATCH_SIZE; j += 8) {
        __m256i p = _mm256_load_si256(reinterpret_cast<const __m256i*>(&batch_payloads[j]));
        __m256i w = _mm256_load_si256(reinterpret_cast<const __m256i*>(&batch_weights[j]));
        
        // Multiply low 32 bits of each 64-bit lane
        __m256i prod_lo = _mm256_mul_epu32(p, w);
        // Multiply high 32 bits of each 64-bit lane
        __m256i p_hi = _mm256_srli_epi64(p, 32);
        __m256i w_hi = _mm256_srli_epi64(w, 32);
        __m256i prod_hi = _mm256_mul_epu32(p_hi, w_hi);
        
        acc_lo = _mm256_add_epi64(acc_lo, prod_lo);
        acc_hi = _mm256_add_epi64(acc_hi, prod_hi);
      }
      
      // Horizontal sum
      alignas(32) uint64_t tmp[4];
      _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), acc_lo);
      sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
      _mm256_store_si256(reinterpret_cast<__m256i*>(tmp), acc_hi);
      sum += tmp[0] + tmp[1] + tmp[2] + tmp[3];
      
      i += BATCH_SIZE;
    }
    
    // Handle remaining elements
    for (; i < n; ++i) {
      const uint32_t probe = probes_ptr[i];
      std::size_t lo = 0, hi = starts_size;
      while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (starts_ptr[mid] <= probe) {
          lo = mid + 1;
        } else {
          hi = mid;
        }
      }
      sum += static_cast<uint64_t>(payloads_ptr[lo - 1]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#else
    // Scalar fallback with manual unrolling
    std::size_t i = 0;
    for (; i + 4 <= n; i += 4) {
      uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
      
      const uint32_t p0 = probes_ptr[i];
      const uint32_t p1 = probes_ptr[i + 1];
      const uint32_t p2 = probes_ptr[i + 2];
      const uint32_t p3 = probes_ptr[i + 3];
      
      std::size_t lo0 = 0, hi0 = starts_size;
      while (lo0 < hi0) {
        std::size_t mid = lo0 + (hi0 - lo0) / 2;
        if (starts_ptr[mid] <= p0) lo0 = mid + 1;
        else hi0 = mid;
      }
      s0 = static_cast<uint64_t>(payloads_ptr[lo0 - 1]) * static_cast<uint64_t>(weights_ptr[i]);
      
      std::size_t lo1 = 0, hi1 = starts_size;
      while (lo1 < hi1) {
        std::size_t mid = lo1 + (hi1 - lo1) / 2;
        if (starts_ptr[mid] <= p1) lo1 = mid + 1;
        else hi1 = mid;
      }
      s1 = static_cast<uint64_t>(payloads_ptr[lo1 - 1]) * static_cast<uint64_t>(weights_ptr[i + 1]);
      
      std::size_t lo2 = 0, hi2 = starts_size;
      while (lo2 < hi2) {
        std::size_t mid = lo2 + (hi2 - lo2) / 2;
        if (starts_ptr[mid] <= p2) lo2 = mid + 1;
        else hi2 = mid;
      }
      s2 = static_cast<uint64_t>(payloads_ptr[lo2 - 1]) * static_cast<uint64_t>(weights_ptr[i + 2]);
      
      std::size_t lo3 = 0, hi3 = starts_size;
      while (lo3 < hi3) {
        std::size_t mid = lo3 + (hi3 - lo3) / 2;
        if (starts_ptr[mid] <= p3) lo3 = mid + 1;
        else hi3 = mid;
      }
      s3 = static_cast<uint64_t>(payloads_ptr[lo3 - 1]) * static_cast<uint64_t>(weights_ptr[i + 3]);
      
      sum += s0 + s1 + s2 + s3;
    }
    
    for (; i < n; ++i) {
      const uint32_t probe = probes_ptr[i];
      std::size_t lo = 0, hi = starts_size;
      while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;
        if (starts_ptr[mid] <= probe) lo = mid + 1;
        else hi = mid;
      }
      sum += static_cast<uint64_t>(payloads_ptr[lo - 1]) * static_cast<uint64_t>(weights_ptr[i]);
    }
#endif
  }
  
  return sum;
}