#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SIMD 1
#else
#define HAS_SIMD 0
#endif

namespace {

// Optimized blend: (s*a + d*(255-a) + 127) / 255
// Using fixed-point arithmetic for better performance
inline uint8_t blend_byte_scalar(uint8_t s, uint8_t d, uint8_t a) {
  uint32_t sa = s * a;
  uint32_t da = d * (255u - a);
  return static_cast<uint8_t>((sa + da + 127u) / 255u);
}

#if HAS_SIMD && (defined(__SSE2__) || defined(__AVX2__))

// SIMD blend for 4 pixels at once using SSE2
inline void blend_4pixels_sse2(const uint8_t* src, const uint8_t* dst, uint8_t* out) {
  __m128i s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(src));
  __m128i d = _mm_loadu_si128(reinterpret_cast<const __m128i*>(dst));
  
  // Extract alpha channels (every 4th byte starting at offset 3)
  __m128i alpha_mask = _mm_set_epi8(15, 15, 15, 15, 11, 11, 11, 11, 7, 7, 7, 7, 3, 3, 3, 3);
  __m128i a = _mm_shuffle_epi8(s, alpha_mask);
  
  // Convert to 16-bit for multiplication
  __m128i s_lo = _mm_unpacklo_epi8(s, _mm_setzero_si128());
  __m128i s_hi = _mm_unpackhi_epi8(s, _mm_setzero_si128());
  __m128i d_lo = _mm_unpacklo_epi8(d, _mm_setzero_si128());
  __m128i d_hi = _mm_unpackhi_epi8(d, _mm_setzero_si128());
  __m128i a_lo = _mm_unpacklo_epi8(a, _mm_setzero_si128());
  __m128i a_hi = _mm_unpackhi_epi8(a, _mm_setzero_si128());
  
  __m128i inv_a_lo = _mm_sub_epi16(_mm_set1_epi16(255), a_lo);
  __m128i inv_a_hi = _mm_sub_epi16(_mm_set1_epi16(255), a_hi);
  
  // sa + da + 127
  __m128i sa_lo = _mm_mullo_epi16(s_lo, a_lo);
  __m128i sa_hi = _mm_mullo_epi16(s_hi, a_hi);
  __m128i da_lo = _mm_mullo_epi16(d_lo, inv_a_lo);
  __m128i da_hi = _mm_mullo_epi16(d_hi, inv_a_hi);
  
  __m128i sum_lo = _mm_add_epi16(_mm_add_epi16(sa_lo, da_lo), _mm_set1_epi16(127));
  __m128i sum_hi = _mm_add_epi16(_mm_add_epi16(sa_hi, da_hi), _mm_set1_epi16(127));
  
  // Divide by 255 using multiply by reciprocal: x/255 ≈ (x*257)>>16
  sum_lo = _mm_mulhi_epu16(sum_lo, _mm_set1_epi16(257));
  sum_hi = _mm_mulhi_epu16(sum_hi, _mm_set1_epi16(257));
  
  // Pack back to 8-bit
  __m128i result = _mm_packus_epi16(sum_lo, sum_hi);
  
  // Set alpha to 255
  __m128i alpha_ff = _mm_set_epi8(255, -1, -1, -1, 255, -1, -1, -1, 255, -1, -1, -1, 255, -1, -1, -1);
  result = _mm_or_si128(_mm_andnot_si128(_mm_set_epi8(255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0), result),
                        _mm_and_si128(alpha_ff, _mm_set_epi8(255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0)));
  result = _mm_or_si128(result, _mm_set_epi8(255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0, 255, 0, 0, 0));
  
  _mm_storeu_si128(reinterpret_cast<__m128i*>(out), result);
}

#endif

// Optimized FNV-1a checksum with loop unrolling
uint64_t checksum_bytes_fast(const uint8_t* bytes, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  // Process 8 bytes at a time
  size_t i = 0;
  for (; i + 7 < len; i += 8) {
    hash ^= bytes[i];
    hash *= prime;
    hash ^= bytes[i+1];
    hash *= prime;
    hash ^= bytes[i+2];
    hash *= prime;
    hash ^= bytes[i+3];
    hash *= prime;
    hash ^= bytes[i+4];
    hash *= prime;
    hash ^= bytes[i+5];
    hash *= prime;
    hash ^= bytes[i+6];
    hash *= prime;
    hash ^= bytes[i+7];
    hash *= prime;
  }
  
  // Handle remaining bytes
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
  
  const size_t pixels = src.size() / 4u;
  const size_t out_size = pixels * 4u;
  
  // Allocate output buffer once
  std::vector<uint8_t> out(out_size);
  uint8_t* out_ptr = out.data();
  const uint8_t* src_ptr = src.data();
  const uint8_t* dst_ptr = dst.data();
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0;
    
#if HAS_SIMD && defined(__SSSE3__)
    // Process 4 pixels (16 bytes) at a time with SIMD
    for (; i + 3 < pixels; i += 4) {
      blend_4pixels_sse2(src_ptr + i * 4, dst_ptr + i * 4, out_ptr + i * 4);
    }
#endif
    
    // Process remaining pixels with scalar code
    for (; i < pixels; ++i) {
      const size_t idx = i * 4u;
      const uint8_t a = src_ptr[idx + 3u];
      out_ptr[idx] = blend_byte_scalar(src_ptr[idx], dst_ptr[idx], a);
      out_ptr[idx + 1u] = blend_byte_scalar(src_ptr[idx + 1u], dst_ptr[idx + 1u], a);
      out_ptr[idx + 2u] = blend_byte_scalar(src_ptr[idx + 2u], dst_ptr[idx + 2u], a);
      out_ptr[idx + 3u] = 255u;
    }
    
    hash = checksum_bytes_fast(out_ptr, out_size);
  }
  
  return hash;
}
