#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64)
#include <immintrin.h>
#endif

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const std::size_t size = packed.size();
  const uint8_t* data = packed.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t local_sum = 0;
    
#if defined(__AVX2__)
    const std::size_t simd_end = (size / 16) * 16;
    const __m256i threshold_vec = _mm256_set1_epi8(threshold);
    const __m256i mask_low = _mm256_set1_epi8(0x0F);
    __m256i acc = _mm256_setzero_si256();
    
    for (std::size_t i = 0; i < simd_end; i += 16) {
      __m128i packed_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
      __m256i expanded = _mm256_cvtepu8_epi16(packed_data);
      
      __m256i low_nibbles = _mm256_and_si256(expanded, mask_low);
      __m256i high_nibbles = _mm256_srli_epi16(expanded, 4);
      high_nibbles = _mm256_and_si256(high_nibbles, mask_low);
      
      __m256i low_mask = _mm256_cmpgt_epi8(low_nibbles, threshold_vec);
      __m256i high_mask = _mm256_cmpgt_epi8(high_nibbles, threshold_vec);
      
      __m256i low_filtered = _mm256_and_si256(low_nibbles, low_mask);
      __m256i high_filtered = _mm256_and_si256(high_nibbles, high_mask);
      
      __m256i combined = _mm256_add_epi8(low_filtered, high_filtered);
      acc = _mm256_add_epi64(acc, _mm256_sad_epu8(combined, _mm256_setzero_si256()));
    }
    
    alignas(32) uint64_t acc_array[4];
    _mm256_store_si256(reinterpret_cast<__m256i*>(acc_array), acc);
    local_sum += acc_array[0] + acc_array[1] + acc_array[2] + acc_array[3];
    
    for (std::size_t i = simd_end; i < size; ++i) {
      uint8_t byte = data[i];
      uint8_t low = byte & 15u;
      uint8_t high = byte >> 4u;
      if (low > threshold) local_sum += low;
      if (high > threshold) local_sum += high;
    }
#elif defined(__SSE2__)
    const std::size_t simd_end = (size / 16) * 16;
    const __m128i threshold_vec = _mm_set1_epi8(threshold);
    const __m128i mask_low = _mm_set1_epi8(0x0F);
    __m128i acc = _mm_setzero_si128();
    
    for (std::size_t i = 0; i < simd_end; i += 16) {
      __m128i packed_data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + i));
      
      __m128i low_nibbles = _mm_and_si128(packed_data, mask_low);
      __m128i high_nibbles = _mm_and_si128(_mm_srli_epi16(packed_data, 4), mask_low);
      
      __m128i low_mask = _mm_cmpgt_epi8(low_nibbles, threshold_vec);
      __m128i high_mask = _mm_cmpgt_epi8(high_nibbles, threshold_vec);
      
      __m128i low_filtered = _mm_and_si128(low_nibbles, low_mask);
      __m128i high_filtered = _mm_and_si128(high_nibbles, high_mask);
      
      __m128i combined = _mm_add_epi8(low_filtered, high_filtered);
      acc = _mm_add_epi64(acc, _mm_sad_epu8(combined, _mm_setzero_si128()));
    }
    
    alignas(16) uint64_t acc_array[2];
    _mm_store_si128(reinterpret_cast<__m128i*>(acc_array), acc);
    local_sum += acc_array[0] + acc_array[1];
    
    for (std::size_t i = simd_end; i < size; ++i) {
      uint8_t byte = data[i];
      uint8_t low = byte & 15u;
      uint8_t high = byte >> 4u;
      if (low > threshold) local_sum += low;
      if (high > threshold) local_sum += high;
    }
#else
    std::size_t i = 0;
    const std::size_t unroll_end = (size / 4) * 4;
    
    for (; i < unroll_end; i += 4) {
      uint8_t b0 = data[i];
      uint8_t b1 = data[i + 1];
      uint8_t b2 = data[i + 2];
      uint8_t b3 = data[i + 3];
      
      uint8_t l0 = b0 & 15u, h0 = b0 >> 4u;
      uint8_t l1 = b1 & 15u, h1 = b1 >> 4u;
      uint8_t l2 = b2 & 15u, h2 = b2 >> 4u;
      uint8_t l3 = b3 & 15u, h3 = b3 >> 4u;
      
      if (l0 > threshold) local_sum += l0;
      if (h0 > threshold) local_sum += h0;
      if (l1 > threshold) local_sum += l1;
      if (h1 > threshold) local_sum += h1;
      if (l2 > threshold) local_sum += l2;
      if (h2 > threshold) local_sum += h2;
      if (l3 > threshold) local_sum += l3;
      if (h3 > threshold) local_sum += h3;
    }
    
    for (; i < size; ++i) {
      uint8_t byte = data[i];
      uint8_t low = byte & 15u;
      uint8_t high = byte >> 4u;
      if (low > threshold) local_sum += low;
      if (high > threshold) local_sum += high;
    }
#endif
    
    sum = local_sum;
  }
  
  return sum;
}