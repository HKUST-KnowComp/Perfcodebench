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
    __m128i newline_vec = _mm_set1_epi8('\n');
    while (p < aligned_end) {
      __m128i data = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
      __m128i cmp = _mm_cmpeq_epi8(data, newline_vec);
      int mask = _mm_movemask_epi8(cmp);
      if (mask != 0) {
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
  
  constexpr size_t BATCH_THRESHOLD = 128;
  constexpr size_t MAX_BATCH = 8;
  
  XXH64_state_t state;
  
  while (p < end) {
    const char* line_start = p;
    const char* line_end = find_newline_simd(p, end);
    size_t line_len = line_end - line_start;
    
    if (line_len < BATCH_THRESHOLD && (end - line_end) > 1) {
      XXH64_reset(&state, 0);
      size_t batch_count = 0;
      
      while (batch_count < MAX_BATCH && p < end) {
        line_end = find_newline_simd(p, end);
        line_len = line_end - p;
        
        XXH64_update(&state, p, line_len);
        
        uint64_t line_hash = XXH64_digest(&state);
        acc ^= line_hash;
        acc *= 1099511628211ULL;
        
        XXH64_reset(&state, 0);
        batch_count++;
        
        p = (line_end < end) ? (line_end + 1) : line_end;
        
        if (p >= end || (end - p) < BATCH_THRESHOLD) break;
      }
    } else {
      uint64_t line_hash = XXH64(line_start, line_len, 0);
      acc ^= line_hash;
      acc *= 1099511628211ULL;
      
      p = (line_end < end) ? (line_end + 1) : line_end;
    }
  }
  
  return acc;
}
