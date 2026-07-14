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
  
  if (sig_size == 0 || iters <= 0) {
    uint64_t total = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
      // empty signal means no iterations of inner loop
    }
    return total;
  }
  
  // Pre-reverse taps for sequential access pattern
  std::vector<int16_t> rev_taps(tap_size);
  for (std::size_t j = 0; j < tap_size; ++j) {
    rev_taps[j] = taps[tap_size - 1 - j];
  }
  
  std::vector<int64_t> output(sig_size);
  uint64_t total = 0;
  
  const int16_t* sig_ptr = signal.data();
  const int16_t* rev_ptr = rev_taps.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    // Compute FIR filter output
    for (std::size_t i = 0; i < sig_size; ++i) {
      // Number of taps to use: min(i+1, tap_size)
      std::size_t num_taps = (i + 1 < tap_size) ? (i + 1) : tap_size;
      // Start index in reversed taps
      std::size_t rev_start = tap_size - num_taps;
      // Start index in signal
      std::size_t sig_start = i + 1 - num_taps;
      
      int64_t acc = 0;
      std::size_t k = 0;
      
#ifdef __AVX2__
      // Process 16 elements at a time using AVX2
      __m256i sum_lo = _mm256_setzero_si256();
      __m256i sum_hi = _mm256_setzero_si256();
      
      for (; k + 16 <= num_taps; k += 16) {
        __m256i s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(sig_ptr + sig_start + k));
        __m256i t = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(rev_ptr + rev_start + k));
        
        // Multiply and accumulate: need 32-bit products, then sum to 64-bit
        __m256i lo = _mm256_mullo_epi16(s, t);
        __m256i hi = _mm256_mulhi_epi16(s, t);
        
        // Unpack to 32-bit
        __m256i prod0 = _mm256_unpacklo_epi16(lo, hi);
        __m256i prod1 = _mm256_unpackhi_epi16(lo, hi);
        
        // Sign extend 32-bit to 64-bit and accumulate
        __m256i p0_lo = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(prod0));
        __m256i p0_hi = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(prod0, 1));
        __m256i p1_lo = _mm256_cvtepi32_epi64(_mm256_castsi256_si128(prod1));
        __m256i p1_hi = _mm256_cvtepi32_epi64(_mm256_extracti128_si256(prod1, 1));
        
        sum_lo = _mm256_add_epi64(sum_lo, p0_lo);
        sum_lo = _mm256_add_epi64(sum_lo, p0_hi);
        sum_hi = _mm256_add_epi64(sum_hi, p1_lo);
        sum_hi = _mm256_add_epi64(sum_hi, p1_hi);
      }
      
      // Horizontal sum
      __m256i sum_all = _mm256_add_epi64(sum_lo, sum_hi);
      __m128i sum128 = _mm_add_epi64(_mm256_castsi256_si128(sum_all), 
                                      _mm256_extracti128_si256(sum_all, 1));
      sum128 = _mm_add_epi64(sum128, _mm_srli_si128(sum128, 8));
      acc = _mm_cvtsi128_si64(sum128);
#endif
      
      // Scalar tail
      for (; k < num_taps; ++k) {
        acc += static_cast<int64_t>(sig_ptr[sig_start + k]) * 
               static_cast<int64_t>(rev_ptr[rev_start + k]);
      }
      
      output[i] = acc;
    }
    
    // Compute checksum
    total = 1469598103934665603ULL;
    const int64_t* out_ptr = output.data();
    std::size_t idx = 0;
    
    // Unroll by 4
    for (; idx + 4 <= sig_size; idx += 4) {
      int64_t a0 = out_ptr[idx];
      int64_t a1 = out_ptr[idx + 1];
      int64_t a2 = out_ptr[idx + 2];
      int64_t a3 = out_ptr[idx + 3];
      
      total ^= static_cast<uint64_t>(a0 * a0 + 3 * a0);
      total *= 1099511628211ULL;
      total ^= static_cast<uint64_t>(a1 * a1 + 3 * a1);
      total *= 1099511628211ULL;
      total ^= static_cast<uint64_t>(a2 * a2 + 3 * a2);
      total *= 1099511628211ULL;
      total ^= static_cast<uint64_t>(a3 * a3 + 3 * a3);
      total *= 1099511628211ULL;
    }
    
    for (; idx < sig_size; ++idx) {
      int64_t acc = out_ptr[idx];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  
  return total;
}