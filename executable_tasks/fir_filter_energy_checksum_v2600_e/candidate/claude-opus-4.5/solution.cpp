#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t fir_energy_checksum(
    const std::vector<int16_t>& signal,
    const std::vector<int16_t>& taps,
    int iters) {
  
  const std::size_t sig_size = signal.size();
  const std::size_t tap_size = taps.size();
  
  if (sig_size == 0 || tap_size == 0 || iters <= 0) {
    uint64_t total = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
      // empty output, total stays the same
    }
    return total;
  }
  
  const int16_t* sig_ptr = signal.data();
  const int16_t* tap_ptr = taps.data();
  
  std::vector<int64_t> output(sig_size);
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Phase 1: Partial overlap region (i < tap_size - 1)
    std::size_t partial_end = (tap_size - 1 < sig_size) ? (tap_size - 1) : sig_size;
    
    for (std::size_t i = 0; i < partial_end; ++i) {
      int64_t acc = 0;
      std::size_t j = 0;
      // Unroll by 4
      std::size_t limit = i + 1;
      std::size_t limit4 = limit & ~3ULL;
      for (; j < limit4; j += 4) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 1]) * static_cast<int64_t>(tap_ptr[j + 1]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 2]) * static_cast<int64_t>(tap_ptr[j + 2]);
        acc += static_cast<int64_t>(sig_ptr[i - j - 3]) * static_cast<int64_t>(tap_ptr[j + 3]);
      }
      for (; j <= i; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      output[i] = acc;
    }
    
    // Phase 2: Full kernel region (i >= tap_size - 1)
    // Here all taps are valid, no bounds checking needed
    for (std::size_t i = partial_end; i < sig_size; ++i) {
      int64_t acc = 0;
      const int16_t* sig_start = sig_ptr + i;
      
      std::size_t j = 0;
      
#ifdef __AVX2__
      // Process 16 elements at a time with AVX2
      __m256i sum_lo = _mm256_setzero_si256();
      __m256i sum_hi = _mm256_setzero_si256();
      
      std::size_t tap16 = tap_size & ~15ULL;
      for (; j < tap16; j += 16) {
        // Load 16 signal values (reversed order access)
        __m128i s0 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(sig_start - j - 7));
        __m128i s1 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(sig_start - j - 15));
        
        // Reverse the order within each 128-bit chunk
        s0 = _mm_shuffle_epi8(s0, _mm_set_epi8(1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14));
        s1 = _mm_shuffle_epi8(s1, _mm_set_epi8(1,0,3,2,5,4,7,6,9,8,11,10,13,12,15,14));
        
        __m256i sig_vec = _mm256_set_m128i(s1, s0);
        
        // Load 16 tap values
        __m256i tap_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(tap_ptr + j));
        
        // Multiply and accumulate (madd gives 32-bit results)
        __m256i prod = _mm256_madd_epi16(sig_vec, tap_vec);
        
        // Accumulate to 64-bit
        __m256i prod_lo = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(prod));
        __m256i prod_hi = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(prod, 1));
        
        sum_lo = _mm256_add_epi64(sum_lo, prod_lo);
        sum_hi = _mm256_add_epi64(sum_hi, prod_hi);
      }
      
      // Horizontal sum
      __m256i sum_all = _mm256_add_epi64(sum_lo, sum_hi);
      __m128i sum128 = _mm_add_epi64(_mm256_castsi256_si128(sum_all), 
                                      _mm256_extracti128_si256(sum_all, 1));
      sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
      acc = _mm_cvtsi128_si64(sum128);
#endif
      
      // Scalar cleanup
      for (; j < tap_size; ++j) {
        acc += static_cast<int64_t>(sig_start[-static_cast<std::ptrdiff_t>(j)]) * 
               static_cast<int64_t>(tap_ptr[j]);
      }
      
      output[i] = acc;
    }
    
    // Compute checksum
    total = 1469598103934665603ULL;
    for (std::size_t i = 0; i < sig_size; ++i) {
      int64_t acc = output[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  
  return total;
}