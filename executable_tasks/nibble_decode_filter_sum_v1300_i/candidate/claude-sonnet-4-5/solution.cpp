#include "interface.h"

#include <vector>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t size = packed.size();
  const uint8_t* data = packed.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    
#if defined(__AVX2__)
    // SIMD path: process 16 bytes (32 nibbles) at a time
    const std::size_t simd_end = size & ~15ULL;
    const __m256i threshold_vec = _mm256_set1_epi8(threshold);
    const __m256i mask_low = _mm256_set1_epi8(0x0F);
    __m256i sum_vec = _mm256_setzero_si256();
    
    for (std::size_t i = 0; i < simd_end; i += 16) {
      // Load 16 bytes
      __m128i packed_128 = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
      __m256i packed_256 = _mm256_cvtepu8_epi16(packed_128);
      
      // Extract low and high nibbles
      __m256i low_nibbles = _mm256_and_si256(packed_256, mask_low);
      __m256i high_nibbles = _mm256_srli_epi16(packed_256, 4);
      high_nibbles = _mm256_and_si256(high_nibbles, mask_low);
      
      // Compare with threshold (result is 0xFF if greater, 0x00 otherwise)
      __m256i low_mask = _mm256_cmpgt_epi16(low_nibbles, _mm256_cvtepu8_epi16(_mm256_castsi256_si128(threshold_vec)));
      __m256i high_mask = _mm256_cmpgt_epi16(high_nibbles, _mm256_cvtepu8_epi16(_mm256_castsi256_si128(threshold_vec)));
      
      // Mask and accumulate
      low_nibbles = _mm256_and_si256(low_nibbles, low_mask);
      high_nibbles = _mm256_and_si256(high_nibbles, high_mask);
      
      sum_vec = _mm256_add_epi16(sum_vec, low_nibbles);
      sum_vec = _mm256_add_epi16(sum_vec, high_nibbles);
    }
    
    // Horizontal sum of SIMD accumulator
    __m128i sum_low = _mm256_castsi256_si128(sum_vec);
    __m128i sum_high = _mm256_extracti128_si256(sum_vec, 1);
    __m128i sum_128 = _mm_add_epi16(sum_low, sum_high);
    
    // Further reduce to 64-bit
    __m128i sum_64 = _mm_sad_epu8(sum_128, _mm_setzero_si128());
    sum += _mm_extract_epi64(sum_64, 0) + _mm_extract_epi64(sum_64, 1);
    
    // Scalar remainder
    for (std::size_t i = simd_end; i < size; ++i) {
      uint8_t byte = data[i];
      uint8_t low = byte & 15u;
      uint8_t high = byte >> 4u;
      if (low > threshold) sum += low;
      if (high > threshold) sum += high;
    }
#else
    // Scalar path with unrolling
    std::size_t i = 0;
    const std::size_t unroll_end = size & ~3ULL;
    
    for (; i < unroll_end; i += 4) {
      uint8_t b0 = data[i];
      uint8_t b1 = data[i + 1];
      uint8_t b2 = data[i + 2];
      uint8_t b3 = data[i + 3];
      
      uint8_t low0 = b0 & 15u, high0 = b0 >> 4u;
      uint8_t low1 = b1 & 15u, high1 = b1 >> 4u;
      uint8_t low2 = b2 & 15u, high2 = b2 >> 4u;
      uint8_t low3 = b3 & 15u, high3 = b3 >> 4u;
      
      if (low0 > threshold) sum += low0;
      if (high0 > threshold) sum += high0;
      if (low1 > threshold) sum += low1;
      if (high1 > threshold) sum += high1;
      if (low2 > threshold) sum += low2;
      if (high2 > threshold) sum += high2;
      if (low3 > threshold) sum += low3;
      if (high3 > threshold) sum += high3;
    }
    
    for (; i < size; ++i) {
      uint8_t byte = data[i];
      uint8_t low = byte & 15u;
      uint8_t high = byte >> 4u;
      if (low > threshold) sum += low;
      if (high > threshold) sum += high;
    }
#endif
  }
  
  return sum;
}
