#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  
  // Unroll by 8
  for (; i + 7 < n; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
    hash ^= (uint64_t)data[i + 1];
    hash *= prime;
    hash ^= (uint64_t)data[i + 2];
    hash *= prime;
    hash ^= (uint64_t)data[i + 3];
    hash *= prime;
    hash ^= (uint64_t)data[i + 4];
    hash *= prime;
    hash ^= (uint64_t)data[i + 5];
    hash *= prime;
    hash ^= (uint64_t)data[i + 6];
    hash *= prime;
    hash ^= (uint64_t)data[i + 7];
    hash *= prime;
  }
  
  // Handle remainder
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  return hash;
}

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  uint64_t hash = 0;
  
  // Allocate once outside loop
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) return 0;
  
  for (int iter = 0; iter < iters; ++iter) {
#ifdef __SSE2__
    size_t i = 0;
    const __m128i vec_255 = _mm_set1_epi16(255);
    const __m128i vec_127 = _mm_set1_epi16(127);
    const __m128i vec_zero = _mm_setzero_si128();
    
    // Process 4 pixels at a time (16 bytes)
    for (; i + 3 < pixels; i += 4) {
      size_t idx = i * 4u;
      
      // Load 16 bytes (4 RGBA pixels)
      __m128i src_pixels = _mm_loadu_si128((const __m128i*)&src[idx]);
      __m128i dst_pixels = _mm_loadu_si128((const __m128i*)&dst[idx]);
      
      // Extract alpha channel and replicate
      __m128i alpha_bytes = _mm_shuffle_epi8(src_pixels, 
        _mm_set_epi8(15,15,15,15, 11,11,11,11, 7,7,7,7, 3,3,3,3));
      
      // Unpack to 16-bit
      __m128i src_lo = _mm_unpacklo_epi8(src_pixels, vec_zero);
      __m128i src_hi = _mm_unpackhi_epi8(src_pixels, vec_zero);
      __m128i dst_lo = _mm_unpacklo_epi8(dst_pixels, vec_zero);
      __m128i dst_hi = _mm_unpackhi_epi8(dst_pixels, vec_zero);
      __m128i alpha_lo = _mm_unpacklo_epi8(alpha_bytes, vec_zero);
      __m128i alpha_hi = _mm_unpackhi_epi8(alpha_bytes, vec_zero);
      
      // Compute (255 - alpha)
      __m128i inv_alpha_lo = _mm_sub_epi16(vec_255, alpha_lo);
      __m128i inv_alpha_hi = _mm_sub_epi16(vec_255, alpha_hi);
      
      // Blend: (s*a + d*(255-a) + 127) / 255
      __m128i blend_lo = _mm_mullo_epi16(src_lo, alpha_lo);
      blend_lo = _mm_add_epi16(blend_lo, _mm_mullo_epi16(dst_lo, inv_alpha_lo));
      blend_lo = _mm_add_epi16(blend_lo, vec_127);
      blend_lo = _mm_srli_epi16(_mm_mulhi_epu16(blend_lo, _mm_set1_epi16(0x8081)), 7);
      
      __m128i blend_hi = _mm_mullo_epi16(src_hi, alpha_hi);
      blend_hi = _mm_add_epi16(blend_hi, _mm_mullo_epi16(dst_hi, inv_alpha_hi));
      blend_hi = _mm_add_epi16(blend_hi, vec_127);
      blend_hi = _mm_srli_epi16(_mm_mulhi_epu16(blend_hi, _mm_set1_epi16(0x8081)), 7);
      
      // Pack back to 8-bit
      __m128i result = _mm_packus_epi16(blend_lo, blend_hi);
      
      // Set alpha to 255
      result = _mm_or_si128(result, _mm_set_epi8(0,0,0,0xFF, 0,0,0,0xFF, 0,0,0,0xFF, 0,0,0,0xFF));
      
      _mm_storeu_si128((__m128i*)&out[idx], result);
    }
    
    // Handle remaining pixels
    for (; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
#else
    // Scalar fallback
    for (size_t i = 0; i < pixels; ++i) {
      size_t idx = i * 4u;
      uint8_t a = src[idx + 3u];
      out[idx] = blend_byte(src[idx], dst[idx], a);
      out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
      out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
      out[idx + 3u] = 255u;
    }
#endif
    
    hash = checksum_bytes(out, pixels * 4u);
  }
  
  free(out);
  return hash;
}