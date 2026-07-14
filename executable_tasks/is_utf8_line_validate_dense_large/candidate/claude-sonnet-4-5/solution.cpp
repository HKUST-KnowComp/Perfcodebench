#include "interface.h"
#include <cstdint>

#ifdef __SSE4_2__
#include <emmintrin.h>
#include <smmintrin.h>
#endif

#ifdef __AVX2__
#include <immintrin.h>
#endif

static inline bool is_continuation(unsigned char c) {
  return (c & 0xC0U) == 0x80U;
}

static bool validate_utf8_optimized(const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  
#ifdef __AVX2__
  const __m256i ascii_mask = _mm256_set1_epi8(0x80);
  while (end - p >= 32) {
    __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
    if (_mm256_testz_si256(chunk, ascii_mask)) {
      p += 32;
      continue;
    }
    break;
  }
#elif defined(__SSE4_2__)
  const __m128i ascii_mask = _mm_set1_epi8(0x80);
  while (end - p >= 16) {
    __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
    if (_mm_testz_si128(chunk, ascii_mask)) {
      p += 16;
      continue;
    }
    break;
  }
#endif

  while (p < end) {
    unsigned char c = *p++;
    if (c <= 0x7F) continue;
    
    if (c < 0xC2 || c > 0xF4) return false;
    
    std::size_t remaining = end - p;
    
    if (c <= 0xDF) {
      if (remaining < 1 || !is_continuation(p[0])) return false;
      p += 1;
    } else if (c <= 0xEF) {
      if (remaining < 2) return false;
      if (c == 0xE0) {
        if (p[0] < 0xA0 || p[0] > 0xBF || !is_continuation(p[1])) return false;
      } else if (c == 0xED) {
        if (p[0] < 0x80 || p[0] > 0x9F || !is_continuation(p[1])) return false;
      } else {
        if (!is_continuation(p[0]) || !is_continuation(p[1])) return false;
      }
      p += 2;
    } else {
      if (remaining < 3) return false;
      if (c == 0xF0) {
        if (p[0] < 0x90 || p[0] > 0xBF || !is_continuation(p[1]) || !is_continuation(p[2])) return false;
      } else if (c == 0xF4) {
        if (p[0] < 0x80 || p[0] > 0x8F || !is_continuation(p[1]) || !is_continuation(p[2])) return false;
      } else {
        if (!is_continuation(p[0]) || !is_continuation(p[1]) || !is_continuation(p[2])) return false;
      }
      p += 3;
    }
  }
  return true;
}

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_end = p;
    
#ifdef __AVX2__
    const __m256i newline = _mm256_set1_epi8('\n');
    while (end - line_end >= 32) {
      __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(line_end));
      __m256i cmp = _mm256_cmpeq_epi8(chunk, newline);
      int mask = _mm256_movemask_epi8(cmp);
      if (mask) {
        line_end += __builtin_ctz(mask);
        break;
      }
      line_end += 32;
    }
#elif defined(__SSE4_2__)
    const __m128i newline = _mm_set1_epi8('\n');
    while (end - line_end >= 16) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(line_end));
      __m128i cmp = _mm_cmpeq_epi8(chunk, newline);
      int mask = _mm_movemask_epi8(cmp);
      if (mask) {
        line_end += __builtin_ctz(mask);
        break;
      }
      line_end += 16;
    }
#endif
    
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    if (validate_utf8_optimized(p, static_cast<std::size_t>(line_end - p))) {
      ++count;
    }
    
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return count;
}