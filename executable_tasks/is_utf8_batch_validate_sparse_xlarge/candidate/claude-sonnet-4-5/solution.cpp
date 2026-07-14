#include "interface.h"
#include <cstdint>

#ifdef __SSE4_2__
#include <emmintrin.h>
#include <smmintrin.h>
#endif

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  // Lookup table for UTF-8 sequence lengths based on first byte
  static const uint8_t utf8_length[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x00-0x0F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x10-0x1F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x20-0x2F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x30-0x3F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x40-0x4F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x50-0x5F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x60-0x6F
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0x70-0x7F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x80-0x8F (invalid)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x90-0x9F (invalid)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xA0-0xAF (invalid)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0xB0-0xBF (invalid)
    0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xC0-0xCF
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xD0-0xDF
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, // 0xE0-0xEF
    4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0  // 0xF0-0xFF
  };

  auto valid_optimized = [&](const std::string& s) -> bool {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
    const uint8_t* end = p + s.size();
    
#ifdef __SSE4_2__
    // Fast path: process ASCII in 16-byte chunks using SIMD
    while (end - p >= 16) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
      int mask = _mm_movemask_epi8(chunk);
      if (mask == 0) {
        p += 16;
        continue;
      }
      // Found non-ASCII, process byte by byte from here
      break;
    }
#else
    // Scalar fast path: process 8 bytes at a time for ASCII
    while (end - p >= 8) {
      uint64_t chunk;
      __builtin_memcpy(&chunk, p, 8);
      if ((chunk & 0x8080808080808080ULL) == 0) {
        p += 8;
        continue;
      }
      break;
    }
#endif

    // Process remaining bytes
    while (p < end) {
      uint8_t c = *p;
      uint8_t len = utf8_length[c];
      
      if (len == 0) return false;
      if (len == 1) {
        ++p;
        continue;
      }
      
      if (end - p < len) return false;
      
      if (len == 2) {
        if ((p[1] & 0xC0) != 0x80) return false;
        p += 2;
      } else if (len == 3) {
        uint8_t b1 = p[1], b2 = p[2];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) return false;
        
        if (c == 0xE0) {
          if (b1 < 0xA0 || b1 > 0xBF) return false;
        } else if (c == 0xED) {
          if (b1 < 0x80 || b1 > 0x9F) return false;
        }
        p += 3;
      } else { // len == 4
        uint8_t b1 = p[1], b2 = p[2], b3 = p[3];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return false;
        
        if (c == 0xF0) {
          if (b1 < 0x90 || b1 > 0xBF) return false;
        } else if (c == 0xF4) {
          if (b1 < 0x80 || b1 > 0x8F) return false;
        }
        p += 4;
      }
    }
    return true;
  };

  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += valid_optimized(s);
  }
  return count;
}
