#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <emmintrin.h>
#define USE_SSE2
#elif defined(__ARM_NEON) || defined(__aarch64__)
#include <arm_neon.h>
#define USE_NEON
#endif

static inline uint8_t blend_byte_fast(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t tmp = (uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 128u;
  return (uint8_t)((tmp + (tmp >> 8)) >> 8);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  size_t i = 0;
  
  for (; i + 7 < n; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
    hash ^= (uint64_t)data[i+1];
    hash *= prime;
    hash ^= (uint64_t)data[i+2];
    hash *= prime;
    hash ^= (uint64_t)data[i+3];
    hash *= prime;
    hash ^= (uint64_t)data[i+4];
    hash *= prime;
    hash ^= (uint64_t)data[i+5];
    hash *= prime;
    hash ^= (uint64_t)data[i+6];
    hash *= prime;
    hash ^= (uint64_t)data[i+7];
    hash *= prime;
  }
  
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  return hash;
}

#ifdef USE_SSE2
static void blend_pixels_simd(uint8_t* out, const uint8_t* src, const uint8_t* dst, size_t pixels) {
  size_t i = 0;
  __m128i zero = _mm_setzero_si128();
  __m128i c128 = _mm_set1_epi16(128);
  __m128i c255 = _mm_set1_epi16(255);
  __m128i alpha_mask = _mm_set1_epi32(0xFF000000);
  
  for (; i + 3 < pixels; i += 4) {
    __m128i src_pixels = _mm_loadu_si128((__m128i*)(src + i * 4));
    __m128i dst_pixels = _mm_loadu_si128((__m128i*)(dst + i * 4));
    
    __m128i src_lo = _mm_unpacklo_epi8(src_pixels, zero);
    __m128i src_hi = _mm_unpackhi_epi8(src_pixels, zero);
    __m128i dst_lo = _mm_unpacklo_epi8(dst_pixels, zero);
    __m128i dst_hi = _mm_unpackhi_epi8(dst_pixels, zero);
    
    __m128i alpha_lo = _mm_shufflehi_epi16(_mm_shufflelo_epi16(src_lo, 0xFF), 0xFF);
    __m128i alpha_hi = _mm_shufflehi_epi16(_mm_shufflelo_epi16(src_hi, 0xFF), 0xFF);
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
    
    _mm_storeu_si128((__m128i*)(out + i * 4), result);
  }
  
  for (; i < pixels; ++i) {
    size_t idx = i * 4;
    uint8_t a = src[idx + 3];
    out[idx] = blend_byte_fast(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#elif defined(USE_NEON)
static void blend_pixels_simd(uint8_t* out, const uint8_t* src, const uint8_t* dst, size_t pixels) {
  size_t i = 0;
  uint16x8_t c128 = vdupq_n_u16(128);
  uint16x8_t c255 = vdupq_n_u16(255);
  
  for (; i + 3 < pixels; i += 4) {
    uint8x16_t src_pixels = vld1q_u8(src + i * 4);
    uint8x16_t dst_pixels = vld1q_u8(dst + i * 4);
    
    uint16x8_t src_lo = vmovl_u8(vget_low_u8(src_pixels));
    uint16x8_t src_hi = vmovl_u8(vget_high_u8(src_pixels));
    uint16x8_t dst_lo = vmovl_u8(vget_low_u8(dst_pixels));
    uint16x8_t dst_hi = vmovl_u8(vget_high_u8(dst_pixels));
    
    uint16x8_t alpha_lo = vdupq_lane_u16(vget_low_u16(src_lo), 3);
    uint16x8_t alpha_hi = vdupq_lane_u16(vget_high_u16(src_hi), 3);
    uint16x8_t inv_alpha_lo = vsubq_u16(c255, alpha_lo);
    uint16x8_t inv_alpha_hi = vsubq_u16(c255, alpha_hi);
    
    uint16x8_t blend_lo = vmlaq_u16(vmulq_u16(dst_lo, inv_alpha_lo), src_lo, alpha_lo);
    uint16x8_t blend_hi = vmlaq_u16(vmulq_u16(dst_hi, inv_alpha_hi), src_hi, alpha_hi);
    
    blend_lo = vaddq_u16(blend_lo, c128);
    blend_hi = vaddq_u16(blend_hi, c128);
    blend_lo = vshrq_n_u16(vaddq_u16(blend_lo, vshrq_n_u16(blend_lo, 8)), 8);
    blend_hi = vshrq_n_u16(vaddq_u16(blend_hi, vshrq_n_u16(blend_hi, 8)), 8);
    
    uint8x16_t result = vcombine_u8(vmovn_u16(blend_lo), vmovn_u16(blend_hi));
    result = vsetq_lane_u8(255, result, 3);
    result = vsetq_lane_u8(255, result, 7);
    result = vsetq_lane_u8(255, result, 11);
    result = vsetq_lane_u8(255, result, 15);
    
    vst1q_u8(out + i * 4, result);
  }
  
  for (; i < pixels; ++i) {
    size_t idx = i * 4;
    uint8_t a = src[idx + 3];
    out[idx] = blend_byte_fast(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#else
static void blend_pixels_simd(uint8_t* out, const uint8_t* src, const uint8_t* dst, size_t pixels) {
  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4;
    uint8_t a = src[idx + 3];
    out[idx] = blend_byte_fast(src[idx], dst[idx], a);
    out[idx + 1] = blend_byte_fast(src[idx + 1], dst[idx + 1], a);
    out[idx + 2] = blend_byte_fast(src[idx + 2], dst[idx + 2], a);
    out[idx + 3] = 255;
  }
}
#endif

uint64_t run(const uint8_t* src, const uint8_t* dst, int width, int height, int iters) {
  size_t pixels = (size_t)width * (size_t)height;
  size_t buffer_size = pixels * 4;
  uint8_t* out = (uint8_t*)malloc(buffer_size);
  if (!out) return 0;
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    blend_pixels_simd(out, src, dst, pixels);
    hash = checksum_bytes(out, buffer_size);
  }
  
  free(out);
  return hash;
}