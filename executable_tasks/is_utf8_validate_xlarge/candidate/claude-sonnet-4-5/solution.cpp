#include "interface.h"
#include <immintrin.h>
#include <cstdint>

#ifdef _OPENMP
#include <omp.h>
#endif

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
  
  // Scalar tail
  while (p < end) {
    if (*p++ & 0x80) return false;
  }
  return true;
}

// Optimized UTF-8 validation with SIMD fast paths
bool is_valid_utf8_optimized(const std::string& s) {
  if (s.empty()) return true;
  
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();
  const unsigned char* start = p;
  
  // Fast path: check if entire string is ASCII
  if (s.size() >= 16) {
    const unsigned char* scan_end = end - 15;
    const unsigned char* scan = p;
    bool all_ascii = true;
    
#ifdef __SSE4_2__
    while (scan < scan_end) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(scan));
      if (_mm_movemask_epi8(chunk) != 0) {
        all_ascii = false;
        break;
      }
      scan += 16;
    }
#else
    for (; scan < scan_end; scan += 16) {
      uint64_t w1 = *reinterpret_cast<const uint64_t*>(scan);
      uint64_t w2 = *reinterpret_cast<const uint64_t*>(scan + 8);
      if ((w1 | w2) & 0x8080808080808080ULL) {
        all_ascii = false;
        break;
      }
    }
#endif
    
    if (all_ascii) {
      // Check remaining bytes
      while (scan < end) {
        if (*scan++ & 0x80) {
          all_ascii = false;
          break;
        }
      }
      if (all_ascii) return true;
    }
  }
  
  // Full UTF-8 validation
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
      
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      
      if (c == 0xE0) {
        if (b1 < 0xA0 || b1 > 0xBF || (b2 & 0xC0U) != 0x80U) return false;
      } else if (c == 0xED) {
        if (b1 < 0x80 || b1 > 0x9F || (b2 & 0xC0U) != 0x80U) return false;
      } else {
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U) return false;
      }
      p += 2;
      continue;
    }
    
    // 4-byte sequences
    if (c >= 0xF0 && c <= 0xF4) {
      if (end - p < 3) return false;
      
      const unsigned char b1 = p[0];
      const unsigned char b2 = p[1];
      const unsigned char b3 = p[2];
      
      if (c == 0xF0) {
        if (b1 < 0x90 || b1 > 0xBF || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
      } else if (c == 0xF4) {
        if (b1 < 0x80 || b1 > 0x8F || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
      } else {
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
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
  const size_t n = inputs.size();
  if (n == 0) return 0;
  
  std::size_t count = 0;
  
#ifdef _OPENMP
  // Use parallel processing for large datasets
  if (n >= 1000) {
    #pragma omp parallel for reduction(+:count) schedule(dynamic, 64)
    for (size_t i = 0; i < n; ++i) {
      count += is_valid_utf8_optimized(inputs[i]) ? 1U : 0U;
    }
    return count;
  }
#endif
  
  // Serial processing for smaller datasets
  for (const auto& s : inputs) {
    count += is_valid_utf8_optimized(s) ? 1U : 0U;
  }
  return count;
}