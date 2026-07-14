#include "interface.h"

#include "xxhash.h"
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#else
#define USE_AVX2 0
#endif

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  
#if USE_AVX2
  const __m256i newline_vec = _mm256_set1_epi8('\n');
#endif

  while (p < end) {
    const char* line_end = p;
    
#if USE_AVX2
    // Use AVX2 to scan for newlines in 32-byte chunks
    while (line_end + 32 <= end) {
      __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(line_end));
      __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
      int mask = _mm256_movemask_epi8(cmp);
      if (mask != 0) {
        line_end += __builtin_ctz(mask);
        goto found_newline;
      }
      line_end += 32;
    }
#endif
    
    // Scalar fallback for remaining bytes
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
#if USE_AVX2
found_newline:
#endif
    {
      size_t len = static_cast<size_t>(line_end - p);
      // XXH3_64bits is faster for longer lines
      uint64_t line_hash = (len >= 240) 
        ? XXH3_64bits(p, len)
        : static_cast<uint64_t>(XXH64(p, len, 0));
      acc ^= line_hash;
      acc *= 1099511628211ULL;
    }
    
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return acc;
}