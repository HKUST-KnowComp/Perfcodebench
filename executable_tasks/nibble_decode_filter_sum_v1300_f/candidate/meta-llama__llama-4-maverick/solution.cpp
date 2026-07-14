#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t nibble_filter_sum(const std::vector<uint8_t>& packed, uint8_t threshold, int iters) {
  uint64_t sum = 0;
  const __m128i threshold_vec = _mm_set1_epi8(threshold);
  const __m128i zero = _mm_setzero_si128();
  for (int iter = 0; iter < iters; ++iter) {
    __m128i sum_vec = _mm_setzero_si128();
    for (std::size_t i = 0; i < packed.size(); i += 16) {
      __m128i bytes = _mm_loadu_si128(reinterpret_cast<const __m128i*>(packed.data() + i));
      __m128i low_nibbles = _mm_and_si128(bytes, _mm_set1_epi8(0x0F));
      __m128i high_nibbles = _mm_and_si128(_mm_srli_epi16(bytes, 4), _mm_set1_epi8(0x0F));
      __m128i low_cmp = _mm_cmpgt_epi8(low_nibbles, threshold_vec);
      __m128i high_cmp = _mm_cmpgt_epi8(high_nibbles, threshold_vec);
      __m128i low_sum = _mm_and_si128(low_cmp, low_nibbles);
      __m128i high_sum = _mm_and_si128(high_cmp, high_nibbles);
      sum_vec = _mm_add_epi64(sum_vec, _mm_sad_epu8(low_sum, zero));
      sum_vec = _mm_add_epi64(sum_vec, _mm_sad_epu8(high_sum, zero));
    }
    sum = 0;
    uint64_t sum_buffer[2];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(sum_buffer), sum_vec);
    sum = sum_buffer[0] + sum_buffer[1];
  }
  return sum;
}