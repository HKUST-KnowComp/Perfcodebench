#include "interface.h"
#include <emmintrin.h>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  
  for (const auto& s : inputs) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
    const unsigned char* end = p + s.size();
    bool valid = true;
    
    // Fast path: SIMD for ASCII-heavy content
    while (end - p >= 16) {
      __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
      int mask = _mm_movemask_epi8(chunk);
      
      if (mask == 0) {
        // All ASCII
        p += 16;
        continue;
      }
      
      // Has non-ASCII, process byte-by-byte
      break;
    }
    
    // Scalar validation for remaining bytes and multibyte sequences
    while (p < end && valid) {
      const unsigned char c = *p++;
      
      if (c <= 0x7F) continue;
      
      if (c >= 0xC2 && c <= 0xDF) {
        if (p >= end || (*p & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        ++p;
      } else if (c == 0xE0) {
        if (end - p < 2) {
          valid = false;
          break;
        }
        unsigned char c1 = p[0];
        unsigned char c2 = p[1];
        if (c1 < 0xA0 || c1 > 0xBF || (c2 & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 2;
      } else if (c >= 0xE1 && c <= 0xEC) {
        if (end - p < 2) {
          valid = false;
          break;
        }
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 2;
      } else if (c == 0xED) {
        if (end - p < 2) {
          valid = false;
          break;
        }
        unsigned char c1 = p[0];
        if (c1 < 0x80 || c1 > 0x9FU || (p[1] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 2;
      } else if (c >= 0xEE && c <= 0xEF) {
        if (end - p < 2) {
          valid = false;
          break;
        }
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 2;
      } else if (c == 0xF0) {
        if (end - p < 3) {
          valid = false;
          break;
        }
        unsigned char c1 = p[0];
        if (c1 < 0x90 || c1 > 0xBF || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 3;
      } else if (c >= 0xF1 && c <= 0xF3) {
        if (end - p < 3) {
          valid = false;
          break;
        }
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 3;
      } else if (c == 0xF4) {
        if (end - p < 3) {
          valid = false;
          break;
        }
        unsigned char c1 = p[0];
        if (c1 < 0x80 || c1 > 0x8FU || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) {
          valid = false;
          break;
        }
        p += 3;
      } else {
        valid = false;
        break;
      }
    }
    
    count += valid ? 1U : 0U;
  }
  
  return count;
}