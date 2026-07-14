#include "interface.h"
#include <immintrin.h>
#include <cstring>

namespace {

// Fast ASCII check using SIMD
inline bool is_ascii_simd(const unsigned char* data, size_t len) {
  const unsigned char* p = data;
  const unsigned char* end = data + len;
  
#ifdef __AVX2__
  // Process 32 bytes at a time with AVX2
  while (end - p >= 32) {
    __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
    if (_mm256_movemask_epi8(chunk) != 0) return false;
    p += 32;
  }
#endif
  
#ifdef __SSE4_2__
  // Process 16 bytes at a time with SSE
  while (end - p >= 16) {
    __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
    if (_mm_movemask_epi8(chunk) != 0) return false;
    p += 16;
  }
#endif
  
  // Handle remaining bytes
  while (p < end) {
    if (*p++ & 0x80) return false;
  }
  return true;
}

inline bool is_valid_utf8_optimized(const std::string& s) {
  if (s.empty()) return true;
  
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();
  const unsigned char* start = p;
  
  // Fast path: check if entire string is ASCII
  if (s.size() >= 16 && is_ascii_simd(p, s.size())) {
    return true;
  }
  
  // Mixed content - process byte by byte
  p = start;
  while (p < end) {
    const unsigned char c = *p++;
    
    // ASCII fast path
    if (c <= 0x7F) continue;
    
    // 2-byte sequence
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || (*p & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }
    
    // 3-byte sequences
    if (c >= 0xE0 && c <= 0xEF) {
      if (end - p < 2) return false;
      
      if (c == 0xE0) {
        if (p[0] < 0xA0 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U) return false;
      } else if (c == 0xED) {
        if (p[0] < 0x80 || p[0] > 0x9F || (p[1] & 0xC0U) != 0x80U) return false;
      } else {
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
      }
      p += 2;
      continue;
    }
    
    // 4-byte sequences
    if (c >= 0xF0 && c <= 0xF4) {
      if (end - p < 3) return false;
      
      if (c == 0xF0) {
        if (p[0] < 0x90 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      } else if (c == 0xF4) {
        if (p[0] < 0x80 || p[0] > 0x8F || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      } else {
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      }
      p += 3;
      continue;
    }
    
    return false;
  }
  return true;
}

}  // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const size_t n = inputs.size();
  
  // Use parallel processing for large datasets
  if (n >= 1000) {
    #pragma omp parallel for reduction(+:count) schedule(static)
    for (size_t i = 0; i < n; ++i) {
      count += is_valid_utf8_optimized(inputs[i]) ? 1U : 0U;
    }
  } else {
    // Sequential for small datasets to avoid threading overhead
    for (const auto& s : inputs) {
      count += is_valid_utf8_optimized(s) ? 1U : 0U;
    }
  }
  
  return count;
}
