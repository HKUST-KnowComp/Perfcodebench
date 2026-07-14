#include "interface.h"

#include <cstdint>
#include <vector>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __AVX2__
#include <immintrin.h>
#endif

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
}

#ifdef __AVX2__
void blend_pixels_avx2(const uint8_t* __restrict__ src,
                       const uint8_t* __restrict__ dst,
                       uint8_t* __restrict__ out,
                       size_t pixels) {
  const __m256i zero = _mm256_setzero_si256();
  const __m256i round = _mm256_set1_epi16(127);
  const __m256i c255 = _mm256_set1_epi16(255);
  const __m256i alpha_mask = _mm256_set1_epi32(0xFF000000);
  
  size_t i = 0;
  for (; i + 8 <= pixels; i += 8) {
    __m256i src_pixels = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i * 4));
    __m256i dst_pixels = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(dst + i * 4));
    
    __m256i alpha = _mm256_and_si256(src_pixels, alpha_mask);
    alpha = _mm256_srli_epi32(alpha, 24);
    __m256i alpha_broadcast = _mm256_shuffle_epi8(alpha,
      _mm256_setr_epi8(0,0,0,0, 4,4,4,4, 8,8,8,8, 12,12,12,12,
                       0,0,0,0, 4,4,4,4, 8,8,8,8, 12,12,12,12));
    
    __m256i src_lo = _mm256_unpacklo_epi8(src_pixels, zero);
    __m256i src_hi = _mm256_unpackhi_epi8(src_pixels, zero);
    __m256i dst_lo = _mm256_unpacklo_epi8(dst_pixels, zero);
    __m256i dst_hi = _mm256_unpackhi_epi8(dst_pixels, zero);
    __m256i alpha_lo = _mm256_unpacklo_epi8(alpha_broadcast, zero);
    __m256i alpha_hi = _mm256_unpackhi_epi8(alpha_broadcast, zero);
    
    __m256i inv_alpha_lo = _mm256_sub_epi16(c255, alpha_lo);
    __m256i inv_alpha_hi = _mm256_sub_epi16(c255, alpha_hi);
    
    __m256i blend_lo = _mm256_mullo_epi16(src_lo, alpha_lo);
    blend_lo = _mm256_add_epi16(blend_lo, _mm256_mullo_epi16(dst_lo, inv_alpha_lo));
    blend_lo = _mm256_add_epi16(blend_lo, round);
    blend_lo = _mm256_srli_epi16(blend_lo, 8);
    
    __m256i blend_hi = _mm256_mullo_epi16(src_hi, alpha_hi);
    blend_hi = _mm256_add_epi16(blend_hi, _mm256_mullo_epi16(dst_hi, inv_alpha_hi));
    blend_hi = _mm256_add_epi16(blend_hi, round);
    blend_hi = _mm256_srli_epi16(blend_hi, 8);
    
    __m256i result = _mm256_packus_epi16(blend_lo, blend_hi);
    result = _mm256_or_si256(result, alpha_mask);
    
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i * 4), result);
  }
  
  for (; i < pixels; ++i) {
    const size_t idx = i * 4;
    const uint8_t a = src[idx + 3];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#endif

#ifdef __SSE2__
void blend_pixels_sse2(const uint8_t* __restrict__ src,
                       const uint8_t* __restrict__ dst,
                       uint8_t* __restrict__ out,
                       size_t pixels) {
  const __m128i zero = _mm_setzero_si128();
  const __m128i round = _mm_set1_epi16(127);
  const __m128i c255 = _mm_set1_epi16(255);
  const __m128i alpha_mask = _mm_set1_epi32(0xFF000000);
  
  size_t i = 0;
  for (; i + 4 <= pixels; i += 4) {
    __m128i src_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i * 4));
    __m128i dst_pixels = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i * 4));
    
    __m128i alpha = _mm_and_si128(src_pixels, alpha_mask);
    alpha = _mm_srli_epi32(alpha, 24);
    __m128i alpha_broadcast = _mm_shuffle_epi8(alpha,
      _mm_setr_epi8(0,0,0,0, 4,4,4,4, 8,8,8,8, 12,12,12,12));
    
    __m128i src_lo = _mm_unpacklo_epi8(src_pixels, zero);
    __m128i src_hi = _mm_unpackhi_epi8(src_pixels, zero);
    __m128i dst_lo = _mm_unpacklo_epi8(dst_pixels, zero);
    __m128i dst_hi = _mm_unpackhi_epi8(dst_pixels, zero);
    __m128i alpha_lo = _mm_unpacklo_epi8(alpha_broadcast, zero);
    __m128i alpha_hi = _mm_unpackhi_epi8(alpha_broadcast, zero);
    
    __m128i inv_alpha_lo = _mm_sub_epi16(c255, alpha_lo);
    __m128i inv_alpha_hi = _mm_sub_epi16(c255, alpha_hi);
    
    __m128i blend_lo = _mm_mullo_epi16(src_lo, alpha_lo);
    blend_lo = _mm_add_epi16(blend_lo, _mm_mullo_epi16(dst_lo, inv_alpha_lo));
    blend_lo = _mm_add_epi16(blend_lo, round);
    blend_lo = _mm_srli_epi16(blend_lo, 8);
    
    __m128i blend_hi = _mm_mullo_epi16(src_hi, alpha_hi);
    blend_hi = _mm_add_epi16(blend_hi, _mm_mullo_epi16(dst_hi, inv_alpha_hi));
    blend_hi = _mm_add_epi16(blend_hi, round);
    blend_hi = _mm_srli_epi16(blend_hi, 8);
    
    __m128i result = _mm_packus_epi16(blend_lo, blend_hi);
    result = _mm_or_si128(result, alpha_mask);
    
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i * 4), result);
  }
  
  for (; i < pixels; ++i) {
    const size_t idx = i * 4;
    const uint8_t a = src[idx + 3];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#endif

void blend_pixels_scalar(const uint8_t* __restrict__ src,
                         const uint8_t* __restrict__ dst,
                         uint8_t* __restrict__ out,
                         size_t pixels) {
  size_t i = 0;
  for (; i + 4 <= pixels; i += 4) {
    for (int j = 0; j < 4; ++j) {
      const size_t idx = (i + j) * 4;
      const uint8_t a = src[idx + 3];
      const uint32_t inv_a = 255u - a;
      out[idx] = static_cast<uint8_t>((src[idx] * a + dst[idx] * inv_a + 127u) / 255u);
      out[idx + 1] = static_cast<uint8_t>((src[idx + 1] * a + dst[idx + 1] * inv_a + 127u) / 255u);
      out[idx + 2] = static_cast<uint8_t>((src[idx + 2] * a + dst[idx + 2] * inv_a + 127u) / 255u);
      out[idx + 3] = 255;
    }
  }
  
  for (; i < pixels; ++i) {
    const size_t idx = i * 4;
    const uint8_t a = src[idx + 3];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}

uint64_t checksum_bytes_fast(const uint8_t* __restrict__ bytes, size_t size) {
  uint64_t hash0 = 1469598103934665603ULL;
  uint64_t hash1 = 1469598103934665603ULL;
  uint64_t hash2 = 1469598103934665603ULL;
  uint64_t hash3 = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  for (; i + 4 <= size; i += 4) {
    hash0 ^= bytes[i];
    hash0 *= prime;
    hash1 ^= bytes[i + 1];
    hash1 *= prime;
    hash2 ^= bytes[i + 2];
    hash2 *= prime;
    hash3 ^= bytes[i + 3];
    hash3 *= prime;
  }
  
  uint64_t hash = hash0 ^ hash1 ^ hash2 ^ hash3;
  for (; i < size; ++i) {
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
  const size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out(src.size());
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
#ifdef __AVX2__
    blend_pixels_avx2(src.data(), dst.data(), out.data(), pixels);
#elif defined(__SSE2__)
    blend_pixels_sse2(src.data(), dst.data(), out.data(), pixels);
#else
    blend_pixels_scalar(src.data(), dst.data(), out.data(), pixels);
#endif
    hash = checksum_bytes_fast(out.data(), out.size());
  }
  
  return hash;
}
