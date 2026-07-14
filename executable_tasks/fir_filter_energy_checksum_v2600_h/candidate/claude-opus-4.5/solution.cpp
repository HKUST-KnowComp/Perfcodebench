#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

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
    return 1469598103934665603ULL;
  }
  
  std::vector<int64_t> output(sig_size);
  const int16_t* __restrict sig_ptr = signal.data();
  const int16_t* __restrict tap_ptr = taps.data();
  int64_t* __restrict out_ptr = output.data();
  
  uint64_t total = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    // Phase 1: partial overlap (i < tap_size - 1)
    std::size_t phase1_end = (tap_size - 1 < sig_size) ? (tap_size - 1) : sig_size;
    for (std::size_t i = 0; i < phase1_end; ++i) {
      int64_t acc = 0;
      for (std::size_t j = 0; j <= i; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      out_ptr[i] = acc;
    }
    
    // Phase 2: full overlap (i >= tap_size - 1)
#if defined(__AVX2__)
    for (std::size_t i = phase1_end; i < sig_size; ++i) {
      const int16_t* sig_start = sig_ptr + i - tap_size + 1;
      __m256i acc_vec = _mm256_setzero_si256();
      
      std::size_t j = 0;
      // Process 16 elements at a time
      for (; j + 15 < tap_size; j += 16) {
        // Load 16 signal values (in reverse order relative to taps)
        __m256i s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(sig_start + (tap_size - 1 - j - 15)));
        // Reverse the signal vector
        s = _mm256_permute2x128_si256(s, s, 0x01);
        s = _mm256_shufflelo_epi16(s, _MM_SHUFFLE(0,1,2,3));
        s = _mm256_shufflehi_epi16(s, _MM_SHUFFLE(0,1,2,3));
        __m256i s_lo = _mm256_unpacklo_epi64(s, s);
        __m256i s_hi = _mm256_unpackhi_epi64(s, s);
        s = _mm256_blend_epi32(_mm256_slli_si256(s_hi, 8), _mm256_srli_si256(s_lo, 8), 0xCC);
        s = _mm256_shuffle_epi32(s, _MM_SHUFFLE(1,0,3,2));
        
        // Load 16 tap values
        __m256i t = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(tap_ptr + j));
        
        // Multiply and accumulate using madd (pairs of 16-bit -> 32-bit)
        __m256i prod = _mm256_madd_epi16(s, t);
        acc_vec = _mm256_add_epi64(acc_vec, _mm256_add_epi64(
            _mm256_cvtepi32_epi64(_mm256_extracti128_si256(prod, 0)),
            _mm256_cvtepi32_epi64(_mm256_extracti128_si256(prod, 1))));
      }
      
      // Horizontal sum
      int64_t acc_arr[4];
      _mm256_storeu_si256(reinterpret_cast<__m256i*>(acc_arr), acc_vec);
      int64_t acc = acc_arr[0] + acc_arr[1] + acc_arr[2] + acc_arr[3];
      
      // Scalar cleanup
      for (; j < tap_size; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      out_ptr[i] = acc;
    }
#else
    for (std::size_t i = phase1_end; i < sig_size; ++i) {
      int64_t acc = 0;
      for (std::size_t j = 0; j < tap_size; ++j) {
        acc += static_cast<int64_t>(sig_ptr[i - j]) * static_cast<int64_t>(tap_ptr[j]);
      }
      out_ptr[i] = acc;
    }
#endif
    
    // Compute checksum
    total = 1469598103934665603ULL;
    for (std::size_t i = 0; i < sig_size; ++i) {
      int64_t acc = out_ptr[i];
      total ^= static_cast<uint64_t>(acc * acc + 3 * acc);
      total *= 1099511628211ULL;
    }
  }
  
  return total;
}