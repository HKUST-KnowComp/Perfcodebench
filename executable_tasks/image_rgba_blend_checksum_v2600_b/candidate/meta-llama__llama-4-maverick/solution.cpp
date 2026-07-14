#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void blend_pixels(uint8_t* out, const uint8_t* src, const uint8_t* dst, std::size_t pixels) {
  const __m128i zero = _mm_setzero_si128();
  const __m128i k255 = _mm_set1_epi16(255);
  const __m128i k127 = _mm_set1_epi16(127);
  const __m128i k1 = _mm_set1_epi16(1);
  for (std::size_t i = 0; i < pixels; i += 4) {
    __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i * 4));
    __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i * 4));
    __m128i alpha = _mm_shuffle_epi8(s, _mm_set_epi8(15, 15, 15, 15, 11, 11, 11, 11, 7, 7, 7, 7, 3, 3, 3, 3));
    __m128i alpha_expanded = _mm_unpacklo_epi8(alpha, zero);
    __m128i inv_alpha_expanded = _mm_sub_epi16(k255, alpha_expanded);
    __m128i s_expanded = _mm_unpacklo_epi8(s, zero);
    __m128i d_expanded = _mm_unpacklo_epi8(d, zero);
    __m128i prod_s = _mm_mullo_epi16(s_expanded, alpha_expanded);
    __m128i prod_d = _mm_mullo_epi16(d_expanded, inv_alpha_expanded);
    __m128i sum = _mm_add_epi16(_mm_add_epi16(prod_s, prod_d), k127);
    __m128i quotient = _mm_srli_epi16(_mm_add_epi16(sum, k1), 8);
    __m128i result_lo = _mm_packus_epi16(quotient, zero);
    
    alpha_expanded = _mm_unpackhi_epi8(alpha, zero);
    inv_alpha_expanded = _mm_sub_epi16(k255, alpha_expanded);
    s_expanded = _mm_unpackhi_epi8(s, zero);
    d_expanded = _mm_unpackhi_epi8(d, zero);
    prod_s = _mm_mullo_epi16(s_expanded, alpha_expanded);
    prod_d = _mm_mullo_epi16(d_expanded, inv_alpha_expanded);
    sum = _mm_add_epi16(_mm_add_epi16(prod_s, prod_d), k127);
    quotient = _mm_srli_epi16(_mm_add_epi16(sum, k1), 8);
    __m128i result_hi = _mm_packus_epi16(quotient, zero);
    __m128i result = _mm_or_si128(_mm_slli_si128(result_hi, 8), result_lo);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i * 4), result);
    out[i * 4 + 3] = 255;
    out[i * 4 + 7] = 255;
    out[i * 4 + 11] = 255;
    out[i * 4 + 15] = 255;
  }
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  const std::size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out(src.size());
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    blend_pixels(out.data(), src.data(), dst.data(), pixels);
    hash = checksum_bytes(out);
  }
  return hash;
}
