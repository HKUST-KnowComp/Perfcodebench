#include "interface.h"

#include "xxhash.h"
#include <cstring>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

static inline const char* find_newline_simd(const char* p, const char* end) {
#ifdef __SSE2__
  const char* aligned_end = end - 15;
  if (p < aligned_end) {
    __m128i newline = _mm_set1_epi8('\n');
    while (p < aligned_end) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
      __m128i cmp = _mm_cmpeq_epi8(chunk, newline);
      int mask = _mm_movemask_epi8(cmp);
      if (mask) {
        return p + __builtin_ctz(mask);
      }
      p += 16;
    }
  }
#endif
  while (p < end && *p != '\n') {
    ++p;
  }
  return p;
}

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  constexpr uint64_t FNV_PRIME = 1099511628211ULL;
  
  while (p < end) {
    const char* line_end = find_newline_simd(p, end);
    std::size_t len = static_cast<std::size_t>(line_end - p);
    
    uint64_t h1 = XXH64(p, len, 0);
    acc ^= h1;
    acc *= FNV_PRIME;
    
    p = line_end;
    if (p < end && *p == '\n') {
      ++p;
      
      if (p < end) {
        line_end = find_newline_simd(p, end);
        len = static_cast<std::size_t>(line_end - p);
        uint64_t h2 = XXH64(p, len, 0);
        acc ^= h2;
        acc *= FNV_PRIME;
        p = line_end;
        if (p < end && *p == '\n') {
          ++p;
        }
      }
    }
  }
  
  return acc;
}