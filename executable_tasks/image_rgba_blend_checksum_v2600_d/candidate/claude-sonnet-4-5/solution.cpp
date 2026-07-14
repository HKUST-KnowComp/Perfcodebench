#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE2
#if defined(__AVX2__)
#define HAS_AVX2
#endif
#endif

namespace {

inline uint8_t blend_byte_scalar(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t tmp = static_cast<uint32_t>(s) * a + static_cast<uint32_t>(d) * (255u - a) + 127u;
  return static_cast<uint8_t>((tmp + (tmp >> 8)) >> 8);
}

#ifdef HAS_SSE2
void blend_pixels_sse2(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t pixels) {
  const __m128i zero = _mm_setzero_si128();
  const __m128i c255 = _mm_set1_epi16(255);
  const __m128i c128 = _mm_set1_epi16(128);
  const __m128i alpha_mask = _mm_set1_epi32(0xFF000000);
  
  size_t i = 0;
  for (; i + 4 <= pixels; i += 4) {
    __m128i src_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i * 4));
    __m128i dst_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i * 4));
    
    __m128i alpha = _mm_and_si128(src_pixels, alpha_mask);
    alpha = _mm_or_si128(alpha, _mm_srli_epi32(alpha, 8));
    alpha = _mm_or_si128(alpha, _mm_srli_epi32(alpha, 16));
    
    __m128i src_lo = _mm_unpacklo_epi8(src_pixels, zero);
    __m128i src_hi = _mm_unpackhi_epi8(src_pixels, zero);
    __m128i dst_lo = _mm_unpacklo_epi8(dst_pixels, zero);
    __m128i dst_hi = _mm_unpackhi_epi8(dst_pixels, zero);
    __m128i alpha_lo = _mm_unpacklo_epi8(alpha, zero);
    __m128i alpha_hi = _mm_unpackhi_epi8(alpha, zero);
    
    __m128i inv_alpha_lo = _mm_sub_epi16(c255, alpha_lo);
    __m128i inv_alpha_hi = _mm_sub_epi16(c255, alpha_hi);
    
    __m128i blend_lo = _mm_add_epi16(_mm_mullo_epi16(src_lo, alpha_lo), _mm_mullo_epi16(dst_lo, inv_alpha_lo));
    __m128i blend_hi = _mm_add_epi16(_mm_mullo_epi16(src_hi, alpha_hi), _mm_mullo_epi16(dst_hi, inv_alpha_hi));
    
    blend_lo = _mm_add_epi16(blend_lo, c128);
    blend_hi = _mm_add_epi16(blend_hi, c128);
    
    blend_lo = _mm_srli_epi16(_mm_add_epi16(blend_lo, _mm_srli_epi16(blend_lo, 8)), 8);
    blend_hi = _mm_srli_epi16(_mm_add_epi16(blend_hi, _mm_srli_epi16(blend_hi, 8)), 8);
    
    __m128i result = _mm_packus_epi16(blend_lo, blend_hi);
    result = _mm_or_si128(_mm_andnot_si128(alpha_mask, result), alpha_mask);
    
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i * 4), result);
  }
  
  for (; i < pixels; ++i) {
    const size_t idx = i * 4;
    const uint8_t a = src[idx + 3];
    out[idx] = blend_byte_scalar(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte_scalar(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte_scalar(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#endif

void blend_pixels_scalar(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t pixels) {
  for (size_t i = 0; i < pixels; ++i) {
    const size_t idx = i * 4;
    const uint8_t a = src[idx + 3];
    out[idx] = blend_byte_scalar(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte_scalar(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte_scalar(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}

uint64_t checksum_bytes_fast(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  for (; i + 8 <= len; i += 8) {
    hash ^= bytes[i];
    hash *= prime;
    hash ^= bytes[i + 1];
    hash *= prime;
    hash ^= bytes[i + 2];
    hash *= prime;
    hash ^= bytes[i + 3];
    hash *= prime;
    hash ^= bytes[i + 4];
    hash *= prime;
    hash ^= bytes[i + 5];
    hash *= prime;
    hash ^= bytes[i + 6];
    hash *= prime;
    hash ^= bytes[i + 7];
    hash *= prime;
  }
  
  for (; i < len; ++i) {
    hash ^= bytes[i];
    hash *= prime;
  }
  
  return hash;
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
  
  const size_t pixels = src.size() / 4;
  const size_t out_size = pixels * 4;
  
  std::vector<uint8_t> out(out_size);
  uint64_t hash = 0;
  
  const uint8_t* src_ptr = src.data();
  const uint8_t* dst_ptr = dst.data();
  uint8_t* out_ptr = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
#ifdef HAS_SSE2
    blend_pixels_sse2(src_ptr, dst_ptr, out_ptr, pixels);
#else
    blend_pixels_scalar(src_ptr, dst_ptr, out_ptr, pixels);
#endif
    hash = checksum_bytes_fast(out_ptr, out_size);
  }
  
  return hash;
}
