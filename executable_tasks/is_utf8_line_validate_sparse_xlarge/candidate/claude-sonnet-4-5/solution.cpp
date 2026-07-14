#include "interface.h"
#include <cstdint>

#ifdef __SSE4_2__
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#ifdef __AVX2__
#include <immintrin.h>
#endif

std::size_t count_valid_lines(const std::string& input) {
  auto valid_simd = [](const char* data, std::size_t len) -> bool {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = p + len;
    
#ifdef __AVX2__
    // Fast path: process 32 bytes at a time for ASCII
    while (end - p >= 32) {
      __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
      int mask = _mm256_movemask_epi8(chunk);
      if (mask == 0) {
        p += 32;
        continue;
      }
      // Found non-ASCII, break to scalar processing
      break;
    }
#elif defined(__SSE4_2__)
    // Fast path: process 16 bytes at a time for ASCII
    while (end - p >= 16) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
      int mask = _mm_movemask_epi8(chunk);
      if (mask == 0) {
        p += 16;
        continue;
      }
      break;
    }
#endif
    
    // Scalar processing with optimized validation
    while (p < end) {
      unsigned char c = *p++;
      
      // ASCII fast path
      if (c <= 0x7F) continue;
      
      // 2-byte sequence
      if (c <= 0xDF) {
        if (c < 0xC2 || p >= end) return false;
        if ((*p++ & 0xC0) != 0x80) return false;
        continue;
      }
      
      // 3-byte sequence
      if (c <= 0xEF) {
        if (end - p < 2) return false;
        unsigned char b1 = *p++;
        unsigned char b2 = *p++;
        
        if (c == 0xE0) {
          if (b1 < 0xA0 || b1 > 0xBF || (b2 & 0xC0) != 0x80) return false;
        } else if (c == 0xED) {
          if (b1 < 0x80 || b1 > 0x9F || (b2 & 0xC0) != 0x80) return false;
        } else {
          if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) return false;
        }
        continue;
      }
      
      // 4-byte sequence
      if (c <= 0xF4) {
        if (end - p < 3) return false;
        unsigned char b1 = *p++;
        unsigned char b2 = *p++;
        unsigned char b3 = *p++;
        
        if (c == 0xF0) {
          if (b1 < 0x90 || b1 > 0xBF || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return false;
        } else if (c == 0xF4) {
          if (b1 < 0x80 || b1 > 0x8F || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return false;
        } else {
          if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return false;
        }
        continue;
      }
      
      return false;
    }
    return true;
  };

  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    // Find line end using optimized search
    const char* line_end = p;
    const char* search_end = end;
    
#ifdef __AVX2__
    // SIMD newline search for long lines
    __m256i newline_vec = _mm256_set1_epi8('\n');
    while (search_end - line_end >= 32) {
      __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(line_end));
      __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
      int mask = _mm256_movemask_epi8(cmp);
      if (mask != 0) {
        line_end += __builtin_ctz(mask);
        goto found_newline;
      }
      line_end += 32;
    }
#elif defined(__SSE4_2__)
    __m128i newline_vec = _mm_set1_epi8('\n');
    while (search_end - line_end >= 16) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(line_end));
      __m128i cmp = _mm_cmpeq_epi8(chunk, newline_vec);
      int mask = _mm_movemask_epi8(cmp);
      if (mask != 0) {
        line_end += __builtin_ctz(mask);
        goto found_newline;
      }
      line_end += 16;
    }
#endif
    
    // Scalar search for remainder
    while (line_end < search_end && *line_end != '\n') {
      ++line_end;
    }
    
found_newline:
    count += valid_simd(p, static_cast<std::size_t>(line_end - p));
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return count;
}