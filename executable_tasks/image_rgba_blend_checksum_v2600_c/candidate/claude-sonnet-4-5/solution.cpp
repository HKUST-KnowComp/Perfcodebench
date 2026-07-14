#include "interface.h"

#include <cstdint>
#include <vector>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SIMD 1
#else
#define HAS_SIMD 0
#endif

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t sa = static_cast<uint32_t>(s) * a;
  uint32_t da = static_cast<uint32_t>(d) * (255u - a);
  return static_cast<uint8_t>((sa + da + 128u) >> 8);
}

uint64_t checksum_bytes(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  for (; i + 7 < len; i += 8) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 1]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 2]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 3]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 4]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 5]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 6]);
    hash *= prime;
    hash ^= static_cast<uint64_t>(bytes[i + 7]);
    hash *= prime;
  }
  
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(bytes[i]);
    hash *= prime;
  }
  
  return hash;
}

#if HAS_SIMD && (defined(__SSE2__) || defined(__AVX2__))
void blend_simd(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t pixels) {
  size_t i = 0;
  
#ifdef __AVX2__
  const __m256i zero = _mm256_setzero_si256();
  const __m256i c255 = _mm256_set1_epi16(255);
  const __m256i c128 = _mm256_set1_epi16(128);
  const __m256i alpha_ff = _mm256_set1_epi32(0xFF000000);
  
  for (; i + 7 < pixels; i += 8) {
    __m256i src_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(src + i * 4));
    __m256i dst_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(dst + i * 4));
    
    __m256i alpha = _mm256_shuffle_epi8(src_vec, 
      _mm256_set_epi8(15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3,
                      15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3));
    
    __m256i src_lo = _mm256_unpacklo_epi8(src_vec, zero);
    __m256i src_hi = _mm256_unpackhi_epi8(src_vec, zero);
    __m256i dst_lo = _mm256_unpacklo_epi8(dst_vec, zero);
    __m256i dst_hi = _mm256_unpackhi_epi8(dst_vec, zero);
    __m256i alpha_lo = _mm256_unpacklo_epi8(alpha, zero);
    __m256i alpha_hi = _mm256_unpackhi_epi8(alpha, zero);
    
    __m256i inv_alpha_lo = _mm256_sub_epi16(c255, alpha_lo);
    __m256i inv_alpha_hi = _mm256_sub_epi16(c255, alpha_hi);
    
    __m256i blend_lo = _mm256_add_epi16(
      _mm256_mullo_epi16(src_lo, alpha_lo),
      _mm256_mullo_epi16(dst_lo, inv_alpha_lo)
    );
    __m256i blend_hi = _mm256_add_epi16(
      _mm256_mullo_epi16(src_hi, alpha_hi),
      _mm256_mullo_epi16(dst_hi, inv_alpha_hi)
    );
    
    blend_lo = _mm256_srli_epi16(_mm256_add_epi16(blend_lo, c128), 8);
    blend_hi = _mm256_srli_epi16(_mm256_add_epi16(blend_hi, c128), 8);
    
    __m256i result = _mm256_packus_epi16(blend_lo, blend_hi);
    result = _mm256_or_si256(result, alpha_ff);
    
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(out + i * 4), result);
  }
#endif
  
#ifdef __SSE2__
  const __m128i zero = _mm_setzero_si128();
  const __m128i c255 = _mm_set1_epi16(255);
  const __m128i c128 = _mm_set1_epi16(128);
  const __m128i alpha_ff = _mm_set1_epi32(0xFF000000);
  
  for (; i + 3 < pixels; i += 4) {
    __m128i src_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src + i * 4));
    __m128i dst_vec = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst + i * 4));
    
    __m128i alpha = _mm_shuffle_epi8(src_vec, 
      _mm_set_epi8(15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3));
    
    __m128i src_lo = _mm_unpacklo_epi8(src_vec, zero);
    __m128i src_hi = _mm_unpackhi_epi8(src_vec, zero);
    __m128i dst_lo = _mm_unpacklo_epi8(dst_vec, zero);
    __m128i dst_hi = _mm_unpackhi_epi8(dst_vec, zero);
    __m128i alpha_lo = _mm_unpacklo_epi8(alpha, zero);
    __m128i alpha_hi = _mm_unpackhi_epi8(alpha, zero);
    
    __m128i inv_alpha_lo = _mm_sub_epi16(c255, alpha_lo);
    __m128i inv_alpha_hi = _mm_sub_epi16(c255, alpha_hi);
    
    __m128i blend_lo = _mm_add_epi16(
      _mm_mullo_epi16(src_lo, alpha_lo),
      _mm_mullo_epi16(dst_lo, inv_alpha_lo)
    );
    __m128i blend_hi = _mm_add_epi16(
      _mm_mullo_epi16(src_hi, alpha_hi),
      _mm_mullo_epi16(dst_hi, inv_alpha_hi)
    );
    
    blend_lo = _mm_srli_epi16(_mm_add_epi16(blend_lo, c128), 8);
    blend_hi = _mm_srli_epi16(_mm_add_epi16(blend_hi, c128), 8);
    
    __m128i result = _mm_packus_epi16(blend_lo, blend_hi);
    result = _mm_or_si128(result, alpha_ff);
    
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + i * 4), result);
  }
#endif
  
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

void blend_scalar(const uint8_t* src, const uint8_t* dst, uint8_t* out, size_t pixels) {
  size_t i = 0;
  
  for (; i + 3 < pixels; i += 4) {
    size_t idx0 = i * 4;
    size_t idx1 = idx0 + 4;
    size_t idx2 = idx1 + 4;
    size_t idx3 = idx2 + 4;
    
    uint8_t a0 = src[idx0 + 3];
    out[idx0] = blend_byte(src[idx0], dst[idx0], a0);
    out[idx0 + 1] = blend_byte(src[idx0 + 1], dst[idx0 + 1], a0);
    out[idx0 + 2] = blend_byte(src[idx0 + 2], dst[idx0 + 2], a0);
    out[idx0 + 3] = 255;
    
    uint8_t a1 = src[idx1 + 3];
    out[idx1] = blend_byte(src[idx1], dst[idx1], a1);
    out[idx1 + 1] = blend_byte(src[idx1 + 1], dst[idx1 + 1], a1);
    out[idx1 + 2] = blend_byte(src[idx1 + 2], dst[idx1 + 2], a1);
    out[idx1 + 3] = 255;
    
    uint8_t a2 = src[idx2 + 3];
    out[idx2] = blend_byte(src[idx2], dst[idx2], a2);
    out[idx2 + 1] = blend_byte(src[idx2 + 1], dst[idx2 + 1], a2);
    out[idx2 + 2] = blend_byte(src[idx2 + 2], dst[idx2 + 2], a2);
    out[idx2 + 3] = 255;
    
    uint8_t a3 = src[idx3 + 3];
    out[idx3] = blend_byte(src[idx3], dst[idx3], a3);
    out[idx3 + 1] = blend_byte(src[idx3 + 1], dst[idx3 + 1], a3);
    out[idx3 + 2] = blend_byte(src[idx3 + 2], dst[idx3 + 2], a3);
    out[idx3 + 3] = 255;
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
  
  const uint8_t* src_ptr = src.data();
  const uint8_t* dst_ptr = dst.data();
  uint8_t* out_ptr = out.data();
  
  for (int iter = 0; iter < iters; ++iter) {
#if HAS_SIMD && (defined(__SSE2__) || defined(__AVX2__))
    blend_simd(src_ptr, dst_ptr, out_ptr, pixels);
#else
    blend_scalar(src_ptr, dst_ptr, out_ptr, pixels);
#endif
    hash = checksum_bytes(out_ptr, out.size());
  }
  
  return hash;
}
